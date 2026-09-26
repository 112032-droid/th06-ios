#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <direct.h>
#include <new>
#include <windows.h>
#elif __cplusplus >= 201703L
#include <filesystem>
#else
#include <sys/stat.h>
#endif

#include "FileSystem.hpp"
#include "pbg3/Pbg3Archive.hpp"
#include "utils.hpp"

u32 g_LastFileSize;

FILE *FileSystem::FopenUTF8(const char *filepath, const char *mode)
{
#ifndef _WIN32
    return std::fopen(filepath, mode);
#else
    u32 filepathWLen =
        MultiByteToWideChar(CP_UTF8, 0, filepath, -1, NULL, 0) * 2;

    u32 modeWLen =
        MultiByteToWideChar(CP_UTF8, 0, mode, -1, NULL, 0) * 2;

    if (filepathWLen == 0 || modeWLen == 0)
    {
        return NULL;
    }

    wchar_t *filepathW = new wchar_t[filepathWLen];
    wchar_t *modeW = new wchar_t[modeWLen];

    MultiByteToWideChar(
        CP_UTF8, 0,
        filepath, -1,
        filepathW, filepathWLen / 2
    );

    MultiByteToWideChar(
        CP_UTF8, 0,
        mode, -1,
        modeW, modeWLen / 2
    );

    FILE *f = _wfopen(filepathW, modeW);

    delete[] filepathW;
    delete[] modeW;

    return f;
#endif
}

void FileSystem::CreateDir(const char *path)
{
#ifdef _WIN32
    _mkdir(path);
#elif __cplusplus >= 201703L
    auto p = std::filesystem::path(path);
    std::filesystem::create_directory(p);
#else
    mkdir(path, 0755);
#endif
}

