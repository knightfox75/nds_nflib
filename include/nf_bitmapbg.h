// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Bitmap background functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_BITMAPBG_H__
#define NF_BITMAPBG_H__

#include <nds.h>

/// @file   nf_bitmapbg.h
/// @brief  Bitmap background support.

/// @defgroup nf_bitmapbg Bitmap background support.
///
/// Functions to load and manage bitmap backgrounds.
///
/// @{

/// Maximum number of slots of 16-bit bitmap backgrounds
#define NF_SLOTS_BG16B 16

/// Maximum number of slots of 8-bit bitmap backgrounds
#define NF_SLOTS_BG8B 16

/// Struct that holds information about 16-bit bitmap backgrounds.
typedef struct {
    u16 *buffer;    ///< Data buffer
    size_t size;    ///< Size of the buffer
    u16 width;      ///< Width of the image (max 256 pixels)
    u16 height;     ///< Height of the image (max 256 pixels)
    bool inuse;     ///< True if the slot is in use
} NF_TYPE_BG16B_INFO;

/// Information of all 16-bit bitmap backgrounds
extern NF_TYPE_BG16B_INFO NF_BG16B[NF_SLOTS_BG16B];

// Backbuffer of 16 bit of each screen
extern u16* NF_16BITS_BACKBUFFER[2];

/// Struct that holds information about 8-bit bitmap backgrounds.
typedef struct {
    u8 *data;           ///< Data buffer
    size_t data_size;   ///< Data buffer size
    u16 *pal;           ///< Palette buffer
    size_t pal_size;    ///< Palette buffer size
    bool inuse;         ///< True if the slot is in use
} NF_TYPE_BG8B_INFO;

/// Information of all 8-bit bitmap backgrounds
extern NF_TYPE_BG8B_INFO NF_BG8B[NF_SLOTS_BG8B];

/// Information of a backbuffer of 8 bit
typedef struct {
    u8 *data;       ///< Pointer to the bitmap
    u16 *pal;       ///< Pointer to the bitmap palette
} NF_TYPE_BB8B_INFO;

/// Backbuffer of 8 bit of each screen
extern NF_TYPE_BB8B_INFO NF_8BITS_BACKBUFFER[2];

/// Initialize buffers to store 16-bit bitmap backgrounds.
///
/// You must call this function once in you code before loading any 16-bit
/// bitmap.
void NF_Init16bitsBgBuffers(void);

/// Resets all 16 bit background buffers and clears them.
///
/// Useful when changing game levels, for example.
void NF_Reset16bitsBgBuffers(void);

/// Initialize the 16 bit background backbuffer of the selected screen.
///
/// Use this function once before using the backbuffer.
///
/// @param screen Screen (0 - 1).
void NF_Init16bitsBackBuffer(int screen);

/// Enables the 16-bit backbuffer of the selected screen.
///
/// If the backbuffer is already enabled, the contents are cleared.
///
/// @param screen Screen (0 - 1).
void NF_Enable16bitsBackBuffer(int screen);

/// Disables the 16-bit backbuffer of selected screen.
///
/// It frees the RAM used by it (128 KB).
///
/// @param screen Screen (0 - 1).
void NF_Disble16bitsBackBuffer(int screen);

/// Sends the 16-bit backbuffer to the VRAM of the selected screen.
///
/// Use it to edit the image in the backbuffer and to update the image displayed
/// on the screen without showing the drawing process (which may be very slow).
///
/// @param screen Screen (0 - 1).
void NF_Flip16bitsBackBuffer(int screen);

/// Initializes the selected screen in "bitmap" mode.
///
/// The color depth of the bitmap can be 8 or 16 bits.
///
/// The DS 2D engine must be set to video mode 5.
///
/// Example:
/// ```
/// // Setup the top screen to use 16-bit bitmaps
/// NF_InitBitmapBgSys(0, 1);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param mode Depth mode (0: 8 bits / 256 colors; 1: 16 bits)
void NF_InitBitmapBgSys(int screen, u32 mode);

/// Loads a 16-bit bitmap from the filesystem.
///
/// The file must be in binary format ".img", and its max size is 256x256
/// pixels (128 KB).
///
/// You can convert the file using this GRIT command line:
///
/// ```
/// grit file.png -ftb -fh! -gb -gB16
/// ```
///
/// You can load as many files as defined in NF_SLOTS_BG16B.
///
/// Example:
/// ```
/// // Load "bitmap16.img" to slot 0
/// NF_Load16bitsBg("bmp/bitmap16", 0);
/// ```
///
/// @param file File name without extension.
/// @param slot Slot number (0 - 15).
void NF_Load16bitsBg(const char *file, u32 slot);

/// Loads a 16 bits image into a RAM slot.
///
/// The image must be in ".img" format, and it has a max size of 256x256 pixels
/// You must also specify the size of the image. The image will be loaded into
/// the specified 16-bits bitmap background slot.
///
/// Use NF_Unload16bitsBg() to remove it from RAM.
///
/// Example:
/// ```
/// // Loads file "character" file to slot 1. The size is 64x128 pixels.
/// NF_Load16bitsImage("bmp/character", 1, 64, 128);
/// ```
/// Any magenta pixel (0xFF00FF) will be transparent.
///
/// @param file File name without extension.
/// @param slot Slot number (0 - 15).
/// @param size_x Width in pixels.
/// @param size_y Height in pixels.
void NF_Load16bitsImage(const char *file, u32 slot, u32 size_x, u32 size_y);

