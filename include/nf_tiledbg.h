// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Tiled background functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_TILEDBG_H__
#define NF_TILEDBG_H__

/// @file   nf_tiledbg.h
/// @brief  Functions related to regular tiled backgrounds.

/// @defgroup nf_tiledbg Functions related to regular tiled backgrounds.
///
/// Functions related to regular (non-affine) tiled backgrounds.
///
/// @{

#include <nds.h>

/// Maximum number of slots of tiled backgrounds
#define NF_SLOTS_TBG 64

/// Maximum number of slots for extended palettes (max 16 per background)
#define NF_SLOTS_EXBGPAL 128

/// Maxmimum number of VRAM blocks used for tilesets
#define NF_MAX_BANKS_TILES 8

/// Maxmimum number of VRAM blocks used for maps
#define NF_MAX_BANKS_MAPS 16

/// Maxmimum number of tile banks.
///
/// 1 bank = 16 KB. One tile bank is equivalent to 8 map banks.
extern u8 NF_BANKS_TILES[2];

/// Maxmimum number of map banks.
///
/// 1 bank = 2 KB. One tile bank is equivalent to 8 map banks.
///
/// Use multiples of 8 for those values.
extern u8 NF_BANKS_MAPS[2];

/// Buffers to hold background tiles.
extern char *NF_BUFFER_BGTILES[NF_SLOTS_TBG];

/// Buffers to hold background maps.
extern char *NF_BUFFER_BGMAP[NF_SLOTS_TBG];

/// Buffers to hold background palettes.
extern char *NF_BUFFER_BGPAL[NF_SLOTS_TBG];

/// Struct that holds information about regular tiled backgrounds.
typedef struct {
    char name[32];      ///< Background name
    size_t tilesize;    ///< Tileset size
    size_t mapsize;     ///< Map size
    size_t palsize;     ///< Palette size
    u16 width;          ///< Background width
    u16 height;         ///< Background height
    bool available;     ///< If the background is available it is true.
} NF_TYPE_TBG_INFO;

/// Information of all tiled backgrounds.
extern NF_TYPE_TBG_INFO NF_TILEDBG[NF_SLOTS_TBG];

/// Struct that holds information about extended palettes.
typedef struct {
    char *buffer;   ///< Buffer that holds the palette
    size_t palsize; ///< Palette size
    bool inuse;     ///< True if the slot is in use.
} NF_TYPE_EXBGPAL_INFO;

/// Information of all extended palettes.
extern NF_TYPE_EXBGPAL_INFO NF_EXBGPAL[NF_SLOTS_EXBGPAL];

/// Struct that holds information about backgrounds loaded to the screen.
///
/// The hardware of the DS doesn't allow using maps bigger than 512x512 pixels.
/// It is needed to keep a value that speficies if the hardware manages the
/// background (if it's smaller or equal to 512x512), or if we use the tile
/// swapping engine of NFLib (if it's bigger).
///
/// Background types:
/// - 0: Normal (max is 512 x 512)
/// - 1: Larger than 512 x 256
/// - 2: 256 x Largen than 512
/// - 3: Both dimensions are larger than 512
typedef struct {
    u8 tilebase;        ///< Initial VRAM block used by the tile set
    u8 tileblocks;      ///< Number of blocks used by the tile set
    u8 mapbase;         ///< Initial VRAM block used by the map
    u8 mapblocks;       ///< Number of blocks used by the map
    u16 bgwidth;        ///< Background width
    u16 bgheight;       ///< Background height
    u16 mapwidth;       ///< Map width
    u16 mapheight;      ///< Map height
    u8 bgtype;          ///< Background type
    u8 bgslot;          ///< Graphics buffer used (NF_BUFFER_BGMAP)
    u8 blockx;          ///< Map block (horizontal)
    u8 blocky;          ///< Map block (vertical)
    bool created;       ///< True if the background has been created
} NF_TYPE_TBGLAYERS_INFO;

/// Information of all backgrounds loaded to the screen
extern NF_TYPE_TBGLAYERS_INFO NF_TILEDBG_LAYERS[2][4]; //[screen][layer]

/// Array of free blocks used for tiles
extern u8 NF_TILEBLOCKS[2][NF_MAX_BANKS_TILES];

/// Array of free blocks used for maps
extern u8 NF_MAPBLOCKS[2][NF_MAX_BANKS_MAPS];

/// Initialize library to load files from the filesystem to create tiled BGs.
///
/// Use this function once before loading any BG from FAT. Don't call it more
/// than once.
///
/// Example:
/// ```
/// // Init buffers and variables to load tiled BGs
/// NF_InitTiledBgBuffers();
/// ```
void NF_InitTiledBgBuffers(void);

