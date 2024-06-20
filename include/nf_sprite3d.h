// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - 3D sprite functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_SPRITE3D_H__
#define NF_SPRITE3D_H__

#include <nds.h>

/// @file   nf_sprite3d.h
/// @brief  Functions related to sprites rendered by the 3D GPU.

/// @defgroup nf_sprite3d 3D sprite functions.
///
/// These functions are special, since they use the 3D engine to draw sprites
/// by using textured polygons. It can only be used in one screen at the same
/// time, and background layer 0 is replaced by the 3D output.
///
/// In return we can create up to 256 sprites of a maximum size of 1024x1024, we
/// can use any size in base 2, and use a maximum of 32 palettes simultaneously.
///
/// For the loading of graphics and palettes, use the same functions as with 2D
/// sprites. You can convert indexed images of 256 colors to create textures for
/// 3D sprites with the following grit command:
///
/// ```
/// grit image.png -ftb -fh! -gb -gu8 -gB8 -pu8 -gTFF00FF
/// ```
///
/// You can also use the same command used for 8 bit backgrounds.
///
/// @{

/// Maximum number of slots of 3D sprites
#define NF_3DSPRITES 256

/// Struct that holds information about 3D sprites
typedef struct {
    s16 x;                  ///< X coordinate
    s16 y;                  ///< Y coordinate
    s16 z;                  ///< Z coordinate
    s16 rx;                 ///< X axis rotation (-512 << 6 to 512 << 6)
    s16 ry;                 ///< Y axis rotation (-512 << 6 to 512 << 6)
    s16 rz;                 ///< Z axis rotation (-512 << 6 to 512 << 6)
    bool rot;               ///< True if the sprite is being rotated
    u16 sx;                 ///< X scale (0 to 64 << 6 to more than 512 << 6)
    u16 sy;                 ///< Y scale (0 to 64 << 6 to more than 512 << 6)
    bool scale;             ///< True if the sprite is being scaled
    s16 width;              ///< Sprite width
    s16 height;             ///< Sprite height
    bool inuse;             ///< True if the sprite is being used
    bool show;              ///< True if the sprite has to be drawn
    u32 gfx_tex_format;     ///< Texture format
    u32 gfx;                ///< Address of the texture in VRAM
    u16 gfxid;              ///< ID of the texture/graphics being used by the sprite
    u16 frame;              ///< Current frame
    u16 newframe;           ///< Next frame
    u16 framesize;          ///< Frame size in bytes
    u16 lastframe;          ///< Last frame
    u32 gfx_pal_format;     ///< Palette format
    u32 pal;                ///< Address of the palette in VRAM
    u16 palid;              ///< ID of the palette being used by the sprite
    u16 prio;               ///< Sprite priority (lower values are higher priorities)
    u8 poly_id;             ///< Polygon ID (0 by default, don't use 63)
    u8 alpha;               ///< Alpha value (0 - 31) (31 by default)
} NF_TYPE_3DSPRITE_INFO;

/// Information of all 3D sprites
extern NF_TYPE_3DSPRITE_INFO NF_3DSPRITE[NF_3DSPRITES];

/// Struct that holds information about 3D sprite textures in VRAM
typedef struct {
    u32 size;               ///< Size of the texture in bytes
    u16 width;              ///< Width
    u16 height;             ///< Height
    u32 address;            ///< Address in VRAM
    u16 ramid;              ///< RAM slot where it comes from
    u16 framesize;          ///< Frame size in bytes
    u16 lastframe;          ///< Last frame
    bool keepframes;        ///< For animated sprites, keep all frames in RAM
    bool inuse;             ///< True if this slot is in use
} NF_TYPE_TEX256VRAM_INFO;

/// Information of all 3D sprite textures in VRAM
extern NF_TYPE_TEX256VRAM_INFO NF_TEX256VRAM[NF_3DSPRITES];

