#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>

#include "pbg3/Pbg3Archive.hpp"

Pbg3Archive **g_Pbg3Archives;


// ============================================================
// TH06 Runtime Log
// ============================================================

static void TH06WriteLog(const char *format, ...)
{
    char buffer[4096];

    va_list args;
    va_start(args, format);

    std::vsnprintf(
        buffer,
        sizeof(buffer),
        format,
        args
    );

    va_end(args);

    // --------------------------------------------------------
    // Console
    // --------------------------------------------------------

    std::printf("%s", buffer);
    std::fflush(stdout);


    // --------------------------------------------------------
    // iOS Documents/log.txt
    // --------------------------------------------------------

    const char *home = std::getenv("HOME");

    if (home == NULL)
    {
        return;
    }

    char logPath[2048];

    std::snprintf(
        logPath,
        sizeof(logPath),
        "%s/Documents/log.txt",
        home
    );

    FILE *logFile =
        std::fopen(logPath, "ab");

    if (logFile == NULL)
    {
        return;
    }

    std::fwrite(
        buffer,
        1,
        std::strlen(buffer),
        logFile
    );

    std::fflush(logFile);
    std::fclose(logFile);
}


// ============================================================
// Constructor
// ============================================================

Pbg3Archive::Pbg3Archive()
{
    this->fileTableOffset = 0;
    this->numOfEntries = 0;
    this->entries = NULL;
    this->parser = NULL;
    this->unk = NULL;

    TH06WriteLog(
        "\n========================================\n"
        "TH06 PBG3 LOGGER STARTED\n"
        "========================================\n"
    );
}


// ============================================================
// ParseHeader
// ============================================================

i32 Pbg3Archive::ParseHeader()
{
    TH06WriteLog(
        "\n[PBG3] ParseHeader START\n"
    );

    if (this->parser->ReadMagic() != 0x33474250)
    {
        TH06WriteLog(
            "[PBG3] ERROR: invalid magic\n"
        );

        if (this->parser != NULL)
        {
            delete this->parser;
            this->parser = NULL;
        }

        return false;
    }

    this->numOfEntries =
        this->parser->ReadVarInt();

    this->fileTableOffset =
        this->parser->ReadVarInt();

    TH06WriteLog(
        "[PBG3] ParseHeader entries=%u tableOffset=%u\n",
        this->numOfEntries,
        this->fileTableOffset
    );

    if (!this->parser->SeekToOffset(
            this->fileTableOffset))
    {
        TH06WriteLog(
            "[PBG3] ERROR: failed to seek file table\n"
        );

        if (this->parser != NULL)
        {
            delete this->parser;
            this->parser = NULL;
        }

        return false;
    }

    this->entries =
        new Pbg3Entry[this->numOfEntries];

    if (this->entries == NULL)
    {
        TH06WriteLog(
            "[PBG3] ERROR: failed to allocate entries\n"
        );

        if (this->parser != NULL)
        {
            delete this->parser;
            this->parser = NULL;
        }

        return false;
    }

    for (u32 idx = 0;
         idx < this->numOfEntries;
         idx += 1)
    {
        this->entries[idx].unk2 =
            this->parser->ReadVarInt();

        this->entries[idx].unk1 =
            this->parser->ReadVarInt();

        this->entries[idx].checksum =
            this->parser->ReadVarInt();

        this->entries[idx].dataOffset =
            this->parser->ReadVarInt();

        this->entries[idx].uncompressedSize =
            this->parser->ReadVarInt();

        if (!this->parser->ReadString(
                this->entries[idx].filename,
                sizeof(this->entries[idx].filename)))
        {
            TH06WriteLog(
                "[PBG3] ERROR: failed to read filename at entry %u\n",
                idx
            );

            if (this->parser != NULL)
            {
                delete this->parser;
                this->parser = NULL;
            }

            if (this->entries != NULL)
            {
                delete[] this->entries;
                this->entries = NULL;
            }

            return false;
        }

        // ----------------------------------------------------
        // Important TH06 resources
        // ----------------------------------------------------

        if (
            std::strcmp(
                this->entries[idx].filename,
                "text.anm"
            ) == 0 ||

            std::strcmp(
                this->entries[idx].filename,
                "plst00.wav"
            ) == 0 ||

            std::strcmp(
                this->entries[idx].filename,
                "pldead00.wav"
            ) == 0
        )
        {
            TH06WriteLog(
                "\n[PBG3] !!! IMPORTANT ENTRY !!!\n"
                "  index    = %u\n"
                "  name     = [%s]\n"
                "  offset   = %u\n"
                "  size     = %u\n"
                "  checksum = %u\n",
                idx,
                this->entries[idx].filename,
                this->entries[idx].dataOffset,
                this->entries[idx].uncompressedSize,
                this->entries[idx].checksum
            );
        }
    }

    TH06WriteLog(
        "[PBG3] ParseHeader COMPLETE entries=%u\n",
        this->numOfEntries
    );

    return true;
}