/// Reset state used for tiled BGs loaded from FAT.
///
/// This function empties all buffers in use and resets variables to their
/// default values. It’s useful to do this when you change a level in a game,
/// to clean all stuff from RAM and make free space to load the new level.
///
/// Example:
/// ```
/// // Empty all buffers and reset variable values
/// NF_ResetTiledBgBuffers();
/// ```
void NF_ResetTiledBgBuffers(void);

/// Initialize the tiled BG engine of the selected screen.
///
/// You must call this function before using any tiled BG. This function:
///
/// - Inititializes all variables to control BGs, tiles, palettes and maps.
/// - Configures VRAM to use 128 KB for BGs (96 KB for tiles, 32 KB for maps).
/// - Activates all 4 layers to use with tiled BGs.
/// - Reserves 8 banks of 16 KB for tiles (2 for maps, 6 for tiles).
/// - Reserves 16 banks of 2 KB for maps. The first 2 tile banks will be used
///   for this.
/// - Enables extended palettes.
///
/// The memory allocated for tiles and maps is defined in:
/// ```
/// #define NF_BANKS_TILES 8
/// #define NF_BANKS_MAPS 16
/// ```
/// Each tile bank is as big as 8 map banks.
///
/// Example:
/// ```
/// // Init tiled BG system of screen 1
/// NF_InitTiledBgSys(1);
/// ```
///
/// @param screen Screen (0 - 1).
void NF_InitTiledBgSys(int screen);

/// Load all files needed to create a tiled BG from FAT to RAM.
///
/// All files for a BG must have the same name, using IMG extension for tiles
/// files, MAP for map files and PAL for palette files.
///
/// Check the GRIT folder for more info about BG files conversion.
///
/// You can load up to 32 BGs at the same time, this is defined in:
/// ```
/// #define NF_SLOTS_TBG 32
/// ```
///
/// Example:
/// ```
/// // Load to RAM files "mainstage.img", "mainstage.map" and "mainstage.pal"
/// // from the "stage1" subfolder and call it “mifondo”. Also store the size of
/// // the BG (2048 x 256).
/// NF_LoadTiledBg("stage1/mainstage", "mifondo", 2048, 256);
/// ```
///
/// @param file File path without extension.
/// @param name Name used for the BG for other functions.
/// @param width BG width.
/// @param height BG height.
void NF_LoadTiledBg(const char *file, const char *name, u32 width, u32 height);

/// Load a tilesed and palette from FAT to RAM.
///
/// It works like NF_LoadTiledBg() but it lets you specify the range of tiles to
/// load. Also, no actual map is loaded. Instead, a blank map of the given size
/// is created. The background is created using NF_CreateTiledBg().
///
/// Example:
/// ```
/// // Load to RAM tiles 0 to 23 (24 tiles in total) from "mainstage.img" and
/// // the palette (from "mainstage.pal") file, from "stage1" subfolder and
/// // assigns the "mifondo" name to the background. It also sets the size of
/// // background to 256x256 pixels. This creates a 32x32 tiles blank map.
/// NF_LoadTilesForBg("stage1/mainstage", "mifondo", 256, 256, 0, 23);
/// ```
///
/// @param file File name, without extension.
/// @param name Name of the BG.
/// @param width Width of the BG in pixels.
/// @param height Height of the BG in pixels.
/// @param tile_start First tile to load.
/// @param tile_end Last tile to load.
void NF_LoadTilesForBg(const char *file, const char *name, u32 width, u32 height,
                       u32 tile_start, u32 tile_end);

/// Delete from RAM the BG with the specified name.
///
/// You can delete from RAM the BG if you don't need it more or if the size of
/// the BG size is less or equal than 512 x 512. If it's bigger, you must keep
/// it in RAM until you don't need it anymore.
///
/// Example:
/// ```
/// // Delete from RAM the BG with name "mifondo" and mark as free the slot it
/// // uses.
/// NF_UnloadTiledBg("mifondo");
/// ```
///
/// @param name Name used for the BG.
void NF_UnloadTiledBg(const char *name);

/// Create a BG on the screen, using data loaded in RAM.
///
/// This function copies to VRAM all required data. Before you create the BG,
/// you must load data to RAM using NF_LoadTiledBg(). The BG is created on the
/// specified screen and layer.
///
/// Example:
/// ```
/// // Create a tiled BG on layer 3 of screen 0, using the BG called "mifondo"
/// NF_CreateTiledBg(0, 3, "mifondo");
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param name Name used for the BG.
void NF_CreateTiledBg(int screen, u32 layer, const char *name);

