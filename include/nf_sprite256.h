// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - 256 color sprites functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_SPRITE256_H__
#define NF_SPRITE256_H__

/// @file   nf_sprite256.h
/// @brief  Functions related to 256 color sprites.

/// @defgroup nf_sprite256 256 color sprites
///
/// Functions to load and handle 256 color sprites.
///
/// @{

#include <nds.h>

/// Maximum number of slots of 256 color sprites
#define NF_SLOTS_SPR256GFX 256

/// Maximum number of slots of palettes of 256 color sprites
#define NF_SLOTS_SPR256PAL 64

/// Buffers to hold 256 color sprite graphics
extern char *NF_BUFFER_SPR256GFX[NF_SLOTS_SPR256GFX];

/// Buffers to hold 256 color sprite palettes
extern char *NF_BUFFER_SPR256PAL[NF_SLOTS_SPR256PAL];

/// Struct that holds information about sprite graphics in RAM
typedef struct {
    size_t size;        ///< Size in bytes of the graphics data
    u16 width;          ///< Width of graphics data
    u16 height;         ///< Height of graphics data
    bool available;     ///< True if this slot is free, false otherwise
} NF_TYPE_SPR256GFX_INFO;

/// Information of all sprite graphics in RAM
extern NF_TYPE_SPR256GFX_INFO NF_SPR256GFX[NF_SLOTS_SPR256GFX];

/// Struct that holds information about sprite palettes in RAM
typedef struct {
    size_t size;        ///< Size in bytes of the palette
    bool available;     ///< True if this slot is free, false otherwise
} NF_TYPE_SPR256PAL_INFO;

/// Information of all palettes in RAM
extern NF_TYPE_SPR256PAL_INFO NF_SPR256PAL[NF_SLOTS_SPR256PAL];

/// Struct that holds information about sprite graphics in VRAM
typedef struct {
    u32 size;           ///< Size in bytes of the graphics data
    u16 width;          ///< Width of graphics data
    u16 height;         ///< Height of graphics data
    u32 address;        ///< Address of the graphics in VRAM
    u16 ramid;          ///< RAM slot with the original copy of the graphics
    u16 framesize;      ///< Size of a frame in bytes
    u16 lastframe;      ///< Last frame index
    bool keepframes;    ///< For animated sprites, keep all frames in RAM
    bool inuse;         ///< True if this slot is in use
} NF_TYPE_SPR256VRAM_INFO;

/// Information of all sprite graphics in VRAM
extern NF_TYPE_SPR256VRAM_INFO NF_SPR256VRAM[2][128];

/// Struct that holds information about sprite palettes in VRAM
typedef struct {
    bool inuse;         ///< True if this slot is in use
    u8 ramslot;         ///< Slot index of the original palette in RAM
} NF_TYPE_SPRPALSLOT_INFO;

/// Information of all palettes in VRAM
extern NF_TYPE_SPRPALSLOT_INFO NF_SPRPALSLOT[2][16];

/// Struct that defines OAM information
typedef struct {
    u8 index;           ///< Sprite number
    s16 x;              ///< X coordinate
    s16 y;              ///< Y coordinate
    u8 layer;           ///< Layer priority
    u8 pal;             ///< Palette index
    u32 size;           ///< Sprite of the sprite (macro)
    u32 color;          ///< Color mode (macro)
    u32 *gfx;           ///< Pointer to the graphics data
    s8 rot;             ///< Rotation matrix index (0 - 31, -1 = none)
    bool doublesize;    ///< Enable double size mode when rotating
    bool hide;          ///< Hide the sprite
    bool vflip;         ///< Vertical flip
    bool hflip;         ///< Horizontal flip
    bool mosaic;        ///< Enable mosaic effect
    u16 gfxid;          ///< Graphics object ID
    u16 frame;          ///< Current frame
    u16 framesize;      ///< Size of the frame in bytes
    u16 lastframe;      ///< Last frame
    bool created;       ///< True if this sprite has been created
} NF_TYPE_SPRITEOAM_INFO;