u8 *FileSystem::OpenPath(const char *filepath, int isExternalResource)
{
    u8 *data = NULL;
    FILE *file = NULL;
    size_t fsize = 0;

    i32 entryIdx = -1;
    i32 pbg3Idx = -1;

    const char *entryname = filepath;

    /*
     * ------------------------------------------------------------
     * External resource
     * ------------------------------------------------------------
     *
     * External resources are loaded directly from the filesystem.
     */
    if (isExternalResource != 0)
    {
        utils::DebugPrint2(
            "[FileSystem] External resource: %s\n",
            filepath
        );

        file = FopenUTF8(filepath, "rb");

        if (file == NULL)
        {
            utils::DebugPrint2(
                "[FileSystem] ERROR: %s is not found.\n",
                filepath
            );

            return NULL;
        }

        std::fseek(file, 0, SEEK_END);
        fsize = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);

        g_LastFileSize = (u32)fsize;

        data = (u8 *)std::malloc(fsize);

        if (data == NULL)
        {
            std::fclose(file);
            return NULL;
        }

        std::fread(data, 1, fsize, file);
        std::fclose(file);

        return data;
    }

    /*
     * ------------------------------------------------------------
     * Search inside PBG3 archives
     * ------------------------------------------------------------
     *
     * IMPORTANT:
     *
     * The old code stripped:
     *
     *     data/wav/plst00.wav
     *
     * into:
     *
     *     plst00.wav
     *
     * before calling FindEntry().
     *
     * Some TH06 PBG3 archives store the complete virtual path,
     * so we must try the complete path first.
     */

    if (g_Pbg3Archives != NULL)
    {
        char normalizedPath[1024];
        char windowsPath[1024];
        char basename[1024];

        std::snprintf(
            normalizedPath,
            sizeof(normalizedPath),
            "%s",
            filepath
        );

        std::snprintf(
            windowsPath,
            sizeof(windowsPath),
            "%s",
            filepath
        );

        std::snprintf(
            basename,
            sizeof(basename),
            "%s",
            filepath
        );

        /*
         * Convert separators.
         */
        for (size_t i = 0; normalizedPath[i] != '\0'; ++i)
        {
            if (normalizedPath[i] == '\\')
            {
                normalizedPath[i] = '/';
            }
        }

        for (size_t i = 0; windowsPath[i] != '\0'; ++i)
        {
            if (windowsPath[i] == '/')
            {
                windowsPath[i] = '\\';
            }
        }

        /*
         * Extract basename.
         */
        const char *slash1 = std::strrchr(basename, '/');
        const char *slash2 = std::strrchr(basename, '\\');

        const char *lastSlash = slash1;

        if (slash2 != NULL &&
            (lastSlash == NULL || slash2 > lastSlash))
        {
            lastSlash = slash2;
        }

        if (lastSlash != NULL)
        {
            std::memmove(
                basename,
                lastSlash + 1,
                std::strlen(lastSlash + 1) + 1
            );
        }

        utils::DebugPrint2(
            "[FileSystem] Search resource: %s\n",
            filepath
        );

        /*
         * Search every mounted PBG3 archive.
         */
        for (pbg3Idx = 0; pbg3Idx < 0x10; ++pbg3Idx)
        {
            if (g_Pbg3Archives[pbg3Idx] == NULL)
            {
                continue;
            }

            /*
             * 1. Try normal Unix-style path.
             */
            entryIdx =
                g_Pbg3Archives[pbg3Idx]->FindEntry(normalizedPath);

            if (entryIdx >= 0)
            {
                entryname = normalizedPath;

                utils::DebugPrint2(
                    "[FileSystem] FOUND: %s in PBG3 #%d\n",
                    entryname,
                    pbg3Idx
                );

                break;
            }

            /*
             * 2. Try Windows-style path.
             */
            entryIdx =
                g_Pbg3Archives[pbg3Idx]->FindEntry(windowsPath);

            if (entryIdx >= 0)
            {
                entryname = windowsPath;

                utils::DebugPrint2(
                    "[FileSystem] FOUND: %s in PBG3 #%d\n",
                    entryname,
                    pbg3Idx
                );

                break;
            }

            /*
             * 3. Finally try basename.
             */
            entryIdx =
                g_Pbg3Archives[pbg3Idx]->FindEntry(basename);

            if (entryIdx >= 0)
            {
                entryname = basename;

                utils::DebugPrint2(
                    "[FileSystem] FOUND basename: %s in PBG3 #%d\n",
                    entryname,
                    pbg3Idx
                );

                break;
            }
        }
    }

    /*
     * ------------------------------------------------------------
     * Load from PBG3
     * ------------------------------------------------------------
     */
    if (entryIdx >= 0 && pbg3Idx >= 0)
    {
        utils::DebugPrint2(
            "[FileSystem] Decode: %s\n",
            entryname
        );

        data =
            g_Pbg3Archives[pbg3Idx]->ReadDecompressEntry(
                entryIdx,
                entryname
            );

        if (data == NULL)
        {
            utils::DebugPrint2(
                "[FileSystem] ERROR: failed to decompress %s\n",
                entryname
            );

            return NULL;
        }

        g_LastFileSize =
            g_Pbg3Archives[pbg3Idx]->GetEntrySize(entryIdx);

        return data;
    }

    /*
     * ------------------------------------------------------------
     * PBG3 search failed.
     *
     * Try physical file as a fallback.
     * ------------------------------------------------------------
     */
    utils::DebugPrint2(
        "[FileSystem] Not found in PBG3: %s\n",
        filepath
    );

    utils::DebugPrint2(
        "[FileSystem] Trying physical file: %s\n",
        filepath
    );

    file = FopenUTF8(filepath, "rb");

    if (file == NULL)
    {
        utils::DebugPrint2(
            "[FileSystem] ERROR: %s is not found.\n",
            filepath
        );

        return NULL;
    }

    std::fseek(file, 0, SEEK_END);
    fsize = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);

    g_LastFileSize = (u32)fsize;

    data = (u8 *)std::malloc(fsize);

    if (data == NULL)
    {
        std::fclose(file);
        return NULL;
    }

    std::fread(data, 1, fsize, file);
    std::fclose(file);

    utils::DebugPrint2(
        "[FileSystem] Loaded physical file: %s\n",
        filepath
    );

    return data;
}

int FileSystem::WriteDataToFile(
    const char *path,
    const void *data,
    size_t size)
{
    FILE *f;

    f = FopenUTF8(path, "wb");

    if (f == NULL)
    {
        return -1;
    }

    if (std::fwrite(data, 1, size, f) != size)
    {
        std::fclose(f);
        return -2;
    }

    std::fclose(f);

    return 0;
}
