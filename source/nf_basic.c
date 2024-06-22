// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023-2024 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Basic and debug functions
// http://www.nightfoxandco.com/

#ifdef BLOCKSDS
#define iprintf printf
#endif

#include <stdio.h>

#include <nds.h>

#include "nf_basic.h"

// Folder used as root by NFLib
char NF_ROOTFOLDER[64];

__attribute__((noreturn)) void NF_Error(u32 code, const char *text, unsigned int value)
{
    consoleDemoInit();      // Initialize demo text console
    setBrightness(3, 0);    // Set the brightness to the top

    switch (code)
    {
        case 101: // File not found
            iprintf("File %s not found.\n", text);
            break;

        case 102: // Not enough memory
            iprintf("Out of memory.\n");
            iprintf("%u bytes\n", value);
            iprintf("can't be allocated.\n");
            break;

        case 103: // Not enough free slots
            iprintf("Out of %s slots.\n", text);
            iprintf("All %u slots are in use.\n", value);
            break;

        case 104: // Tiled background not found
            iprintf("Tiled Bg %s\n", text);
            iprintf("not found.\n");
            break;

        case 105: // Background hasn't been created
            iprintf("Background number %u\n", value);
            iprintf("on screen %s is\n", text);
            iprintf("not created.\n");
            break;

        case 106: // Value out of range
            iprintf("%s value out\n", text);
            iprintf("of range (%u max).\n", value);
            break;

        case 107: // Not enough contiguous VRAM blocks (tiles)
            iprintf("Can't allocate %d\n", (int)value);
            iprintf("blocks of tiles for\n");
            iprintf("%s background\n", text);
            iprintf("Free %d KB of VRAM or try to\n", (value * 16384) / 1024);
            iprintf("reload all BGs again\n");
            break;

        case 108: // Not enough contiguous VRAM blocks (maps)
            iprintf("Can't allocate %d\n", (int)value);
            iprintf("blocks of maps for\n");
            iprintf("%s background\n", text);
            iprintf("Free %d KB of VRAM or try to\n", (value * 2048) / 1024);
            iprintf("reload all BGs again\n");
            break;

        case 109: // ID in use
            iprintf("%s ID %u\n", text, value);
            iprintf("is already in use.\n");
            break;

        case 110: // ID not loaded
            iprintf("%s\n", text);
            iprintf("%u not loaded.\n", value);
            break;

        case 111: // ID not in VRAM
            iprintf("%s\n", text);
            iprintf("%u not in VRAM.\n", value);
            break;

        case 112: // Sprite not created
            iprintf("Sprite number %u\n", value);
            iprintf("on screen %s is\n", text);
            iprintf("not created.\n");
            break;

        case 113: // Not enough VRAM
            iprintf("Out of VRAM.\n");
            iprintf("%u bytes for %s\n", value, text);
            iprintf("can't be allocated.\n");
            break;

        case 114: // Text layer doesn't exist
            iprintf("Text layer on screen\n");
            iprintf("%u layer doesn't exist.\n", value);
            break;

        case 115: // Invalid tiled background size (not a multiple of 256)
            iprintf("Tiled BG %s\n", text);
            iprintf("has wrong size.\n");
            iprintf("Your bg sizes must be\n");
            iprintf("a multiple of 256 pixels.\n");
            break;

        case 116: // File is too big
            iprintf("File %s\n", text);
            iprintf("is too big.\n");
            iprintf("Max size for\n");
            iprintf("file is %u KB.\n", value / 1024);
            break;

        case 117: // Invalid affine background size (only 256x256 and 512x512 allowed)
            iprintf("Affine BG %s\n", text);
            iprintf("has wrong size.\n");
            iprintf("Your bg sizes must be\n");
            iprintf("256x256 or 512x512 and\n");
            iprintf("with 256 tiles or less.\n");
            break;

        case 118: // Invalid affine background layer
            iprintf("Affine Bg %s\n", text);
            iprintf("only can be created\n");
            iprintf("on layers 2 or 3.\n");
            break;

        case 119: // Invalid texture size
            iprintf("Texture ID %u illegal size.\n", value);
            iprintf("Only power of 2 sizes can\n");
            iprintf("be used (8 to 1024).\n");
            break;

        case 120: // Invalid sprite size
            iprintf("Sprite ID %u illegal size.\n", value);
            iprintf("8x8 Sprites can't be used\n");
            iprintf("in 1D_128 mode.\n");
            break;

        case 121: // Failed to fseek()
            iprintf("fseek() failed:\n");
            iprintf("%s\n", text);
            break;

        case 122: // Failed to fread()
            iprintf("fread() failed:\n");
            iprintf("%s\n", text);
            break;

        case 123: // Failed to fclose()
            iprintf("fclose() failed:\n");
            iprintf("%s\n", text);
            break;
    }

    // Print error code
    iprintf("Error code %d.\n", (int)code);

    // Stop program
    while (1)
        swiWaitForVBlank();
}