/// Struct that holds information about 3D sprite palettes in VRAM
typedef struct {
    bool inuse;     ///< True if this slot is in use
    u8 ramslot;     ///< Slot index of the original palette in RAM
} NF_TYPE_3DSPRPALSLOT_INFO;

/// Information of all 3D sprite palettes in VRAM
extern NF_TYPE_3DSPRPALSLOT_INFO NF_TEXPALSLOT[32];

/// Struct with information of 3D sprite allocation in VRAM
typedef struct {
    s32 free;                   ///< Free VRAM
    u32 next;                   ///< Next free location
    u32 last;                   ///< Last used location
    u32 pos[NF_3DSPRITES];      ///< Location in VRAM to reuse after a free
    u32 size[NF_3DSPRITES];     ///< Size of the block to reuse
    u16 deleted;                ///< Number of free'd blocks
    s32 fragmented;             ///< Fragmented VRAM
    s32 inarow;                 ///< Contiguous VRAM
} NF_TYPE_TEXVRAM_INFO;

/// Information of 3D sprite allocation in VRAM
extern NF_TYPE_TEXVRAM_INFO NF_TEXVRAM;

/// Struct with information about created 3D sprites
typedef struct {
    s16 total;                  ///< Total number of 3D sprites created
    u16 id[NF_3DSPRITES];       ///< Sprite IDs
    u16 bck[NF_3DSPRITES];      ///< Backup IDs
} NF_TYPE_CREATED_3DSPRITE_INFO;

/// Information of created 3D sprites
extern NF_TYPE_CREATED_3DSPRITE_INFO NF_CREATED_3DSPRITE;

/// Initialize 3D sprite system.
///
/// Asigns 128 KB of VRAM for textures and 16 KB for palettes.
///
/// It enables extended palettes.
///
/// Example:
/// ```
/// // Initialize the 3D sprite system
/// NF_Init3dSpriteSys();
/// ```
void NF_Init3dSpriteSys(void);

/// Copy a texture from RAM to VRAM to use it for 3D sprites.
///
/// You must specify if you want to copy all frames to VRAM (false) or just the
/// first one (true).
///
/// Example:
/// ```
/// // Copy the texture stored in slot 160 of RAM to slot 23 of VRAM, copying
/// // all frames if it's animated.
/// NF_Vram3dSpriteGfx(160, 23, false);
/// ```
///
/// @param ram RAM slot (0 - 255)
/// @param vram VRAM slot (0 - 255)
/// @param keepframes For animated sprites. If true, copy all frames to VRAM.
void NF_Vram3dSpriteGfx(u32 ram, u32 vram, bool keepframes);

/// Delete from VRAM the texture in the selected slot.
///
/// You mustn't delete the graphics while a sprite is using them.
///
/// Example:
/// ```
/// // Delete from VRAM the texture in slot 34.
/// NF_Free3dSpriteGfx(34);
/// ```
///
/// @param id VRAM slot (0 - 255)
void NF_Free3dSpriteGfx(u32 id);

/// Defragments the free VRAM used for 3D sprite textures.
///
/// This function is automaticaly executed when fragmented free VRAM is bigger
/// than 50% of the total free VRAM. You don’t need to manually execute this
/// function. You can get the state of VRAM reading the following variables:
///
/// ```
/// NF_TEXVRAM[screen].free       // Total free VRAM
/// NF_TEXVRAM[screen].fragmented // Total fragmented free VRAM
/// NF_TEXVRAM[screen].inarow     // Largest free block of VRAM at the end
/// NF_TEXVRAM[screen].lost       // Unusable free VRAM because fragmentation
/// ```
void NF_Vram3dSpriteGfxDefrag(void);

/// Copy a palette from RAM to the specified slot in VRAM.
///
/// If the slot is in use, its contents are overwritten.
///
/// Example:
/// ```
/// // Copy the palette from RAM slot 56 to VRAM slot 8
/// NF_VramSpritePal(56, 8);
/// ```
///
/// @param id RAM slot (0 - 64)
/// @param slot VRAM slot (0 - 31)
void NF_Vram3dSpritePal(u32 id, u32 slot);

