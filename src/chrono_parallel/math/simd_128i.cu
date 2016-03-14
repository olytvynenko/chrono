#include "chrono_parallel/math/sse.h"
#include "chrono_parallel/math/real3.h"
#include "chrono_parallel/math/real4.h"
#include "chrono_parallel/math/other_types.h"

#if defined(__CUDA_ARCH__)
#include "chrono_parallel/math/simd_non.h"
#elif defined(USE_SSE)
#include "chrono_parallel/math/simd_sse.h"
#elif defined(USE_AVX)
#include "chrono_parallel/math/simd_avx.h"
#endif

namespace chrono {

CUDA_HOST_DEVICE vec3 operator-(const vec3& a, const vec3& b) {
    return VECEXT::Sub(a, b);
}
CUDA_HOST_DEVICE vec3 operator-(const vec3& a, const int& b) {
    return VECEXT::Sub(a, VECEXT::Set(b));
}
CUDA_HOST_DEVICE vec3 operator+(const vec3& a, const vec3& b) {
    return VECEXT::Add(a, b);
}
CUDA_HOST_DEVICE vec3 operator+(const vec3& a, const int& b) {
    return VECEXT::Add(a, VECEXT::Set(b));
}
CUDA_HOST_DEVICE vec3 Clamp(const vec3& a, const vec3& clamp_min, const vec3& clamp_max) {
    return VECEXT::Max(clamp_min, VECEXT::Min(a, clamp_max));
}
}
