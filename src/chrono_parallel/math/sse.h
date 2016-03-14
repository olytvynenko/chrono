// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2016 projectchrono.org
// All right reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Hammad Mazhar
// =============================================================================
//
// Description: definition of a real number which can be defined as a float
// (increased speed on some architectures) or a double (increased precision)
// =============================================================================

//#pragma once

#include "chrono_parallel/ChApiParallel.h"
#include "chrono_parallel/ChConfigParallel.h"
#include "chrono_parallel/ChCudaDefines.h"

//#ifdef __CUDACC__
//#warning using nvcc
//#ifdef __CUDA_ARCH__
//#warning device code trajectory
//#if __CUDA_ARCH__ > 120
//#warning compiling with double precision
//#else
//#warning compiling with single precision
//#endif
//#else
//#warning nvcc host code trajectory
//#endif
//#else
//#warning non-nvcc code trajectory
//#endif

#ifdef __CUDA_ARCH__
#define VECEXT sisd
#else
#define VECEXT simd
#endif

#ifndef __CUDA_ARCH__
// Check if SSE was found in CMake
#ifdef CHRONO_HAS_SSE
// Depending on the SSE variable in CMake include the proper header file for that
// version of sse
#ifdef CHRONO_SSE_1_0
#include <xmmintrin.h>
#elif defined CHRONO_SSE_2_0
#include <emmintrin.h>
#elif defined CHRONO_SSE_3_0
#include <pmmintrin.h>
#elif defined CHRONO_SSE_4_1
#include <smmintrin.h>
#elif defined CHRONO_SSE_4_2
#include <nmmintrin.h>
#endif
#endif

#if defined(CHRONO_HAS_AVX) && defined(CHRONO_PARALLEL_USE_DOUBLE)
#include <immintrin.h>
#endif

#if defined(CHRONO_USE_SIMD) && defined(CHRONO_HAS_AVX) && defined(CHRONO_PARALLEL_USE_DOUBLE)
#define USE_AVX
#undef USE_SSE
#elif defined(CHRONO_USE_SIMD) && defined(CHRONO_HAS_SSE) && !defined(CHRONO_PARALLEL_USE_DOUBLE)
#undef USE_AVX
#define USE_SSE
#else
#undef USE_AVX
#undef USE_SSE
#endif
#endif
