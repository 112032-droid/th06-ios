#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

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


/* ============================================================
 * Runtime log
 *
 * iOS:
 *     <App Container>/Documents/log.txt
 *
 * This is DIFFERENT from the log.txt bundled inside the .app.
 * ============================================================ */

static void TH06FileSystemLog(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    // Always print to normal console output.
    std::vprintf(fmt, args);
    std::printf("\n");

    va_end(args);

    const char *home = std::getenv("HOME");

    if (home == NULL)
        return;

    char logPath[1024];

    std::snprintf(
        logPath,
        sizeof(logPath),
        "%s/Documents/log.txt",
        home
    );

    FILE *logFile = std::fopen(logPath, "ab");

    if (logFile == NULL)
        return;

    va_start(args, fmt);

    std::vfprintf(logFile, fmt, args);
    std::fprintf(logFile, "\n");

    va_end(args);

    std::fclose(logFile);
}


/* ============================================================
 * UTF-8 fopen
 * ============================================================ */

FILE *FileSystem::FopenUTF8(const char *filename, const char *mode)
{
#ifdef _WIN32

    if (filename == NULL || mode == NULL)
        return NULL;

    int wFilenameSize =
        MultiByteToWideChar(
            CP_UTF8,
            0,
            filename,
            -1,
            NULL,
            0
        );

    int wModeSize =
        MultiByteToWideChar(
            CP_UTF8,
            0,
            mode,
            -1,
            NULL,
            0
        );

    if (wFilenameSize <= 0 || wModeSize <= 0)
        return NULL;

    wchar_t *wFilename =
        new wchar_t[wFilenameSize];

    wchar_t *wMode =
        new wchar_t[wModeSize];

    if (wFilename == NULL || wMode == NULL)
    {
        delete[] wFilename;
        delete[] wMode;
        return NULL;
    }

    MultiByteToWideChar(
        CP_UTF8,
        0,
        filename,
        -1,
        wFilename,
        wFilenameSize
    );

    MultiByteToWideChar(
        CP_UTF8,
        0,
        mode,
        -1,
        wMode,
        wModeSize
    );

    FILE *file =
        _wfopen(
            wFilename,
            wMode
        );

    delete[] wFilename;
    delete[] wMode;

    return file;

#else

    return std::fopen(filename, mode);

#endif
}


/* ============================================================
 * Create directory
 * ============================================================ */

void FileSystem::CreateDir(const char *path)
{
    if (path == NULL)
        return;

#ifdef _WIN32

    _mkdir(path);

#elif __cplusplus >= 201703L

    try
    {
        std::filesystem::create_directories(path);
    }
    catch (...)
    {
        // Ignore directory creation failure.
    }

#else

    mkdir(path, 0755);

#endif
}


/* ============================================================
 * OpenPath
 * ============================================================ */

