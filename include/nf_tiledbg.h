// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
//
// NightFox LIB - Include de Fondos con tiles
// http://www.nightfoxandco.com/
// Version 20140413

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NF_TILEDBG_H__
#define __NF_TILEDBG_H__

#include <nds.h>





// Define los slots maximos para los fondos
#define NF_SLOTS_TBG 64			// Datos de los fondos
#define NF_SLOTS_EXBGPAL 128	// Paletas extendidas (maximo 16 paletas por fondo)

// Define el numero maximo de bancos para tiles y mapas
#define NF_MAX_BANKS_TILES 8
#define NF_MAX_BANKS_MAPS 16

// Define el numero de bancos de Mapas y Tiles
extern u8 NF_BANKS_TILES[2];	// (1 banks = 16kb)	Cada banco de tiles puede alvergar 8 bancos de Mapas
extern u8 NF_BANKS_MAPS[2];		// (1 bank = 2kb)	Usar multiplos de 8. Cada set de 8 bancos consume 1 banco de tiles

// Define los Buffers para almacenar los fondos
extern char* NF_BUFFER_BGTILES[NF_SLOTS_TBG];
extern char* NF_BUFFER_BGMAP[NF_SLOTS_TBG];
extern char* NF_BUFFER_BGPAL[NF_SLOTS_TBG];


// Define estructura para almacenar la info de los fondos
typedef struct {
	char name[32];		// Nombre del fondo
	u32 tilesize;		// Tamaño del Tileset
	u32 mapsize;		// Tamaño del Map
	u32 palsize;		// Tamaño de la Paleta
	u16 width;			// Ancho del fondo
	u16 height;			// Altura del fondo
	bool available;		// Disponibilidat del Slot
} NF_TYPE_TBG_INFO;
extern NF_TYPE_TBG_INFO NF_TILEDBG[NF_SLOTS_TBG];	// Datos de los fondos

// Define la estructura para almacenar la info y datos de las paletas extendidas
typedef struct {
	char* buffer;	// Buffer para almacenar la paleta
	u32 palsize;	// Tamaño de la paleta
	bool inuse;		// Slot libre o en uso
} NF_TYPE_EXBGPAL_INFO;
extern NF_TYPE_EXBGPAL_INFO NF_EXBGPAL[NF_SLOTS_EXBGPAL];	// Datos de las paletas extendidas

// Define estructura para almacenar la info de los fondos en pantalla
typedef struct {
	u8 tilebase;		// Bloque de inicio en VRAM del Tileset
	u8 tileblocks;		// Bloques usados por el Tileset
	u8 mapbase;			// Bloque de inicio en VRAM del Map
	u8 mapblocks;		// Bloques usados por el Map
	u16 bgwidth;		// Ancho del fondo
	u16 bgheight;		// Altura del fondo
	u16 mapwidth;		// Ancho del mapa
	u16 mapheight;		// Altura del mapa
	u8 bgtype;			// Tipo de mapa
	u8 bgslot;			// Buffer de graficos usado (NF_BUFFER_BGMAP)
	u8 blockx;			// Bloque de mapa (horizontal)
	u8 blocky;			// bloque de mapa (vertical)
	bool created;		// Flag de si esta creado
} NF_TYPE_TBGLAYERS_INFO;
// El hardware de la DS no permite mapas mayores de 512x512
// Asi que informaremos si nuestor mapa lo gestionara el hardware si es menor o 
// igual a 512x512, o usaremos nuestro motor de Tile Swaping
// bgtype 0: Normal (maximo 512 x 512)
// bgtype 1: >512 x 256
// bgtype 2: 256 x >512
// bgtype 3: >512 x >512
extern NF_TYPE_TBGLAYERS_INFO NF_TILEDBG_LAYERS[2][4];	//[screen][layer]



// Define el array de bloques libres
extern u8 NF_TILEBLOCKS[2][NF_MAX_BANKS_TILES];
extern u8 NF_MAPBLOCKS[2][NF_MAX_BANKS_MAPS];

/// Initialize library to load files from the filesystem to create tiled BGs.
///
/// Use this function once before loading any BG from FAT.
///
/// Example:
/// <pre>
/// // Init buffers and variables to load tiled BGs
/// NF_InitTiledBgBuffers();
/// </pre>
void NF_InitTiledBgBuffers(void);

/// Reset state used for tiled BGs loaded from FAT.
///
/// This function empties all buffers in use and resets variables to their
/// default values. It’s useful to do this when you change a level in a game,
/// to clean all stuff from RAM and make free space to load the new level.
///
/// Example:
/// <pre>
/// // Empty all buffers and reset variable values
/// NF_ResetTiledBgBuffers();
/// </pre>
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
/// <pre>
/// #define NF_BANKS_TILES 8
/// #define NF_BANKS_MAPS 16
/// </pre>
/// Each tile bank is as big as 8 map banks.
///
/// Example:
/// <pre>
/// // Init tiled BG system of screen 1
/// NF_InitTiledBgSys(1);
/// </pre>
///
/// @param screen Screen (0 - 1).
void NF_InitTiledBgSys(u8 screen);