/// OAM information of all sprites
extern NF_TYPE_SPRITEOAM_INFO NF_SPRITEOAM[2][128];

/// Struct with information of sprite allocation in VRAM
typedef struct {
    s32 free;           ///< Free VRAM
    u32 next;           ///< Next free location
    u32 last;           ///< Last used location
    u32 pos[128];       ///< Location in VRAM to reuse after a free
    u32 size[128];      ///< Size of the block to reuse
    u16 deleted;        ///< Number of free'd blocks
    s32 fragmented;     ///< Fragmented VRAM
    s32 inarow;         ///< Contiguous VRAM
    s32 max;            ///< Maxmimum addressable VRAM
} NF_TYPE_SPRVRAM_INFO;

/// Information of sprite allocation in VRAM of both screens
extern NF_TYPE_SPRVRAM_INFO NF_SPRVRAM[2];

/// Initialize library to load files from the filesystem to create 256 color
/// sprites.
///
/// Use this function once before loading any sprite from FAT. Don't call it
/// more than once.
///
/// Example:
/// ```
/// // Init buffers and variables to load sprites.
/// NF_InitSpriteBuffers();
/// ```
void NF_InitSpriteBuffers(void);

/// Reset state used for 256 color sprites loaded from FAT.
///
/// This function empties all buffers in use and resets variables to their
/// default values. It’s useful to do this when you change a level in a game,
/// to clean all stuff from RAM and make free space to load the new level.
///
/// Example:
/// ```
/// // Empty all buffers and reset variable values
/// NF_ResetSpriteBuffers();
/// ```
void NF_ResetSpriteBuffers(void);

/// Initialize sprite system in the specified screen.
///
/// - Initializes OAM with default parameters.
/// - It assigns 128 KB of VRAM for graphics and palettes.
/// - It enables extended palettes.
///
/// The VRAM mapping parameter is optional, if you don’t set it, 64 is set by
/// default. You can use up to 1024 chunks of 64 bytes (64 mapping mode) or 128
/// bytes (128 mapping mode) and 16 palettes. The use of mode 64 limits the
/// amount of usable VRAM to 64 KB. When using mode 128, 8x8 pixels sprites
/// can't be used.
///
/// Example:
/// ```
/// // Initialize 256 color sprite system in screen 0 in 64 mapping mode and in
/// // screen 1 in 128 mapping mode
/// NF_InitSpriteSys(0);
/// NF_InitSpriteSys(1, 128);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param ... VRAM mapping mode (64 or 128).
void NF_InitSpriteSys(int screen, ...);

/// Load sprite graphics from the filesystem to RAM to use it later.
///
/// You must specify the filename without extension. You must use the ".img"
/// extension in the filename of you sprite graphics file. You must select the
/// RAM slot to hold the graphics (0 to 255), and the graphics sizes. If it's
/// an animated sprite, put the size in pixels of first frame.
///
/// There are 256 available slots, as specified in:
/// ```
/// #define NF_SLOTS_SPR256GFX 256
/// ```
///
/// Example:
/// ```
/// // Loads file "name.img" from folder "stage3" and stores it in slot 100 of
/// // RAM. This graphics objectx has a size of 64 x 32.
/// NF_LoadSpriteGfx("stage3/nave", 100, 64, 32);
/// ```
///
/// @param file File name without extension.
/// @param id Slot number (0 - 255).
/// @param width Width of the graphics object (in pixels).
/// @param height Height of the graphics object (in pixels).
void NF_LoadSpriteGfx(const char *file, u32 id, u32 width, u32 height);

/// Delete from RAM the graphics of the selected slot and mark it as free.
///
/// You can delete the graphics from RAM once the sprite is created if you
/// don't need it anymore or, if it's animated, after transferring all frames to
/// VRAM.
///
/// Example:
/// ```
/// // Delete from RAM the graphics in slot 100 and mark it as free
/// NF_UnloadSpriteGfx(100);
/// ```
/// @param id Slot number (0 - 255).
void NF_UnloadSpriteGfx(u32 id);