// ============================================================
// Release
// ============================================================

i32 Pbg3Archive::Release()
{
    this->fileTableOffset = 0;
    this->numOfEntries = 0;

    if (this->parser != NULL)
    {
        delete this->parser;
        this->parser = NULL;
    }

    if (this->entries != NULL)
    {
        delete[] this->entries;
        this->entries = NULL;
    }

    std::free(this->unk);
    this->unk = NULL;

    return true;
}


// ============================================================
// FindEntry
// ============================================================

i32 Pbg3Archive::FindEntry(const char *path)
{
    if (path == NULL)
    {
        TH06WriteLog(
            "[PBG3] FindEntry: NULL path\n"
        );

        return -1;
    }

    TH06WriteLog(
        "[PBG3] FindEntry searching [%s], entries=%u\n",
        path,
        this->numOfEntries
    );

    if (this->entries == NULL)
    {
        TH06WriteLog(
            "[PBG3] FindEntry ERROR: entries == NULL\n"
        );

        return -1;
    }

    for (u32 entryIdx = 0;
         entryIdx < this->numOfEntries;
         entryIdx += 1)
    {
        char *entryFilename =
            this->entries[entryIdx].filename;

        if (entryFilename == NULL)
        {
            continue;
        }

        if (std::strcmp(
                path,
                entryFilename
            ) == 0)
        {
            TH06WriteLog(
                "[PBG3] FindEntry FOUND [%s] "
                "index=%u size=%u checksum=%u\n",
                entryFilename,
                entryIdx,
                this->entries[entryIdx].uncompressedSize,
                this->entries[entryIdx].checksum
            );

            return (i32)entryIdx;
        }
    }

    if (
        std::strcmp(path, "text.anm") == 0 ||
        std::strcmp(path, "plst00.wav") == 0 ||
        std::strcmp(path, "pldead00.wav") == 0
    )
    {
        TH06WriteLog(
            "[PBG3] FindEntry NOT FOUND [%s]\n",
            path
        );
    }

    return -1;
}


// ============================================================
// GetEntrySize
// ============================================================

u32 Pbg3Archive::GetEntrySize(u32 entryIdx)
{
    if (entryIdx >= this->numOfEntries)
    {
        return 0;
    }

    return this->entries[entryIdx].uncompressedSize;
}


// ============================================================
// ReadEntryRaw
// ============================================================

