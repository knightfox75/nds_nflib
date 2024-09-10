// SPDX-License-Identifier: MIT
//
// Copyright (c) 2009-2014 Cesar Rincon "NightFox"
// Copyright (c) 2023 Antonio Niño Díaz "AntonioND"
//
// NightFox LIB - Global include
// http://www.nightfoxandco.com/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NF_LIB_H__
#define NF_LIB_H__

#include <nf_2d.h>
#include <nf_3d.h>
#include <nf_affinebg.h>
#include <nf_basic.h>
#include <nf_bitmapbg.h>
#include <nf_collision.h>
#include <nf_media.h>
#include <nf_mixedbg.h>
#include <nf_sound.h>
#include <nf_sprite256.h>
#include <nf_sprite3d.h>
#include <nf_text16.h>
#include <nf_text.h>
#include <nf_tiledbg.h>

/// Major version of NightFox's Lib
#define NF_LIB_MAJOR (1)
/// Minor version of NightFox's Lib
#define NF_LIB_MINOR (1)
/// Patch version of NightFox's Lib
#define NF_LIB_PATCH (5)

/// Full version of NightFox's Lib
#define NF_LIB_VERSION ((NF_LIB_MAJOR << 16) | (NF_LIB_MINOR << 8) | (NF_LIB_PATCH))

/// String with the version of NightFox's Lib
#define NF_LIB_VERSION_STRING "1.1.5"

#endif // NF_LIB_H__

#ifdef __cplusplus
}
#endif