/// Create a 3D sprite with the specified ID and display it on the screen.
///
/// Example:
/// ```
/// // Create a sprite with ID 12, using the texture stored in slot 30 of VRAM
/// // and the palette in slot 1. The sprite is created at coordinates (100, 50)
/// NF_Create3dSprite(12, 30, 1, 100, 50);
/// ```
///
/// @param id Sprite ID (0 - 255).
/// @param gfx Texture slot (0 - 255).
/// @param pal Palette slot (0 - 31).
/// @param x X coordinate.
/// @param y Y coordinate.
void NF_Create3dSprite(u32 id, u32 gfx, u32 pal, s32 x, s32 y);

/// Remove the sprite with the specified ID from the screen.
///
/// The texture and palette used by the sprite won't be deleted from VRAM.
///
/// Example:
/// ```
/// // Delete from screen the sprite with ID 12
/// NF_Delete3dSprite(12);
/// ```
///
/// @param id Sprite ID (0 - 255).
void NF_Delete3dSprite(u32 id);

/// Sets the priorities of 3D sprites based on their sprite IDs.
///
/// Lower sprite IDs have higher priorities.
void NF_Sort3dSprites(void);

/// Changes the draw priority of the 3D sprite with the specified ID.
///
/// Lower priority values have a higher priority.
///
/// @param id Sprite ID (0 - 255).
/// @param prio Priority (0 - 255).
void NF_Set3dSpritePriority(u32 id, u32 prio);

/// Swaps the priority of two 3D sprites.
///
/// @param id_a Sprite ID A (0 - 255).
/// @param id_b Sprite ID B (0 - 255).
void NF_Swap3dSpritePriority(u32 id_a, u32 id_b);

/// Move a 3D sprite to the specified position.
///
/// Example:
/// ```
/// // Moves 3D sprite 35 to coordinates (100, 50).
/// NF_Move3dSprite(35, 100, 50);
/// ```
///
/// @param id Sprite ID (0 - 255).
/// @param x X coordinate.
/// @param y Y coordinate.
static inline void NF_Move3dSprite(u32 id, s32 x, s32 y)
{
    NF_3DSPRITE[id].x = x;
    NF_3DSPRITE[id].y = y;
}

/// Show or hide a 3D sprite.
///
/// If you hide it, the 3D sprite becomes invisible, but it isn't deleted.
///
/// Example:
/// ```
/// // Hide 3D sprite 35.
/// NF_Show3dSprite(35, false);
/// // Make 3D sprite 45 visible
/// NF_Show3dSprite(45, true);
/// ```
///
/// @param id Sprite ID (0 - 255).
/// @param show True to show the sprite, false to hide it.
static inline void NF_Show3dSprite(u32 id, bool show)
{
    NF_3DSPRITE[id].show = show;
}

/// Select the frame of an animation to display in the 3D sprite.
///
/// Example:
/// ```
/// // Make sprite 20 show frame 5
/// NF_Set3dSpriteFrame(20, 5);
/// ```
///
/// @param id Sprite ID (0 - 255).
/// @param frame Frame index.
void NF_Set3dSpriteFrame(u32 id, u32 frame);

/// Draw all created 3D sprites on the screen.
///
/// You need to call this function once per frame. This is the basic code to
/// show them:
/// ```
/// // Draw all 3D Sprites
/// NF_Draw3dSprites();
/// // Tell the 3D hardware that all commands for this frame have been sent
/// glFlush(0);
/// // Wait for the vertical sync
/// swiWaitForVBlank();
/// ```
void NF_Draw3dSprites(void);

/// Update the textures of all animated 3D sprites that need it.
///
/// Use this if any of your 3D sprites has the flag "keepframes" set to true.
///
/// Call this function just after swiWaitForVBlank().
void NF_Update3dSpritesGfx(void);