u8 *Pbg3Archive::ReadEntryRaw(
    u32 *outSize,
    u32 *outChecksum,
    i32 entryIdx)
{
    if (this->parser == NULL)
    {
        TH06WriteLog(
            "[PBG3] ReadEntryRaw ERROR: parser == NULL\n"
        );

        return NULL;
    }

    if (entryIdx < 0 ||
        entryIdx >= (i32)this->numOfEntries)
    {
        TH06WriteLog(
            "[PBG3] ReadEntryRaw ERROR: invalid entry=%d\n",
            entryIdx
        );

        return NULL;
    }

    if (outSize == NULL ||
        outChecksum == NULL)
    {
        return NULL;
    }

    if (!this->parser->SeekToOffset(
            this->entries[entryIdx].dataOffset))
    {
        TH06WriteLog(
            "[PBG3] ReadEntryRaw failed to seek entry=%d\n",
            entryIdx
        );

        return NULL;
    }

    u32 size;

    if (entryIdx ==
        (i32)this->numOfEntries - 1)
    {
        size =
            this->fileTableOffset -
            this->entries[entryIdx].dataOffset;
    }
    else
    {
        size =
            this->entries[entryIdx + 1].dataOffset -
            this->entries[entryIdx].dataOffset;
    }

    u8 *data =
        (u8 *)std::malloc(size);

    if (data == NULL)
    {
        TH06WriteLog(
            "[PBG3] ReadEntryRaw malloc failed size=%u\n",
            size
        );

        return NULL;
    }

    if (!this->parser->ReadByteAlignedData(
            data,
            size))
    {
        TH06WriteLog(
            "[PBG3] ReadEntryRaw failed reading entry=%d\n",
            entryIdx
        );

        std::free(data);

        return NULL;
    }

    *outChecksum =
        this->entries[entryIdx].checksum;

    *outSize = size;

    return data;
}


// ============================================================
// Destructor
// ============================================================

Pbg3Archive::~Pbg3Archive()
{
    this->Release();
}


// ============================================================
// Load
// ============================================================

i32 Pbg3Archive::Load(const char *path)
{
    TH06WriteLog(
        "\n[PBG3] ========================================\n"
        "[PBG3] Loading archive [%s]\n"
        "[PBG3] ========================================\n",
        path != NULL ? path : "(null)"
    );

    if (!this->Release())
    {
        return false;
    }

    this->parser =
        new Pbg3Parser();

    if (this->parser == NULL)
    {
        TH06WriteLog(
            "[PBG3] ERROR: failed to create parser\n"
        );

        return false;
    }

    if (!this->parser->OpenArchive(path))
    {
        TH06WriteLog(
            "[PBG3] ERROR: failed to open archive [%s]\n",
            path != NULL ? path : "(null)"
        );

        if (this->parser != NULL)
        {
            delete this->parser;
            this->parser = NULL;
        }

        return false;
    }

    if (!this->ParseHeader())
    {
        TH06WriteLog(
            "[PBG3] ERROR: ParseHeader failed [%s]\n",
            path != NULL ? path : "(null)"
        );

        return false;
    }

    TH06WriteLog(
        "[PBG3] archive loaded successfully [%s]\n",
        path != NULL ? path : "(null)"
    );

    return true;
}


// ============================================================
// LZSS
// ============================================================

#define LZSS_DICTSIZE 0x2000
#define LZSS_DICTSIZE_MASK 0x1fff
#define LZSS_MIN_MATCH 3

#define DEC_NEXT_BIT() \
    inBitMask >>= 1; \
    if (inBitMask == 0) \
    { \
        inBitMask = 0x80; \
    }

#define DEC_WRITE_BYTE(data) \
    *outCursor++ = data; \
    dict[dictHead] = data; \
    dictHead = (dictHead + 1) & LZSS_DICTSIZE_MASK;

#define DEC_HANDLE_FETCH_NEW_BYTE() \
    if (inBitMask == 0x80) \
    { \
        currByte = *inCursor; \
        if (inCursor - rawData >= (i32)size) \
        { \
            currByte = 0; \
        } \
        else \
        { \
            inCursor++; \
        } \
        checksum += currByte; \
    }

#define DEC_READ_FLAG_BIT() \
    DEC_HANDLE_FETCH_NEW_BYTE(); \
    opcode = currByte & inBitMask; \
    DEC_NEXT_BIT();