void NF_SetRootFolder(const char *folder)
{
    if (strcmp(folder, "NITROFS") == 0)
    {
        // Initialize both NitroFS and FAT

        // Define NitroFS as the root folder
        snprintf(NF_ROOTFOLDER, sizeof(NF_ROOTFOLDER), "%s", "");

        // Check if NitroFS exists. NitroFS must be mounted beforehand for this
        // to work.
        if (access("nitro:/", F_OK) == 0)
        {
            // NitroFS started ok. Set it as the NFLib root folder
            chdir("nitro:/");
        }
        else
        {
            // NitroFS start error

            // Initialize text console
            consoleDemoInit();

            if (NF_GetLanguage() == 5)
            {
                iprintf("Error iniciando NitroFS.\n");
                iprintf("Programa detenido.\n\n");
                iprintf("Verifica que tu flashcard\n");
                iprintf("es compatible con Argv.\n");
                iprintf("Si no lo es, intenta usar el\n");
                iprintf("Homebrew Menu para ejecutarla.\n\n");
            }
            else
            {
                iprintf("NitroFS Init Error.\n");
                iprintf("Abnormal termination.\n\n");
                iprintf("Check if your flashcard is\n");
                iprintf("Argv compatible.\n");
                iprintf("If not, try to launch the ROM\n");
                iprintf("using the Homebrew Menu.\n\n");
            }
            iprintf("http://sourceforge.net/projects/devkitpro/files/hbmenu/");

            // Stop program
            while (1)
                swiWaitForVBlank();
        }
    }
    else
    {
        // Initialize FAT only

        // Define the root folder of NFLib
        snprintf(NF_ROOTFOLDER, sizeof(NF_ROOTFOLDER), "%s", folder);

        // Check where the NDS is running from
        bool init_ok = false;

        // First, try to detect the drive where the NDS ROM is. If argv has been
        // provided by the loader, it will contain the drive and the path of the
        // ROM.
        if (__system_argv->argvMagic == ARGV_MAGIC && __system_argv->argc >= 1)
        {
            if (strncmp(__system_argv->argv[0], "fat:", 4) == 0)
            {
                // If argv starts by "fat:", try to setup "fat:" as root
                if (access("fat:/", F_OK) == 0)
                {
                    chdir("fat:/");
                    init_ok = true;
                }
            }
            else if (strncmp(__system_argv->argv[0], "sd:", 3) == 0)
            {
                // If argv starts by "sd:", try to setup "sd:" as root
                if (access("sd:/", F_OK) == 0)
                {
                    chdir("sd:/");
                    init_ok = true;
                }
            }
        }

        // Second, try to bruteforce the detection. Check if there is access to
        // the SD card of the DSi first. If not, try with DLDI.
        if (!init_ok)
        {
            if (access("sd:/", F_OK) == 0)
            {
                chdir("sd:/");
                init_ok = true;
            }
            else if (access("fat:/", F_OK) == 0)
            {
                chdir("fat:/");
                init_ok = true;
            }
        }

        // If that didn't work, give up.
        if (!init_ok)
        {
            // NitroFS start error

            // Initialize text console
            consoleDemoInit();

            if (NF_GetLanguage() == 5)
            {
                iprintf("Error iniciando FAT.\n");
                iprintf("Programa detenido.\n\n");
                iprintf("Verifica que tu flashcard es\n");
                iprintf("compatible con DLDI y la ROM\n");
                iprintf("este parcheada correctamente.\n");
            }
            else
            {
                iprintf("FAT Init Error.\n");
                iprintf("Abnormal termination.\n\n");
                iprintf("Check if your flashcard is\n");
                iprintf("DLDI compatible and the ROM\n");
                iprintf("is correctly patched.\n");
            }

            // Stop program
            while (1)
                swiWaitForVBlank();
        }
    }
}

void NF_DmaMemCopy(void *destination, const void *source, u32 size)
{
    // Based on Coranac's function:
    // http://www.coranac.com/2009/05/dma-vs-arm9-fight/

    // Source and destination addresses
    u32 src = (u32)source;
    u32 dst = (u32)destination;

    // Check if addresses are aligned to at least 16 bits
    if ((src | dst) & 1)
    {
        // Not aligned. DMA can't be used, use memcpy()
        memcpy(destination, source, size);
    }
    else
    {
        // Addresses are aligned, use DMA

        // Wait until channel 3 is available
        while (dmaBusy(3));

        // Make sure that the data in the cache is sent to the main RAM
        DC_FlushRange(source, size);

        // Depending on the alignment use 32-bit or 16-bit copy modes
        if ((src | dst | size) & 3)
            dmaCopyHalfWords(3, source, destination, size);
        else
            dmaCopyWords(3, source, destination, size);

        // Prevent the destination from being in cache, it would corrupt the
        // data when it is flushed
        DC_InvalidateRange(destination, size);
    }
}

void NF_FileLoad(const char *path, char **buffer, size_t *size, size_t min_size)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        NF_Error(101, path, 0);

    // Get file size
    if (fseek(f, 0, SEEK_END) != 0)
        NF_Error(121, path, 0);

    size_t size_ = ftell(f);
    rewind(f);

    // If the file size is smaller than the minimum size, pad it with zeroes
    // with calloc().
    char *buffer_;
    if (size_ < min_size)
    {
        buffer_ = calloc(min_size, sizeof(char));
        *size = min_size;
    }
    else
    {
        buffer_ = malloc(size_);
        *size = size_;
    }

    if (buffer_ == NULL)
        NF_Error(102, NULL, size_);

    *buffer = buffer_;

    // Read full file to the start of the buffer
    if (fread(buffer_, 1, size_, f) != size_)
        NF_Error(122, path, 0);

    if (fclose(f) != 0)
        NF_Error(123, path, 0);
}

bool NF_FileExists(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
        return false;

    if (fclose(f) != 0)
        NF_Error(123, path, 0);

    return true;
}