// Internal use only. Generic loader of 16-bit data to RAM.
void NF_Load16bImgData(const char *file, u32 slot, u32 x, u32 y, u32 type);

/// Deletes from RAM the 16-bit image stored in the selected slot.
///
/// This is useful, for example, when the image has been copied to VRAM so it is
/// no longer needed in RAM.
///
/// Example:
/// ```
/// // Delete from RAM the image stored in slot 0.
/// NF_Unload16bitsBg(0);
/// ```
///
/// @param slot Slot number (0 - 15).
void NF_Unload16bitsBg(u32 slot);

/// Copy the selected 16-bit slot to VRAM or the backbuffer.
///
/// Example:
/// ```
/// // Copy the image of slot 0 to the backbuffer of the top screen
/// NF_Copy16bitsBuffer(0, 1, 0);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param destination Use 0 for VRAM or 1 for backbuffer.
/// @param slot Slot number (0 - 15).
void NF_Copy16bitsBuffer(int screen, int destination, u32 slot);

/// Draws the image in a slot into the backbuffer of the selected screen.
///
/// You can specify the coordinates where the image is drawn.
///
/// If "alpha" is set to true, all magenta pixels (0xFF00FF) won't be drawn.
///
/// Example:
/// ```
/// // Draws the image from Slot 1 to the backbuffer of the bottom screen, at
/// // coordinates (100, 50).
/// NF_ Draw16bitsImage(1, 1, 100, 50, true);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param slot Slot number (0 - 15).
/// @param x X coordinate.
/// @param y Y coordinate.
/// @param alpha True to make magenta pixels transparent.
void NF_Draw16bitsImage(int screen, u32 slot, s32 x, s32 y, bool alpha);

/// Initialize buffers to store 8-bit bitmap backgrounds.
///
/// You must call this function once in you code before loading any 8-bit
/// bitmap.
void NF_Init8bitsBgBuffers(void);

/// Resets all 8 bit background buffers and clears them.
///
/// Useful when changing game levels, for example.
void NF_Reset8bitsBgBuffers(void);

/// Loads a 8-bit bitmap from the filesystem.
///
/// The file must be in binary format ".img", and its max size is 256x256
/// pixels (64 KB). It will also load its palette in ".pal" format.
///
/// You can convert the file using this GRIT command line:
///
/// ```
/// grit file.png -ftb -fh! -gb -gB8
/// ```
///
/// If you want to share the palette with a different background:
/// ```
/// grit file1.png file2.png -ftb -fh! -gb -gu8 -gB8 -pu16 -pS -Ofile1.pal -gTFF00FF
/// cp file1.pal.bin file2.pal.bin
/// ```
///
/// If you want to display 2 8-bit backgrounds on same screen they must share
/// the palette.
///
/// You can load as many files as defined in NF_SLOTS_BG8B.
///
/// Example:
/// ```
/// // Loads "bitmap8.img" and "bitmap8.pal" to slot 0
/// NF_Load8bitsBg("bmp/bitmap8", 0);
/// ```
///
/// @param file File name without extension.
/// @param slot Slot number (0 - 15).
void NF_Load8bitsBg(const char *file, u32 slot);

/// Deletes from RAM the 8-bit image stored in the selected slot.
///
/// This is useful, for example, when the image has been copied to VRAM so it is
/// no longer needed in RAM.
///
/// Example:
/// ```
/// // Delete from RAM the image stored in slot 0.
/// NF_Unload8bitsBg(0);
/// ```
///
/// @param slot Slot number (0 - 15).
void NF_Unload8bitsBg(u32 slot);

/// Copy the selected 8-bit slot to VRAM or the backbuffer.
///
/// Example:
/// ```
/// // Copy the image of slot 0 to the backbuffer of the top screen
/// NF_Copy8bitsBuffer(0, 1, 0);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param destination Use 0 for VRAM or 1 for backbuffer.
/// @param slot Slot number (0 - 15).
void NF_Copy8bitsBuffer(int screen, int destination, u32 slot);

/// Initialize the 8 bit background backbuffer of the selected screen.
///
/// Use this function once before using the backbuffer.
///
/// @param screen Screen (0 - 1).
void NF_Init8bitsBackBuffer(int screen);

/// Enables the 8-bit backbuffer of the selected screen.
///
/// If the backbuffer is already enabled, the contents are cleared.
///
/// @param screen Screen (0 - 1).
void NF_Enable8bitsBackBuffer(int screen);

/// Disables the 8-bit backbuffer of selected screen.
///
/// It frees the RAM used by it (64 KB).
///
/// @param screen Screen (0 - 1).
void NF_Disble8bitsBackBuffer(int screen);

/// Sends the 8-bit backbuffer to the VRAM of the selected screen.
///
/// Use it to edit the image in the backbuffer and to update the image displayed
/// on the screen without showing the drawing process (which may be very slow).
///
/// You can send it to layer 2 (0) or layer 3 (1).
///
/// @param screen Screen (0 - 1).
/// @param destination Destination layer (0: layer 2, 1: layer 3).
void NF_Flip8bitsBackBuffer(int screen, int destination);

/// @}

#endif // NF_BITMAPBG_H__

#ifdef __cplusplus
}
#endif