/// Delete the BG of the specified screen and layer.
///
/// This also deletes from VRAM the data used by this BG.
///
/// Example:
/// ```
/// // Deletes the tiled BG from layer 3 of screen 0
/// NF_DeleteTiledBg(0, 3);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
void NF_DeleteTiledBg(int screen, u32 layer);

/// Gets the address of the tile at the specified position.
///
/// Internal use.
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param tile_x X coordinate.
/// @param tile_y Y coordinate.
/// @return Tile address.
u32 NF_GetTileMapAddress(int screen, u32 layer, u32 tile_x, u32 tile_y);

/// Gets the value of the tile at the specified position.
///
/// Example:
/// ```
/// // Gets the value of the tile at (20, 10) of the map loaded on screen 0,
/// // layer 2.
/// u16 mytile = NF_GetTileOfMap(0, 2, 10, 20);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param tile_x X coordinate.
/// @param tile_y Y coordinate.
/// @return Tile index.
u32 NF_GetTileOfMap(int screen, u32 layer, u32 tile_x, u32 tile_y);

/// Sets the value of the tile at the specified position.
///
/// Example:
/// ```
/// // Sets to 5 the tile at (20, 10) of the map loaded on screen 0, layer 2.
/// NF_SetTileOfMap(0, 2, 10, 20, 5);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param tile_x X coordinate.
/// @param tile_y Y coordinate.
/// @param tile Tile index.
void NF_SetTileOfMap(int screen, u32 layer, u32 tile_x, u32 tile_y, u32 tile);

/// Updates the map of the specified screen and layer specified.
///
/// This updates the map on VRAM with the copy of RAM, that can be modified. Use
/// this fuction to apply changes made with NF_SetTileOfMap().
///
/// Example:
/// ```
/// // Update the map in VRAM with the modified copy in RAM of screen 0, layer 2
/// NF_UpdateVramMap(0, 2);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
void NF_UpdateVramMap(int screen, u32 layer);

/// Changes the value of one color of the palette of a background.
///
/// The change is made directly in VRAM, so it may be overwritten from the copy
/// in RAM. It's also a very slow function, use it twice or 3 times per frame.
///
/// Example:
/// ```
/// // Change the value of color 1 of the palette of layer 3 on top screen to
/// // red. If this layer is a text layer with the default font, the text
/// // becomes red.
/// NF_BgSetPalColor(0, 3, 1, 31, 0, 0);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param number Color number (0 - 255).
/// @param r Red component (0 - 31).
/// @param g Green component (0 - 31).
/// @param b Blue component (0 - 31).
void NF_BgSetPalColor(int screen, u32 layer, u32 number, u32 r, u32 g, u32 b);

/// Changes the value of one color of the palete of the specified background.
///
/// The change is made in the RAM copy of the palette, so you won't see any
/// change until you update it on VRAM with NF_BgUpdatePalette().
///
/// Use this function to make cool effect on your tiled backgrounds.
///
/// Example:
/// ```
/// // Set color 1 of the palette of layer 3 on top screen to red
/// NF_BgSetPalColor(0, 3, 1, 31, 0, 0);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param number Color number (0 - 255).
/// @param r Red component (0 - 31).
/// @param g Green component (0 - 31).
/// @param b Blue component (0 - 31).
void NF_BgEditPalColor(int screen, u32 layer, u32 number, u32 r, u32 g, u32 b);

/// Updates the palette of a background from RAM to VRAM.
///
/// Example:
/// ```
/// // Updates the palette of layer 2 of the bottom screen
/// NF_BgUpdatePalette(1, 2);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
void NF_BgUpdatePalette(int screen, u32 layer);

/// Gets the RGB value of a color of the palette of the selected background,
/// which is loaded in RAM.
///
/// Example:
/// ```
/// // Gets the RGB value of color number 200 of layer 3 of the bottom screen,
/// // stores it in the "red", "green" and "blue" variables.
/// u8 red;
/// u8 green;
/// u8 blue;
/// NF_BgGetPalColor(1, 3, 200, &red, &green, &blue);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param number Color number (0 - 255).
/// @param r Red component result (0 - 31).
/// @param g Green component result (0 - 31).
/// @param b Blue component result (0 - 31).
void NF_BgGetPalColor(int screen, u32 layer, u32 number, u8 *r, u8 *g, u8 *b);