u8 *FileSystem::OpenPath(
    const char *filepath,
    i32 isExternalResource
)
{
    u8 *data = NULL;

    FILE *file = NULL;

    size_t fsize = 0;

    i32 entryIdx = -1;
    i32 pbg3Idx = -1;

    const char *entryname = filepath;


    /* ========================================================
     * Invalid path
     * ======================================================== */

    if (filepath == NULL)
    {
        TH06FileSystemLog(
            "[FileSystem] ERROR: NULL filepath"
        );

        return NULL;
    }


    TH06FileSystemLog(
        "[FileSystem] OpenPath: [%s] external=%d",
        filepath,
        isExternalResource
    );


    /* ========================================================
     * External resource
     *
     * These files are read directly from the filesystem.
     * ======================================================== */

    if (isExternalResource != 0)
    {
        utils::DebugPrint2(
            "[FileSystem] External resource: %s\n",
            filepath
        );

        TH06FileSystemLog(
            "[FileSystem] External resource: %s",
            filepath
        );

        file =
            FopenUTF8(
                filepath,
                "rb"
            );

        if (file == NULL)
        {
            TH06FileSystemLog(
                "[FileSystem] External file OPEN FAILED: %s",
                filepath
            );

            return NULL;
        }

        if (std::fseek(file, 0, SEEK_END) != 0)
        {
            TH06FileSystemLog(
                "[FileSystem] External file SEEK FAILED: %s",
                filepath
            );

            std::fclose(file);
            return NULL;
        }

        long size =
            std::ftell(file);

        if (size < 0)
        {
            TH06FileSystemLog(
                "[FileSystem] External file FTELL FAILED: %s",
                filepath
            );

            std::fclose(file);
            return NULL;
        }

        fsize =
            (size_t)size;

        if (std::fseek(file, 0, SEEK_SET) != 0)
        {
            TH06FileSystemLog(
                "[FileSystem] External file SEEK RESET FAILED: %s",
                filepath
            );

            std::fclose(file);
            return NULL;
        }

        data =
            (u8 *)std::malloc(fsize);

        if (data == NULL)
        {
            TH06FileSystemLog(
                "[FileSystem] External malloc FAILED size=%zu",
                fsize
            );

            std::fclose(file);
            return NULL;
        }

        if (fsize > 0)
        {
            size_t readSize =
                std::fread(
                    data,
                    1,
                    fsize,
                    file
                );

            if (readSize != fsize)
            {
                TH06FileSystemLog(
                    "[FileSystem] External fread FAILED expected=%zu actual=%zu",
                    fsize,
                    readSize
                );

                std::free(data);
                std::fclose(file);

                return NULL;
            }
        }

        std::fclose(file);

        g_LastFileSize =
            (u32)fsize;

        TH06FileSystemLog(
            "[FileSystem] External file SUCCESS: %s size=%u",
            filepath,
            g_LastFileSize
        );

        return data;
    }


    /* ========================================================
     * Search PBG3 archives
     * ======================================================== */

    if (g_Pbg3Archives != NULL)
    {
        char normalizedPath[1024];

        char windowsPath[1024];

        char basename[1024];


        /* ----------------------------------------------------
         * Normalized Unix path
         * ---------------------------------------------------- */

        std::snprintf(
            normalizedPath,
            sizeof(normalizedPath),
            "%s",
            filepath
        );

        for (size_t i = 0;
             normalizedPath[i] != '\0';
             ++i)
        {
            if (normalizedPath[i] == '\\')
                normalizedPath[i] = '/';
        }


        /* ----------------------------------------------------
         * Windows path
         * ---------------------------------------------------- */

        std::snprintf(
            windowsPath,
            sizeof(windowsPath),
            "%s",
            filepath
        );

        for (size_t i = 0;
             windowsPath[i] != '\0';
             ++i)
        {
            if (windowsPath[i] == '/')
                windowsPath[i] = '\\';
        }


        /* ----------------------------------------------------
         * Basename
         *
         * Example:
         *
         * data/wav/plst00.wav
         *
         * becomes:
         *
         * plst00.wav
         * ---------------------------------------------------- */

        const char *basePtr =
            std::strrchr(
                normalizedPath,
                '/'
            );

        if (basePtr != NULL)
        {
            basePtr++;
        }
        else
        {
            basePtr =
                normalizedPath;
        }

        std::snprintf(
            basename,
            sizeof(basename),
            "%s",
            basePtr
        );


        utils::DebugPrint2(
            "[FileSystem] Search resource: %s\n",
            filepath
        );

        TH06FileSystemLog(
            "[FileSystem] Search resource: %s",
            filepath
        );

        TH06FileSystemLog(
            "[FileSystem] normalized=[%s]",
            normalizedPath
        );

        TH06FileSystemLog(
            "[FileSystem] windows=[%s]",
            windowsPath
        );

        TH06FileSystemLog(
            "[FileSystem] basename=[%s]",
            basename
        );


        /* ----------------------------------------------------
         * Important resources
         * ---------------------------------------------------- */

        if (std::strcmp(filepath, "data/text.anm") == 0 ||
            std::strcmp(filepath, "text.anm") == 0)
        {
            TH06FileSystemLog(
                "[FileSystem] !!! IMPORTANT RESOURCE text.anm !!!"
            );
        }

        if (std::strcmp(filepath, "data/wav/plst00.wav") == 0 ||
            std::strcmp(filepath, "plst00.wav") == 0)
        {
            TH06FileSystemLog(
                "[FileSystem] !!! IMPORTANT RESOURCE plst00.wav !!!"
            );
        }

        if (std::strcmp(filepath, "data/wav/pldead00.wav") == 0 ||
            std::strcmp(filepath, "pldead00.wav") == 0)
        {
            TH06FileSystemLog(
                "[FileSystem] !!! IMPORTANT RESOURCE plde ad00.wav !!!"
            );
        }


        /* ----------------------------------------------------
         * Search all possible PBG3 archives
         * ---------------------------------------------------- */

        for (pbg3Idx = 0;
             pbg3Idx < 0x10;
             ++pbg3Idx)
        {
            if (g_Pbg3Archives[pbg3Idx] == NULL)
                continue;


            TH06FileSystemLog(
                "[FileSystem] Searching PBG3 archive index=%d",
                pbg3Idx
            );


            /* ------------------------------------------------
             * 1. Unix path
             * ------------------------------------------------ */

            entryIdx =
                g_Pbg3Archives[pbg3Idx]->FindEntry(
                    normalizedPath
                );

            if (entryIdx >= 0)
            {
                entryname =
                    normalizedPath;

                TH06FileSystemLog(
                    "[FileSystem] FOUND using normalized path: %s",
                    entryname
                );

                break;
            }


            /* ------------------------------------------------
             * 2. Windows path
             * ------------------------------------------------ */

            entryIdx =
                g_Pbg3Archives[pbg3Idx]->FindEntry(
                    windowsPath
                );

            if (entryIdx >= 0)
            {
                entryname =
                    windowsPath;

                TH06FileSystemLog(
                    "[FileSystem] FOUND using Windows path: %s",
                    entryname
                );

                break;
            }


            /* ------------------------------------------------
             * 3. Basename
             * ------------------------------------------------ */

            entryIdx =
                g_Pbg3Archives[pbg3Idx]->FindEntry(
                    basename
                );

            if (entryIdx >= 0)
            {
                entryname =
                    basename;

                TH06FileSystemLog(
                    "[FileSystem] FOUND using basename: %s",
                    entryname
                );

                break;
            }
        }
    }


    /* ========================================================
     * Decode PBG3 entry
     * ======================================================== */

    if (entryIdx >= 0 &&
        pbg3Idx >= 0 &&
        g_Pbg3Archives != NULL &&
        g_Pbg3Archives[pbg3Idx] != NULL)
    {
        utils::DebugPrint2(
            "[FileSystem] Decode: %s\n",
            entryname
        );

        TH06FileSystemLog(
            "[FileSystem] Decode: %s archive=%d entry=%d",
            entryname,
            pbg3Idx,
            entryIdx
        );


        data =
            g_Pbg3Archives[pbg3Idx]
                ->ReadDecompressEntry(
                    entryIdx,
                    entryname
                );


        if (data == NULL)
        {
            TH06FileSystemLog(
                "[FileSystem] ERROR: Decompress failed: %s",
                entryname
            );

            utils::DebugPrint2(
                "[FileSystem] ERROR: Decompress failed: %s\n",
                entryname
            );

            return NULL;
        }


        g_LastFileSize =
            g_Pbg3Archives[pbg3Idx]
                ->GetEntrySize(
                    entryIdx
                );


        TH06FileSystemLog(
            "[FileSystem] PBG3 SUCCESS: %s size=%u archive=%d entry=%d",
            entryname,
            g_LastFileSize,
            pbg3Idx,
            entryIdx
        );


        return data;
    }


    /* ========================================================
     * Not found in PBG3
     *
     * Try physical file.
     * ======================================================== */

    utils::DebugPrint2(
        "[FileSystem] Not found in PBG3: %s\n",
        filepath
    );

    TH06FileSystemLog(
        "[FileSystem] NOT FOUND in PBG3: %s",
        filepath
    );

    utils::DebugPrint2(
        "[FileSystem] Trying physical file: %s\n",
        filepath
    );

    TH06FileSystemLog(
        "[FileSystem] Trying physical file: %s",
        filepath
    );


    /* ========================================================
     * Physical file
     * ======================================================== */

    file =
        FopenUTF8(
            filepath,
            "rb"
        );

    if (file == NULL)
    {
        TH06FileSystemLog(
            "[FileSystem] Physical file OPEN FAILED: %s",
            filepath
        );

        return NULL;
    }


    if (std::fseek(file, 0, SEEK_END) != 0)
    {
        TH06FileSystemLog(
            "[FileSystem] Physical file SEEK FAILED: %s",
            filepath
        );

        std::fclose(file);

        return NULL;
    }


    long size =
        std::ftell(file);

    if (size < 0)
    {
        TH06FileSystemLog(
            "[FileSystem] Physical file FTELL FAILED: %s",
            filepath
        );

        std::fclose(file);

        return NULL;
    }


    fsize =
        (size_t)size;


    if (std::fseek(file, 0, SEEK_SET) != 0)
    {
        TH06FileSystemLog(
            "[FileSystem] Physical file SEEK RESET FAILED: %s",
            filepath
        );

        std::fclose(file);

        return NULL;
    }


    data =
        (u8 *)std::malloc(fsize);

    if (data == NULL)
    {
        TH06FileSystemLog(
            "[FileSystem] Physical malloc FAILED size=%zu",
            fsize
        );

        std::fclose(file);

        return NULL;
    }


    if (fsize > 0)
    {
        size_t readSize =
            std::fread(
                data,
                1,
                fsize,
                file
            );

        if (readSize != fsize)
        {
            TH06FileSystemLog(
                "[FileSystem] Physical fread FAILED expected=%zu actual=%zu",
                fsize,
                readSize
            );

            std::free(data);
            std::fclose(file);

            return NULL;
        }
    }


    std::fclose(file);


    g_LastFileSize =
        (u32)fsize;


    TH06FileSystemLog(
        "[FileSystem] Physical file SUCCESS: %s size=%u",
        filepath,
        g_LastFileSize
    );


    return data;
}