/// Load all files needed to create a tiled BG from FAT to RAM.
///
/// All files for a BG must have the same name, using IMG extension for tiles
/// files, MAP for map files and PAL for palette files.
///
/// Check the GRIT folder for more info about BG files conversion.
///
/// You can load up to 32 BGs at the same time, this is defined in:
/// <pre>
/// #define NF_SLOTS_TBG 32
/// </pre>
///
/// Example:
/// <pre>
/// // Load to RAM files "mainstage.img", "mainstage.map" and "mainstage.pal"
/// // from the "stage1" subfolder and call it “mifondo”. Also store the size of
/// // the BG (2048 x 256).
/// NF_LoadTiledBg("stage1/mainstage", "mifondo", 2048, 256);
/// </pre>
///
/// @param file File path without extension.
/// @param name Name used for the BG for other functions.
/// @param width BG width.
/// @param height BG height.
void NF_LoadTiledBg(const char* file, const char* name, u16 width, u16 height);

/// Load a tilesed and palette from FAT to RAM.
///
/// It works like NF_LoadTiledBg() but it lets you specify the range of tiles to
/// load. Also, no actual map is loaded. Instead, a blank map of the given size
/// is created. The background is created using NF_CreateTiledBg().
///
/// Example:
/// <pre>
/// // Load to RAM tiles 0 to 23 (24 tiles in total) from "mainstage.img" and
/// // the palette (from "mainstage.pal") file, from "stage1" subfolder and
/// // assigns the "mifondo" name to the background. It also sets the size of
/// // background to 256x256 pixels. This creates a 32x32 tiles blank map.
/// NF_LoadTilesForBg("stage1/mainstage", "mifondo", 256, 256, 0, 23);
/// </pre>
///
/// @param file File name, without extension.
/// @param name Name of the BG.
/// @param width Width of the BG in pixels.
/// @param height Height of the BG in pixels.
/// @param tile_start First tile to load.
/// @param tile_end Last tile to load.
void NF_LoadTilesForBg(const char* file, const char* name, u16 width, u16 height, u16 tile_start, u16 tile_end);

/// Delete from RAM the BG with the specified name.
///
/// You can delete from RAM the BG if you don't need it more or if the size of
/// the BG size is less or equal than 512 x 512. If it's bigger, you must keep
/// it in RAM until you don't need it anymore.
///
/// Example:
/// <pre>
/// // Delete from RAM the BG with name "mifondo" and mark as free the slot it
/// // uses.
/// NF_UnloadTiledBg("mifondo");
/// </pre>
///
/// @param name Name used for the BG.
void NF_UnloadTiledBg(const char* name);

/// Create a BG on the screen, using data loaded in RAM.
///
/// This function copies to VRAM all required data. Before you create the BG,
/// you must load data to RAM using NF_LoadTiledBg(). The BG is created on the
/// specified screen and layer.
///
/// Example:
/// <pre>
/// // Create a tiled BG on layer 3 of screen 0, using the BG called "mifondo"
/// NF_CreateTiledBg(0, 3, "mifondo");
/// </pre>
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param name Name used for the BG.
void NF_CreateTiledBg(u8 screen, u8 layer, const char* name);

/// Delete the BG of the specified screen and layer.
///
/// This also deletes from VRAM the data used by this BG.
///
/// Example:
/// <pre>
/// // Deletes the tiled BG from layer 3 of screen 0
/// NF_DeleteTiledBg(0, 3);
/// </pre>
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
void NF_DeleteTiledBg(u8 screen, u8 layer);

/// Gets the address of the tile at the specified position.
///
/// Internal use.
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param tile_x X coordinate.
/// @param tile_y Y coordinate.
/// @return Tile address.
u32 NF_GetTileMapAddress(u8 screen, u8 layer, u16 tile_x, u16 tile_y);

/// Gets the value of the tile at the specified position.
///
/// Example:
/// <pre>
/// // Gets the value of the tile at (20, 10) of the map loaded on screen 0,
/// // layer 2.
/// u16 mytile = NF_GetTileOfMap(0, 2, 10, 20);
/// </pre>
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param tile_x X coordinate.
/// @param tile_y Y coordinate.
/// @return Tile index.
u16 NF_GetTileOfMap(u8 screen, u8 layer, u16 tile_x, u16 tile_y);

/// Sets the value of the tile at the specified position.
///
/// Example:
/// <pre>
/// // Sets to 5 the tile at (20, 10) of the map loaded on screen 0, layer 2.
/// NF_SetTileOfMap(0, 2, 10, 20, 5);
/// </pre>
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param tile_x X coordinate.
/// @param tile_y Y coordinate.
/// @param tile Tile index.
void NF_SetTileOfMap(u8 screen, u8 layer, u16 tile_x, u16 tile_y, u16 tile);