/// Load a palette from the filesystem to RAM to be used for a sprite.
///
/// You must enter the filename without extension. You must use ".pal" extension
/// in all your palette files.
///
/// You can store up to 64 palettes, as specified in:
/// ```
/// #define NF_SLOTS_SPR256PAL 64
/// ```
///
/// Example:
/// ```
/// // Load file "player.pal" from folder "stage3" and store it in slot 34
/// NF_LoadSpritePal(“stage3/player”, 34);
/// ```
///
/// @param file File name without extension.
/// @param id Slot number (0 - 63).
void NF_LoadSpritePal(const char *file, u32 id);

/// Delete the selected palette from RAM and mark it as free.
///
/// You can delete it if you don't need it anymore or if it's already in VRAM.
///
/// @param id Slot number (0 - 63).
void NF_UnloadSpritePal(u32 id);

/// Copy a graphics object from RAM to VRAM of the selected screen to be used as
/// a sprite.
///
/// You must specify if you want to copy all frames to VRAM (false) or just the
/// first one (true).
///
/// Example:
/// ```
/// // Copy the graphics object stored in slot 160 of RAM to slot 23 of VRAM of
/// // screen 1, copying all frames if it’s animated.
/// NF_VramSpriteGfx(1, 160, 23, false);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param ram RAM slot (0 - 255).
/// @param vram VRAM slot (0 - 127).
/// @param keepframes For animated sprites. If true, copy all frames to VRAM.
void NF_VramSpriteGfx(int screen, u32 ram, u32 vram, bool keepframes);

/// Delete from VRAM the graphics object of the selected slot and a screen.
///
/// You mustn't delete the graphics while a sprite is using them.
///
/// Example:
/// ```
/// // Delete from VRAM of screen 1 the graphics in slot 34
/// NF_FreeSpriteGfx(1, 34);
/// ```
/// @param screen Screen (0 - 1).
/// @param id VRAM slot (0 - 127).
void NF_FreeSpriteGfx(int screen, u32 id);

/// Defragments the free VRAM used for sprite graphics.
///
/// This function is automaticaly executed when fragmented free VRAM is bigger
/// than 50% of the total free VRAM. You don’t need to manually execute this
/// function. You can get the state of VRAM reading the following variables:
///
/// ```
/// NF_SPRVRAM[int screen].free       // Total free VRAM
/// NF_SPRVRAM[int screen].fragmented // Total fragmented free VRAM
/// NF_SPRVRAM[int screen].inarow     // Largest free block of VRAM at the end
/// NF_SPRVRAM[int screen].lost       // Unusable free VRAM because fragmentation
/// ```
///
/// Example:
/// ```
/// // Defragments the free VRAM of sprites of screen 1
/// NF_VramSpriteGfxDefrag(1);
/// ```
///
/// @param screen Screen (0 - 1).
void NF_VramSpriteGfxDefrag(int screen);

/// Copy the palette from RAM to a slot of extended palettes in VRAM.
///
/// If the slot is in use, its contents are overwritten.
///
/// Example:
/// ```
/// // Copy palette from RAM slot 56 to the extended palettes slot 8 of screen 1
/// NF_VramSpritePal(1, 56, 8);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id VRAM slot (0 - 64).
/// @param slot VRAM slot (0 - 15).
void NF_VramSpritePal(int screen, u32 id, u32 slot);

/// Create a sprite with the specified ID in the selcted screen.
///
/// You have to select the graphics object to use, as well as the palette to
/// use. You also have to select the initial coordinates of the sprite.
///
/// Example:
/// ```
/// // Create a sprite on screen 0, with ID 12, using the graphics stored in
/// // slot 30 of VRAM and the palette in slot 1. The sprite is created at the
/// // coordinates (100, 50).
/// NF_CreateSprite(0, 12, 30, 1, 100, 50);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
/// @param gfx Graphics object ID (0 - 127).
/// @param pal Palette (0 - 15).
/// @param x X coordinate.
/// @param y Y coordinate.
void NF_CreateSprite(int screen, u32 id, u32 gfx, u32 pal, s32 x, s32 y);