/* ============================================================
 * WriteDataToFile
 * ============================================================ */

int FileSystem::WriteDataToFile(
    const char *filepath,
    const u8 *data,
    u32 size
)
{
    if (filepath == NULL)
    {
        TH06FileSystemLog(
            "[FileSystem] WriteDataToFile: NULL filepath"
        );

        return false;
    }

    if (data == NULL && size != 0)
    {
        TH06FileSystemLog(
            "[FileSystem] WriteDataToFile: NULL data size=%u",
            size
        );

        return false;
    }


    TH06FileSystemLog(
        "[FileSystem] WriteDataToFile: %s size=%u",
        filepath,
        size
    );


    FILE *file =
        FopenUTF8(
            filepath,
            "wb"
        );

    if (file == NULL)
    {
        TH06FileSystemLog(
            "[FileSystem] WriteDataToFile OPEN FAILED: %s",
            filepath
        );

        return false;
    }


    if (size > 0)
    {
        size_t written =
            std::fwrite(
                data,
                1,
                size,
                file
            );

        if (written != size)
        {
            TH06FileSystemLog(
                "[FileSystem] WriteDataToFile FAILED expected=%u actual=%zu",
                size,
                written
            );

            std::fclose(file);

            return false;
        }
    }


    std::fclose(file);


    TH06FileSystemLog(
        "[FileSystem] WriteDataToFile SUCCESS: %s size=%u",
        filepath,
        size
    );


    return true;
}
