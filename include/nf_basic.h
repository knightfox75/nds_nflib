// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Basic and debug functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_BASIC_H__
#define NF_BASIC_H__

/// @file   nf_basic.h
/// @brief  General functions of NFLib

/// @defgroup nf_basic General functions of NFLib
///
/// Functions related to error handling and other general settings.
///
/// @{

#include <nds.h>

/// Root folder used by NFLib
extern char NF_ROOTFOLDER[64];

/// Stops program execution and shows the error on the screen.
///
/// This command is used internally by the lib to generate debug messages and
/// it will rarely be used in you code.
///
/// 101: File not found.
/// 102: Not enough memory.
/// 103: Not enough free slots.
/// 104: Background not found.
/// 105: Background not created.
/// 106: Value outside of bounds.
/// 107: Not enough contiguous blocks in VRAM (tiles).
/// 108: Not enough contiguous blocks in VRAM (maps).
/// 109: ID already used.
/// 110: ID not loaded in RAM.
/// 111: ID not loaded in VRAM.
/// 112: Sprite not created.
/// 113: Not enough VRAM.
/// 114: Text layer doesn't exist.
/// 115: Background dimensions are invalid (not multiple of 256).
/// 116: File too big.
/// 117: Affine background dimensions are invalid.
/// 118: Affine creation layer is invalid.
///
/// @param code Error code.
/// @param text Description.
/// @param value Additional info.
__attribute__((noreturn)) void NF_Error(u32 code, const char *text, unsigned int value);

/// Defines the root folder of your project (FAT or NitroFS).
///
/// This makes it easy to change the name of folder that contains all files of
/// your project after it’s compiled. It’s imperative the use of this function
/// before loading  any file from FAT.
///
/// If you want to use NitroFS, use "NITROFS" as root folder name. You must
/// configure your Makefile correctly to include the NitroFS files in your ROM.
/// When using a flashcard, it must support argv. Use Homebrew Menu to launch
/// the ROM if your flashcard doesn't support it.
///
/// Example:
/// ```
/// // Define "mygame" folder as root for your project using FAT
/// NF_SetRootFolder("mygame");
/// ```
///
/// @param folder
void NF_SetRootFolder(const char *folder);

/// Function copy blocks of memory from RAM to VRAM fast.
///
/// DMA copies from RAM to VRAM are the most efficient. The function checks if
/// the data is aligned for the DMA copy. If not, it uses memcpy() insead.
///
/// Example:
/// ```
/// // Copy to address 0x06000000 (VRAM_A) 128 KB of memory from "buffer"
/// NF_DmaMemCopy((void*)0x06000000, buffer, 131072);
/// ```
///
/// @param destination Destination pointer.
/// @param source Source pointer.
/// @param size Number of bytes to copy.
void NF_DmaMemCopy(void *destination, const void *source, u32 size);

/// Returns the language ID set by the user in the firmware.
///
/// 0 : Japanese
/// 1 : English
/// 2 : French
/// 3 : German
/// 4 : Italian
/// 5 : Spanish
/// 6 : Chinese
///
/// @return The language ID.
static inline u32 NF_GetLanguage(void)
{
    return PersonalData->language;
}

// Internal use. Used to load files and optionally pad the resulting buffer.
void NF_FileLoad(const char *path, char **buffer, size_t *size, size_t min_size);

// Internal use. Returns true if the file exists and can be open for reading.
bool NF_FileExists(const char *path);

/// @}

#endif // NF_BASIC_H__

#ifdef __cplusplus
}
#endif