/// Delete the selected sprite ID from the specified screen.
///
/// The graphics and palette used by the sprite won't be deleted from VRAM.
///
/// Example:
/// ```
/// // Delete sprite ID 12 from screen 0
/// NF_DeleteSprite(0, 12);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param id Sprite ID (0 - 127).
void NF_DeleteSprite(int screen, u32 id);

/// Copy data from the shadow OAM used by NFLib to the real OAM of libnds.
///
/// OAM must be updated only during the vertical blanking period. For example,
/// if you don't have a vertical blanking interrupt handler, you can do:
/// ```
/// NF_SpriteOamSet(0);
/// NF_SpriteOamSet(1);
/// swiWaitForVBlank();
/// oamUpdate(&oamMain);
/// oamUpdate(&oamSub);
/// ```
///
/// If your vertical blanking interrupt handler takes too long to finish,
/// oamUpdate() will be called outside of the vertical blanking period, which
/// will cause issues. In that case, call the functions from the interrupt
/// handler.
///
/// @param screen Screen (0 - 1).
void NF_SpriteOamSet(int screen);

/// Changes a color of a sprite palette in the specified screen.
///
/// The change is made directly in VRAM, so it may be overwritten from the copy
/// in RAM.
///
/// Example:
/// ```
/// // Set color 1 of palette 3 of the top screen to red.
/// NF_SpriteSetPalColor(0, 3, 1, 31, 0, 0);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param pal Palette index (0 - 15).
/// @param number Color number (0 - 255).
/// @param r Red component (0 - 31).
/// @param g Green component (0 - 31).
/// @param b Blue component (0 - 31).
void NF_SpriteSetPalColor(int screen, u32 pal, u32 number, u32 r, u32 g, u32 b);

/// Changes the value of one color in a sprite palettes of the specified screen.
///
/// The change is made in the RAM copy of the palette, so you won't see any
/// change until you update it on VRAM with NF_SpriteUpdatePalette().
///
/// Use this function to make cool effect on your sprites.
///
/// Example:
/// ```
/// // Set the value of color 1 of palette 3 of the top screen to red.
/// NF_SpriteSetPalColor(0, 3, 1, 31, 0, 0);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param pal Palette index (0 - 15).
/// @param number Color number (0 - 255).
/// @param r Red component (0 - 31).
/// @param g Green component (0 - 31).
/// @param b Blue component (0 - 31).
void NF_SpriteEditPalColor(int screen, u32 pal, u32 number, u32 r, u32 g, u32 b);

/// Updates a sprite palette in VRAM with the copy in RAM of it.
///
///Example:
/// ```
/// // Updates palette 2 of the bottom screen.
/// NF_SpriteUpdatePalette(1, 2);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param pal Palette index (0 - 15).
void NF_SpriteUpdatePalette(int screen, u32 pal);

/// Gets the RGB value of a color of a sprites palette in RAM.
///
/// Example:
/// ```
/// // Gets the RGB value of color 200 from sprites palette 3 of the bottom
/// // screen, and stores it in variables "red", "green" and "blue".
/// u8 red;
/// u8 green;
/// u8 blue;
/// NF_SpriteGetPalColor(1, 3, 200, &red, &green, &blue);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param pal Palette index (0 - 15).
/// @param number Color number (0 - 255).
/// @param r Red component (0 - 31).
/// @param g Green component (0 - 31).
/// @param b Blue component (0 - 31).
void NF_SpriteGetPalColor(int screen, u32 pal, u32 number, u8 *r, u8 *g, u8 *b);

/// @}

#endif // NF_SPRITE256_H__

#ifdef __cplusplus
}
#endif
