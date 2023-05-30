// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Sound effect functions
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_SOUND_H__
#define NF_SOUND_H__

#include <nds.h>

/// @file   nf_sound.h
/// @brief  Sound support functions.

/// @defgroup nf_sound Sound support functions.
///
/// Simple helpers to load and play raw PCM sound files.
///
/// @{

/// Number of slot availables for raw sound effects
#define NF_SLOTS_RAWSOUND 32

/// Buffers of all loaded sound files
extern char *NF_BUFFER_RAWSOUND[NF_SLOTS_RAWSOUND];

/// Struct that holds information about the loaded sound files
typedef struct {
    bool available;     ///< True if this slot is available
    size_t size;        ///< Size of the sound effect in bytes
    u16 freq;           ///< Frecuency of the sample
    u8 format;          ///< Format of the sample (0: 8 bits, 1: 16 bits, 2: ADPCM)
} NF_TYPE_RAWSOUND_INFO;

/// Information of all sound effects.
extern NF_TYPE_RAWSOUND_INFO NF_RAWSOUND[NF_SLOTS_RAWSOUND];

/// Initialize all buffers and variables to load sound files.
///
/// You must use this function once before loading or using any sound file.
/// Remember to initialize the DS sound engine using soundEnable().
void NF_InitRawSoundBuffers(void);

/// Resets all sound buffers and clears the data in them.
///
/// It's useful when you change a level in the game, for example.
void NF_ResetRawSoundBuffers(void);

/// Load a RAW file from the filesystem to RAM.
///
/// To convert sound files to "RAW" format you can use the free program "Switch"
/// http://www.nch.com.au/switch/plus.html. The best parameters for "RAW" files
/// on the DS are: 8 bits signed, mono, 11025 Hz or 22050 Hz.
///
/// Example:
/// ```
/// // Load "music.raw" to slot 1. This file is encoded as 8 bit at 22050 Hz
/// NF_LoadRawSound("music", 1, 22050, 0);
/// ```
///
/// @param file File name without extension
/// @param id Destination slot number (0 - 31)
/// @param freq Frequency of the sample in Hz (11025, 22050, etc)
/// @param format Sample format (0 -> 8 bits, 1 -> 16 bits, 2 -> ADPCM).
void NF_LoadRawSound(const char *file, u32 id, u32 freq, u32 format);

/// Deletes from RAM the sound file stored in the specified slot.
///
/// @param id Slot number (0 - 31)
void NF_UnloadRawSound(u32 id);

/// Play the sound file loaded in the specified slot.
///
/// If you want to loop the sound you must also set the sample number where the
/// loop starts.
///
/// This fuction also returns the channel number asigned to the playback.
///
/// Example:
/// ```
/// // Play the sound stored in slot 1 with full volume (127), centered (64),
/// // with looping enabled from the beginning.
/// NF_PlayRawSound(1, 127, 64, true, 0);
/// ```
///
/// You can use libnds functions to pause, stop and set the volume of the sound
/// while it plays.
///
/// @param id Slot number (0 - 31)
/// @param volume Volume (0 - 127)
/// @param pan Panning. 0: left, 64: center, 127: right.
/// @param loop True if you want the sound to loop.
/// @param loopfrom Loop starting point
/// @return Channel number that is playing the sound.
u32 NF_PlayRawSound(u32 id, u32 volume, u32 pan, bool loop, u32 loopfrom);

/// @}

#endif // NF_SOUND_H__

#ifdef __cplusplus
}
#endif
