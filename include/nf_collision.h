// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Collision maps functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_COLLISION_H__
#define NF_COLLISION_H__

#include <nds.h>

/// @file   nf_collision.h
/// @brief  Collision map support.

/// @defgroup nf_collision Collision map support.
///
/// Collision maps only have map information, so the collisions have a
/// granularity of 8x8 pixels. This is useful for games where the map is simple
/// and it doesn't have many details that affect collisions.
///
/// Collision backgrounds also have tile information, so they can have
/// information at a pixel granularity.
///
/// @{

/// Maximum number of available collision maps
#define NF_SLOTS_CMAP 32

/// Struct that holds collision map information
typedef struct {
    char *tiles;        ///< Tileset data
    char *map;          ///< Map data
    size_t tiles_size;  ///< Size of the tileset
    size_t map_size;    ///< Size of the map
    u16 width;          ///< Width of map in pixels
    u16 height;         ///< Height of map in pixels
    bool inuse;         ///< True if the slot is in use
} NF_TYPE_CMAP_INFO;

/// Information of all collision maps
extern NF_TYPE_CMAP_INFO NF_CMAP[NF_SLOTS_CMAP];

/// Initialize buffers to store collision map data.
///
/// You must call this function once in you code before loading any collision
/// map.
void NF_InitCmapBuffers(void);

/// Reset collision map buffers, clearing all data in RAM.
///
/// It's useful to use this function during game level changes, for example, to
/// easily clear all data before loading the new level.
void NF_ResetCmapBuffers(void);

/// Load a collision map into RAM in the specified slot.
///
/// You must specify the width and height of the map in pixels. Remember to make
/// your collision map 8 pixels taller than your background and to use this
/// first row of tiles to define your tileset for the collision map.
///
/// Use grit to convert your maps. You need to copy the ".map" file to your game
/// data folder.
///
/// @param file File name
/// @param id Slot number (0 - 31)
/// @param width Map width (in pixels)
/// @param height Map height (in pixels)
void NF_LoadCollisionMap(const char *file, u32 id, u32 width, u32 height);

/// Unload from RAM the collision map at the specified slot.
///
/// @param id Slot number (0 - 31)
void NF_UnloadCollisionMap(u32 id);

/// Return the tile number at the specified position.
///
/// You must place your tileset in the first row of the collision map image.
///
/// @param slot Slot number (0 - 31)
/// @param x X coordinate in pixels.
/// @param y Y coordinate in pixels.
/// @return Tile index.
u32 NF_GetTile(u32 slot, s32 x, s32 y);

/// Set the value of the tile of a collision map at the specified position.
///
/// @param slot Slot number (0 - 31)
/// @param x X coordinate in pixels.
/// @param y Y coordinate in pixels.
/// @param value New tile.
void NF_SetTile(u32 slot, s32 x, s32 y, u32 value);

/// Load a collision background to RAM at the specified slot.
///
/// You must specify the width and height of the background in pixels. Remember
/// to make your colision background 8 pixels taller than your real background
/// and to use this first row of tiles to define your color tileset for the
/// collision background.
///
/// Use grit to convert your maps. You need to copy the ".map" and ".img" files
/// to your game data folder.
///
/// @param file File name
/// @param id Slot number (0 - 31)
/// @param width Map width (in pixels)
/// @param height Map height (in pixels)
void NF_LoadCollisionBg(const char *file, u32 id, u32 width, u32 height);

/// Unload from RAM the collision background at the specified slot.
///
/// @param id Slot number (0 - 31)
void NF_UnloadCollisionBg(u32 id);

/// Returns the color number at the specified coordinates.
///
/// If the coordinates are outside of the background, it returns 0.
///
/// @param slot Slot number (0 - 31)
/// @param x X coordinate in pixels.
/// @param y Y coordinate in pixels.
/// @return Tile index (0 - 255).
u8 NF_GetPoint(u32 slot, s32 x, s32 y);

// Defines for backwards compatibility
#define NF_LoadColisionMap NF_LoadCollisionMap
#define NF_UnloadColisionMap NF_UnloadCollisionMap
#define NF_LoadColisionBg NF_LoadCollisionBg
#define NF_UnloadColisionBg NF_UnloadCollisionBg

/// @}

#endif // NF_COLLISION_H__

#ifdef __cplusplus
}
#endif
