// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Sound effect functions
// http://www.nightfoxandco.com/

#include <stdio.h>
#include <string.h>

#include <nds.h>

#include "nf_basic.h"
#include "nf_sound.h"

// Buffers of all sound effects
char *NF_BUFFER_RAWSOUND[NF_SLOTS_RAWSOUND];

// Information about all loaded sound effects
NF_TYPE_RAWSOUND_INFO NF_RAWSOUND[NF_SLOTS_RAWSOUND];

void NF_InitRawSoundBuffers(void)
{
    // Initialize buffers for RAW sound files
    for (int n = 0; n < NF_SLOTS_RAWSOUND; n++)
    {
        NF_BUFFER_RAWSOUND[n] = NULL;       // Pointer to the data
        NF_RAWSOUND[n].available = true;    // Availability of this slot
        NF_RAWSOUND[n].size = 0;            // File size
        NF_RAWSOUND[n].freq = 0;            // Frecuency of the sample
        NF_RAWSOUND[n].format = 0;          // Format of the sample
    }
}

void NF_ResetRawSoundBuffers(void)
{
    // Free all buffers of RAW sound files
    for (int n = 0; n < NF_SLOTS_RAWSOUND; n ++)
        free(NF_BUFFER_RAWSOUND[n]);

    // Reset data structures
    NF_InitRawSoundBuffers();
}

void NF_LoadRawSound(const char *file, u32 id, u32 freq, u32 format)
{
    // Verify that the ID is inside the valid range
    if (id >= NF_SLOTS_RAWSOUND)
        NF_Error(106, "Raw Sound", NF_SLOTS_RAWSOUND);

    // Verify that the ID is free
    if (!NF_RAWSOUND[id].available)
        NF_Error(109, "Raw Sound", id);

    // Free the buffer in case it already has data
    free(NF_BUFFER_RAWSOUND[id]);
    NF_BUFFER_RAWSOUND[id] = NULL;

    // File path
    char filename[256];

    // Load the .RAW file
    snprintf(filename, sizeof(filename), "%s/%s.raw", NF_ROOTFOLDER, file);
    NF_FileLoad(filename, &NF_BUFFER_RAWSOUND[id], &NF_RAWSOUND[id].size, 0);

    // If the size is over the limit
    if (NF_RAWSOUND[id].size > (256 * 1024))
        NF_Error(116, filename, 256 * 1024);

    // Save sound parameters
    NF_RAWSOUND[id].freq = freq;
    NF_RAWSOUND[id].format = format;

    // Mark this slot as used
    NF_RAWSOUND[id].available = false;
}

void NF_UnloadRawSound(u32 id)
{
    // Verify that the ID is inside the valid range
    if (id >= NF_SLOTS_RAWSOUND)
        NF_Error(106, "RAW Sound", NF_SLOTS_RAWSOUND);

    // Verify that the sound exists
    if (NF_RAWSOUND[id].available)
        NF_Error(110, "RAW Sound", id);

    // Free the data of this slot
    free(NF_BUFFER_RAWSOUND[id]);
    NF_BUFFER_RAWSOUND[id] = NULL;

    // Reset variables
    NF_RAWSOUND[id].freq = 0;
    NF_RAWSOUND[id].format = 0;

    // Mark this slot as free
    NF_RAWSOUND[id].available = true;
}

u32 NF_PlayRawSound(u32 id, u32 volume, u32 pan, bool loop, u32 loopfrom)
{
    // Verify that the ID is inside the valid range
    if (id >= NF_SLOTS_RAWSOUND)
        NF_Error(106, "RAW Sound", NF_SLOTS_RAWSOUND);

    // Verify that the sound exists
    if (NF_RAWSOUND[id].available)
        NF_Error(110, "RAW Sound", id);

    return soundPlaySample(NF_BUFFER_RAWSOUND[id], NF_RAWSOUND[id].format,
                           NF_RAWSOUND[id].size, NF_RAWSOUND[id].freq,
                           volume, pan, loop, loopfrom);
}