/// Rotates a 3D sprite on its 3 axes.
///
/// You can set a rotation between -512 and 512, with 0 meaning no rotation.
///
/// @param id Sprite ID (0 - 255).
/// @param x Rotation by X axis.
/// @param y Rotation by Y axis.
/// @param z Rotation by Z axis.
void NF_Rotate3dSprite(u32 id, s32 x, s32 y, s32 z);

/// Scales 3D sprite on X and Y axes.
///
/// The scale range goes from 0 to 64 (original size) to any greater value.
///
/// @param id Sprite ID (0 - 255).
/// @param x X axis scale.
/// @param y Y axis scale.
void NF_Scale3dSprite(u32 id, u32 x, u32 y);

/// Set the alpha level of a 3D sprite.
///
/// For transparency to work properly between different translucent sprites you
/// need to use different polygon IDs for them (between 1 and 62). The alpha
/// range is 0 to 31, with 31 being fully opaque.
///
/// To disable transparency, select an alpha value of 31 or a polygon ID of 0.
///
/// @param sprite Sprite ID (0 - 255).
/// @param poly_id Polygon ID (1 - 62).
/// @param alpha Transparency (0 - 31).
void NF_Blend3dSprite(u32 sprite, u32 poly_id, u32 alpha);

/// Select the layer where 3D sprites are drawn.
///
/// This function only changes the priority of background layer 0. The 3D output
/// can only be sent to layer 0 because of how the hardware was designed.
///
/// @param layer Layer (0 - 3).
void NF_3dSpritesLayer(u32 layer);

/// Changes one color of one 3D sprite palette.
///
/// The change is made to the RAM copy of the palette, so you won't see any
/// change until you update the VRAM copy with NF_3dSpriteUpdatePalette(). Use
/// this function to make cool effects with your 3D sprites.
///
/// Example:
/// ```
/// // Set color 1 of palette 3 to red
/// NF_3dSpriteSetPalColor(3, 1, 31, 0, 0);
/// ```
///
/// @param pal Palette slot (0 - 31).
/// @param number Color number (0 - 255).
/// @param r Red component (0 - 31).
/// @param g Green component (0 - 31).
/// @param b Blue component (0 - 31).
void NF_3dSpriteEditPalColor(u32 pal, u32 number, u32 r, u32 g, u32 b);

/// Updates the VRAM copy of a palette with the RAM copy of it.
///
/// Example:
/// ```
/// // Update palette 2
/// NF_3dSpriteUpdatePalette(2);
/// ```
///
/// @param pal Palette slot (0 - 31).
void NF_3dSpriteUpdatePalette(u32 pal);

/// Gets the RGB value of a color from a 3D sprite palette loaded in RAM.
///
/// Example:
/// ```
/// // Gets the RGB values of color 200 of sprite palette 3 and stores it in
/// // variables "red", "green" and "blue".
/// u8 red;
/// u8 green;
/// u8 blue;
/// NF_3dSpriteGetPalColor(3, 200, &red, &green, &blue);
/// ```
///
/// @param pal Palette slot (0 - 31).
/// @param number Color number (0 - 255).
/// @param r Red component (0 - 31).
/// @param g Green component (0 - 31).
/// @param b Blue component (0 - 31).
void NF_3dSpriteGetPalColor(u32 pal, u32 number, u8 *r, u8 *g, u8 *b);

/// Sets the depth of the selected 3D sprite.
///
/// -512 is highest priority (on top of the other sprites). 0 is the default
/// value, and 512 the lowest priority.
///
/// Change the sprite depth to prevent it from intersecting with other sprites
/// when rotation or zoom are applied.
///
/// @param id Sprite ID (0 - 255).
/// @param z Depth (-512 - 512)
void NF_3dSpriteSetDepth(u32 id, s32 z);

// Defines for backwards compatibility
#define NF_3dSpriteSetDeep NF_3dSpriteSetDepth

/// @}

#endif // NF_SPRITE3D_H__

#ifdef __cplusplus
}
#endif