/// Returns the number of extended palette used by specified tile.
///
/// By default, all tiles use extended palette 0.
///
/// Example:
/// ```
/// // Returns the extended palette used by tile in position (20, 10) of layer 3
/// // of the top screen
/// palette = NF_GetTilePal(0, 3, 20, 10);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param tile_x X coordinate.
/// @param tile_y Y coordinate.
/// @return Extended palette index (0 - 15).
u32 NF_GetTilePal(int screen, u32 layer, u32 tile_x, u32 tile_y);

/// Sets the extended palette to use for the tile especified.
///
/// The palette has to be loaded in VRAM, and the changes won't be visible until
/// you use NF_UpdateVramMap() because all operations are done in the copy of
/// the map in RAM.
///
/// Example:
/// ```
/// // Sets tile on position (20, 10) of layer 3 on top screen to use the
/// // extended palette 2
/// NF_SetTilePal(0, 3, 20, 10, 2);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param tile_x X coordinate.
/// @param tile_y Y coordinate.
/// @param pal Extended palette index (0 - 15).
void NF_SetTilePal(int screen, u32 layer, u32 tile_x, u32 tile_y, u32 pal);

/// Load a palette from a file to RAM to use it as a background extended
/// palette.
///
/// Example:
/// ```
/// // Loads the "bg/sunset.pal" file from the filesystem to RAM slot 3.
/// NF_LoadExBgPal("bg/sunset", 3);
/// ```
///
/// @param file File (.pal extension).
/// @param slot RAM slot (0 - 127)
void NF_LoadExBgPal(const char *file, u32 slot);

/// Deletes a loaded palette from RAM.
///
/// Example:
/// ```
/// // Erase from RAM the palette loaded in slot 5. If the palette is already
/// // transfered to VRAM, you can still use it until you update the palettes.
/// NF_UnloadExBgPal(5);
/// ```
///
/// @param slot RAM slot (0 - 127)
void NF_UnloadExBgPal(u32 slot);

/// Transfers a palette from RAM to VRAM to be used as extended palette.
///
/// Example:
/// ```
/// // Transfers the palette from RAM slot 100 to VRAM of layer 3 of the top
/// // screen, to be used as extended palette 10.
/// NF_VramExBgPal(0, 3, 100, 10);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param id Slot of the palette in RAM.
/// @param slot Slot of extended palette in VRAM.
void NF_VramExBgPal(int screen, u32 layer, u32 id, u32 slot);

/// Sets the extended palette to be used in the specified background.
///
/// Example:
/// ```
/// // The backgroun on layer 3 of top screen, uses the extended palette 5
/// NF_SetExBgPal(0, 3, 5);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param pal Extended palette index (0 - 15).
void NF_SetExBgPal(int screen, u32 layer, u32 pal);

/// Inverts the horizontal flip status of a tile in the specified map.
///
/// The changes are done to the copy of the map in RAM, so changes won't be
/// visible until you update the copy in VRAM with NF_UpdateVramMap().
///
/// Example:
/// ```
/// // Flips horizontaly the tile in the position (10, 20) of layer 1 of the top
/// // screen
/// NF_SetTileHflip(0, 1, 10, 20);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param tile_x X coordinate.
/// @param tile_y Y coordinate.
void NF_SetTileHflip(int screen, u32 layer, u32 tile_x, u32 tile_y);

/// Inverts the vertical flip status of a tile in the specified map.
///
/// The changes are done to the copy of the map in RAM, so changes won't be
/// visible until you update the copy in VRAM with NF_UpdateVramMap().
///
/// Example:
/// ```
/// // Flips vertically the tile in the position (10, 20) of layer 1 of the top
/// // screen
/// NF_SetTileVflip(0, 1, 10, 20);
/// ```
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param tile_x X coordinate.
/// @param tile_y Y coordinate.
void NF_SetTileVflip(int screen, u32 layer, u32 tile_x, u32 tile_y);

/// Rotates the graphics of a tile in RAM by the specified angle.
///
/// The rotated graphics are stored int the same tile.
///
/// Rotation values:
/// 1 - 90 degrees clockwise
/// 2 - 90 degrees counter-clockwise
/// 3 - 180 degrees
///
/// Example:
/// ```
/// // Rotates 90 degrees counter-clockwise tile 76 of the buffer in slot 3
/// NF_RotateTileGfx(3, 76, 2);
/// ```
///
/// @param slot RAM slot (0 - 127).
/// @param tile Tile index.
/// @param rotation Rotation value.
void NF_RotateTileGfx(u32 slot, u32 tile, u32 rotation);

/// @}

#endif // NF_TILEDBG_H__

#ifdef __cplusplus
}
#endif