#define DEC_READ_BITS(bitsCount) \
    outBitMask = 0x01 << (bitsCount - 1); \
    inBits = 0; \
    while (outBitMask != 0) \
    { \
        DEC_HANDLE_FETCH_NEW_BYTE(); \
        if ((currByte & inBitMask) != 0) \
        { \
            inBits |= outBitMask; \
        } \
        outBitMask >>= 1; \
        DEC_NEXT_BIT(); \
    }


// ============================================================
// ReadDecompressEntry
// ============================================================

u8 *Pbg3Archive::ReadDecompressEntry(
    u32 entryIdx,
    const char *filename)
{
    if (
        entryIdx >= this->numOfEntries ||
        this->parser == NULL
    )
    {
        TH06WriteLog(
            "[PBG3] Decompress INVALID "
            "entry=%u filename=[%s]\n",
            entryIdx,
            filename != NULL ? filename : "(null)"
        );

        return NULL;
    }

    TH06WriteLog(
        "\n[PBG3] Decompress START "
        "entry=%u filename=[%s]\n",
        entryIdx,
        filename != NULL ? filename : "(null)"
    );

    u32 size =
        this->GetEntrySize(entryIdx);

    TH06WriteLog(
        "[PBG3] Expected uncompressed size=%u\n",
        size
    );

    u8 *out =
        (u8 *)std::malloc(size);

    if (out == NULL)
    {
        TH06WriteLog(
            "[PBG3] Decompress output malloc failed "
            "size=%u\n",
            size
        );

        return NULL;
    }

    u8 *outCursor = out;

    u32 expectedCsum;

    u8 *rawData =
        this->ReadEntryRaw(
            &size,
            &expectedCsum,
            entryIdx
        );

    if (rawData == NULL)
    {
        TH06WriteLog(
            "[PBG3] Decompress ReadEntryRaw FAILED "
            "entry=%u filename=[%s]\n",
            entryIdx,
            filename != NULL ? filename : "(null)"
        );

        std::free(out);

        return NULL;
    }

    u8 *inCursor = rawData;
    u8 inBitMask = 0x80;

    u32 checksum = 0;
    u32 dictHead = 1;

    u8 dict[LZSS_DICTSIZE];

    for (i32 i = 0;
         i < LZSS_DICTSIZE;
         i++)
    {
        dict[i] = 0;
    }

    u32 currByte;
    u32 inBits;
    u32 outBitMask;
    u32 matchOffset;
    u32 opcode;

    for (;;)
    {
        DEC_READ_FLAG_BIT();

        if (opcode != 0)
        {
            DEC_READ_BITS(8);
            DEC_WRITE_BYTE(inBits);
        }
        else
        {
            DEC_READ_BITS(13);

            matchOffset = inBits;

            if (matchOffset == 0)
            {
                break;
            }

            DEC_READ_BITS(4);

            for (i32 i = 0;
                 i <= (i32)inBits + 2;
                 i++)
            {
                u32 c =
                    dict[
                        (matchOffset + i) &
                        LZSS_DICTSIZE_MASK
                    ];

                DEC_WRITE_BYTE(c);
            }
        }
    }

    while (inBitMask != 0x80)
    {
        DEC_READ_FLAG_BIT();
    }

    std::free(rawData);

    if (
        this->entries[entryIdx].checksum
        != checksum
    )
    {
        TH06WriteLog(
            "[PBG3] Decompress CHECKSUM FAILED "
            "filename=[%s] expected=%u actual=%u\n",
            filename != NULL ? filename : "(null)",
            this->entries[entryIdx].checksum,
            checksum
        );

        std::free(out);

        return NULL;
    }

    TH06WriteLog(
        "[PBG3] Decompress SUCCESS "
        "filename=[%s] size=%u checksum=%u\n",
        filename != NULL ? filename : "(null)",
        this->entries[entryIdx].uncompressedSize,
        checksum
    );

    return out;
}