/// Updates the map of the specified screen and layer specified.
///
/// This updates the map on VRAM with the copy of RAM, that can be modified. Use
/// this fuction to apply changes made with NF_SetTileOfMap().
///
/// Example:
/// <pre>
/// // Update the map in VRAM with the modified copy in RAM of screen 0, layer 2
/// NF_UpdateVramMap(0, 2);
/// </pre>
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
void NF_UpdateVramMap(u8 screen, u8 layer);

/// Changes the value of one color of the palette of a background.
///
/// The change is made directly in VRAM, so it may be overwritten from the copy
/// in RAM. It's also a very slow function, use it twice or 3 times per frame.
///
/// Example:
/// <pre>
/// // Change the value of color 1 of the palette of layer 3 on top screen to
/// // red. If this layer is a text layer with the default font, the text
/// // becomes red.
/// NF_BgSetPalColor(0, 3, 1, 31, 0, 0);
/// </pre>
///
/// @param screen Screen (0 - 1).
/// @param layer Layer (0 - 3).
/// @param number Color number (0 - 255).
/// @param r Red component (0 - 31).
/// @param g Green component (0 - 31).
/// @param b Blue component (0 - 31).
void NF_BgSetPalColor(u8 screen, u8 layer, u8 number, u8 r, u8 g, u8 b);



// Funcion NF_BgEditPalColor();
void NF_BgEditPalColor(u8 screen, u8 layer, u8 number, u8 r, u8 g, u8 b);
// Edita un color de la paleta seleccionada.
// El color se edita en el buffer de RAM, para que sea efectivo,
// mandala a la VRAM con NF_UpdatePalette();





// Funcion 	NF_BgUpdatePalette();
void NF_BgUpdatePalette(u8 screen, u8 layer);
// Actualiza la paleta en VRAM con la que se encuentra en el buffer de RAM





// Funcion NF_BgGetPalColor();
void NF_BgGetPalColor(u8 screen, u8 layer, u8 number, u8* r, u8* g, u8* b);
// Obtiene el valor de un color de la paleta que se encuentra en la RAM





// Funcion NF_GetTilePal();
u8 NF_GetTilePal(u8 screen, u8 layer, u16 tile_x, u16 tile_y);
// Devuelve que numero de paleta (0 - 15) esta usando el tile del fondo especificado.
// Por defecto, todos los tiles usan la paleta del Slot nº0
// Los datos se obtienen de la compia en RAM del mapa del fondo.





// Funcion NF_SetTilePal();
void NF_SetTilePal(u8 screen, u8 layer, u16 tile_x, u16 tile_y, u8 pal);
// Cambia el numero de paleta (0 - 15) que usara el tile del fondo especificado.
// Por defecto, todos los tiles usan la paleta del Slot nº0
// Los datos se escriben de la compia en RAM del mapa del fondo, por lo que no seran
// visibles hasta que ejecutes la funcion NF_UpdateVramMap();





// Funcion NF_LoadExBgPal();
void NF_LoadExBgPal(const char* file, u8 slot);
// Carga en el buffer de RAM correspondiente una paleta de fondos, para poderla usar
// mas tarde como paleta extendida.





// Funcion NF_UnloadExBgPal();
void NF_UnloadExBgPal(u8 slot);
// Borra de la RAM la paleta del slot especificado.





// Funcion NF_VramExBgPal();
void NF_VramExBgPal(u8 screen, u8 layer, u8 id, u8 slot);
// Transfiere a la VRAM una paleta extendida en el slot de la pantalla y
// fondo especificados.





// Funcion NF_SetExBgPal();
void NF_SetExBgPal(u8 screen, u8 layer, u8 pal);
// Cambia la paleta extendida que usara un fondo.
// La paleta debe de estar transferida en la VRAM previamente





// Funcion NF_SetTileHflip();
void NF_SetTileHflip(u8 screen, u8 layer, u16 tile_x, u16 tile_y);
// Invierte horizontalmente el estado actual del tile seleccionado
// Los cambios no seran visibles hasta que actualices el mapa
// con la funcion NF_UpdateVramMap();





// Funcion NF_SetTileVflip();
void NF_SetTileVflip(u8 screen, u8 layer, u16 tile_x, u16 tile_y);
// Invierte verticalmente el estado actual del tile seleccionado
// Los cambios no seran visibles hasta que actualices el mapa
// con la funcion NF_UpdateVramMap();




// Funcion NF_RotateTileGfx();
void NF_RotateTileGfx(u8 slot, u16 tile, u8 rotation);
// Rota el grafico de un tile especificado. Rota el tile almacenado en un buffer de fondos
// que se encuentra en RAM. Debes especificar el SLOT del buffer, numero de tile en el buffer
// y el angulo de la rotacion.
// 1 - 90º a la derecha
// 2 - 90º a la izquierda
// 3 - 180º





#endif

#ifdef __cplusplus
}
#endif
