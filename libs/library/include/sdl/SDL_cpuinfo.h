/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/

/**
 *  @file SDL_cpuinfo.h
 *  CPU feature detection for SDL
 */

#ifndef _SDL_cpuinfo_h
#define _SDL_cpuinfo_h

#include "SDL_stdinc.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/** This function returns true if the CPU has the RDTSC instruction */
static inline SDL_bool SDLCALL SDL_HasRDTSC(void) { return SDL_FALSE; }

/** This function returns true if the CPU has MMX features */
static inline SDL_bool SDLCALL SDL_HasMMX(void) { return SDL_FALSE; }

/** This function returns true if the CPU has MMX Ext. features */
static inline SDL_bool SDLCALL SDL_HasMMXExt(void) { return SDL_FALSE; }

/** This function returns true if the CPU has 3DNow features */
static inline SDL_bool SDLCALL SDL_Has3DNow(void) { return SDL_FALSE; }

/** This function returns true if the CPU has 3DNow! Ext. features */
static inline SDL_bool SDLCALL SDL_Has3DNowExt(void) { return SDL_FALSE; }

/** This function returns true if the CPU has SSE features */
static inline SDL_bool SDLCALL SDL_HasSSE(void) { return SDL_FALSE; }

/** This function returns true if the CPU has SSE2 features */
static inline SDL_bool SDLCALL SDL_HasSSE2(void) { return SDL_FALSE; }

/** This function returns true if the CPU has AltiVec features */
static inline SDL_bool SDLCALL SDL_HasAltiVec(void) { return SDL_FALSE; }

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _SDL_cpuinfo_h */
