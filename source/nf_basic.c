// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023-2024 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Basic and debug functions
// http://www.nightfoxandco.com/

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
            printf("File %s not found.\n", text);
            break;

        case 102: // Not enough memory
            printf("Out of memory.\n");
            printf("%u bytes\n", value);
            printf("can't be allocated.\n");
            break;

        case 103: // Not enough free slots
            printf("Out of %s slots.\n", text);
            printf("All %u slots are in use.\n", value);
            break;

        case 104: // Tiled background not found
            printf("Tiled Bg %s\n", text);
            printf("not found.\n");
            break;

        case 105: // Background hasn't been created
            printf("Background number %u\n", value);
            printf("on screen %s is\n", text);
            printf("not created.\n");
            break;

        case 106: // Value out of range
            printf("%s value out\n", text);
            printf("of range (%u max).\n", value);
            break;

        case 107: // Not enough contiguous VRAM blocks (tiles)
            printf("Can't allocate %d\n", (int)value);
            printf("blocks of tiles for\n");
            printf("%s background\n", text);
            printf("Free %d KB of VRAM or try to\n", (value * 16384) / 1024);
            printf("reload all BGs again\n");
            break;

        case 108: // Not enough contiguous VRAM blocks (maps)
            printf("Can't allocate %d\n", (int)value);
            printf("blocks of maps for\n");
            printf("%s background\n", text);
            printf("Free %d KB of VRAM or try to\n", (value * 2048) / 1024);
            printf("reload all BGs again\n");
            break;

        case 109: // ID in use
            printf("%s ID %u\n", text, value);
            printf("is already in use.\n");
            break;

        case 110: // ID not loaded
            printf("%s\n", text);
            printf("%u not loaded.\n", value);
            break;

        case 111: // ID not in VRAM
            printf("%s\n", text);
            printf("%u not in VRAM.\n", value);
            break;

        case 112: // Sprite not created
            printf("Sprite number %u\n", value);
            printf("on screen %s is\n", text);
            printf("not created.\n");
            break;

        case 113: // Not enough VRAM
            printf("Out of VRAM.\n");
            printf("%u bytes for %s\n", value, text);
            printf("can't be allocated.\n");
            break;

        case 114: // Text layer doesn't exist
            printf("Text layer on screen\n");
            printf("%u layer doesn't exist.\n", value);
            break;

        case 115: // Invalid tiled background size (not a multiple of 256)
            printf("Tiled BG %s\n", text);
            printf("has wrong size.\n");
            printf("Your bg sizes must be\n");
            printf("a multiple of 256 pixels.\n");
            break;

        case 116: // File is too big
            printf("File %s\n", text);
            printf("is too big.\n");
            printf("Max size for\n");
            printf("file is %u KB.\n", value / 1024);
            break;

        case 117: // Invalid affine background size (only 256x256 and 512x512 allowed)
            printf("Affine BG %s\n", text);
            printf("has wrong size.\n");
            printf("Your bg sizes must be\n");
            printf("256x256 or 512x512 and\n");
            printf("with 256 tiles or less.\n");
            break;

        case 118: // Invalid affine background layer
            printf("Affine Bg %s\n", text);
            printf("only can be created\n");
            printf("on layers 2 or 3.\n");
            break;

        case 119: // Invalid texture size
            printf("Texture ID %u illegal size.\n", value);
            printf("Only power of 2 sizes can\n");
            printf("be used (8 to 1024).\n");
            break;

        case 120: // Invalid sprite size
            printf("Sprite ID %u illegal size.\n", value);
            printf("8x8 Sprites can't be used\n");
            printf("in 1D_128 mode.\n");
            break;

        case 121: // Failed to fseek()
            printf("fseek() failed:\n");
            printf("%s\n", text);
            break;

        case 122: // Failed to fread()
            printf("fread() failed:\n");
            printf("%s\n", text);
            break;

        case 123: // Failed to fclose()
            printf("fclose() failed:\n");
            printf("%s\n", text);
            break;
    }

    // Print error code
    printf("Error code %d.\n", (int)code);

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
                printf("Error iniciando NitroFS.\n");
                printf("Programa detenido.\n\n");
                printf("Verifica que tu flashcard\n");
                printf("es compatible con Argv.\n");
                printf("Si no lo es, intenta usar el\n");
                printf("Homebrew Menu para ejecutarla.\n\n");
            }
            else
            {
                printf("NitroFS Init Error.\n");
                printf("Abnormal termination.\n\n");
                printf("Check if your flashcard is\n");
                printf("Argv compatible.\n");
                printf("If not, try to launch the ROM\n");
                printf("using the Homebrew Menu.\n\n");
            }
            printf("https://github.com/devkitPro/nds-hb-menu/releases");

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
                printf("Error iniciando FAT.\n");
                printf("Programa detenido.\n\n");
                printf("Verifica que tu flashcard es\n");
                printf("compatible con DLDI y la ROM\n");
                printf("este parcheada correctamente.\n");
            }
            else
            {
                printf("FAT Init Error.\n");
                printf("Abnormal termination.\n\n");
                printf("Check if your flashcard is\n");
                printf("DLDI compatible and the ROM\n");
                printf("is correctly patched.\n");
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
