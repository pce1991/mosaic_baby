
#ifndef _ETC_MATH_NOISE_H
#define _ETC_MATH_NOISE_H

static int hash[] = {
    151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
    140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
    247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
    57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
    74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
    60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
    65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
    200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
    52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
    207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
    119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
    129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
    218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
    81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
    184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
    222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180,

    // Repetition to allow indexing from 256..511
    151,160,137, 91, 90, 15,131, 13,201, 95, 96, 53,194,233,  7,225,
    140, 36,103, 30, 69,142,  8, 99, 37,240, 21, 10, 23,190,  6,148,
    247,120,234, 75,  0, 26,197, 62, 94,252,219,203,117, 35, 11, 32,
    57,177, 33, 88,237,149, 56, 87,174, 20,125,136,171,168, 68,175,
    74,165, 71,134,139, 48, 27,166, 77,146,158,231, 83,111,229,122,
    60,211,133,230,220,105, 92, 41, 55, 46,245, 40,244,102,143, 54,
    65, 25, 63,161,  1,216, 80, 73,209, 76,132,187,208, 89, 18,169,
    200,196,135,130,116,188,159, 86,164,100,109,198,173,186,  3, 64,
    52,217,226,250,124,123,  5,202, 38,147,118,126,255, 82, 85,212,
    207,206, 59,227, 47, 16, 58, 17,182,189, 28, 42,223,183,170,213,
    119,248,152,  2, 44,154,163, 70,221,153,101,155,167, 43,172,  9,
    129, 22, 39,253, 19, 98,108,110, 79,113,224,232,178,185,112,104,
    218,246, 97,228,251, 34,242,193,238,210,144, 12,191,179,162,241,
    81, 51,145,235,249, 14,239,107, 49,192,214, 31,181,199,106,157,
    184, 84,204,176,115,121, 50, 45,127,  4,150,254,138,236,205, 93,
    222,114, 67, 29, 24, 72,243,141,128,195, 78, 66,215, 61,156,180
};

#define HASHMASK 255
#define INV_HASHMASK (1.0f / HASHMASK)

static real32 Smooth(real32 t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static real32 SmoothDerivative(real32 t) {
    return 30.0f * t * t * (t * (t - 2.0f) + 1.0f);
}

inline real32 Value1f(real32 sample, real32 frequency) {
    sample = Modf(frequency * sample, HASHMASK + 1);

    int32 i0 = (int32)floor(sample);
    i0 = i0 & HASHMASK;
    int32 i1 = i0 + 1;

    int32 h0 = hash[i0];
    int32 h1 = hash[i1];

    real32 t = Smooth(sample - i0);

    return Lerp(h0, h1, t) * INV_HASHMASK;
}

inline vec2 Value1DDerivative(real32 sample, real32 frequency) {
    sample = Modf(frequency * sample, HASHMASK + 1);

    int32 i0 = (int32)floor(sample);
    i0 = i0 & HASHMASK;
    int32 i1 = i0 + 1;

    int32 h0 = hash[i0];
    int32 h1 = hash[i1];

    real32 a = h0;
    real32 b = h1 - h0;

    real32 t = Smooth(sample - i0);
    real32 dt = SmoothDerivative(sample - i0);

    vec2 result = V2(0, 0);
    result.y = a + b * t;
    result.x = b * dt;

    result.x *= frequency;

    return result * INV_HASHMASK;
}

inline real32 Value2f(vec2 sample, real32 frequency) {
    sample = V2(Modf(frequency * sample.x, HASHMASK + 1),
                Modf(frequency * sample.y, HASHMASK + 1));

    int32 ix0 = (int32)floor(sample.x);
    int32 iy0 = (int32)floor(sample.y);
    ix0 = ix0 & HASHMASK;
    iy0 = iy0 & HASHMASK;

    int32 ix1 = ix0 + 1;
    int32 iy1 = iy0 + 1;

    int32 h0 = hash[ix0];
    int32 h1 = hash[ix1];
    int32 h00 = hash[h0 + iy0];
    int32 h10 = hash[h1 + iy0];
    int32 h01 = hash[h0 + iy1];
    int32 h11 = hash[h1 + iy1];

    real32 tx = Smooth(sample.x - ix0);
    real32 ty = Smooth(sample.y - iy0);
        
    return Lerp(Lerp(h00, h10, tx),
                      Lerp(h01, h11, tx),
                      ty) * INV_HASHMASK;
}

inline vec3 Value2DDerivative(vec2 sample, real32 frequency) {
    sample = V2(Modf(frequency * sample.x, HASHMASK + 1),
                Modf(frequency * sample.y, HASHMASK + 1));

    int32 ix0 = (int32)floor(sample.x);
    int32 iy0 = (int32)floor(sample.y);
    ix0 = ix0 & HASHMASK;
    iy0 = iy0 & HASHMASK;

    int32 ix1 = ix0 + 1;
    int32 iy1 = iy0 + 1;

    int32 h0 = hash[ix0];
    int32 h1 = hash[ix1];
    int32 h00 = hash[h0 + iy0];
    int32 h10 = hash[h1 + iy0];
    int32 h01 = hash[h0 + iy1];
    int32 h11 = hash[h1 + iy1];

    real32 a = h00;
    real32 b = h10 - h00;
    real32 c = h01 - h00;
    real32 d = h11 - h01 - h10 + h00;

    real32 tx = Smooth(sample.x - ix0);
    real32 ty = Smooth(sample.y - iy0);
    real32 dtx = SmoothDerivative(sample.x - ix0);
    real32 dty = SmoothDerivative(sample.y - iy0);
        
    vec3 result = V3(0, 0, 0);
    result.z = a + b * tx + (c + d * tx) * ty;
    result.x = (b + d * ty) * dtx;
    result.y = (c + d * tx) * dty;

    result.x *= frequency;
    result.y *= frequency;

    return result * INV_HASHMASK;
}

inline real32 Value3f(vec3 sample, real32 frequency) {
    sample = V3(Modf(frequency * sample.x, HASHMASK + 1),
                Modf(frequency * sample.y, HASHMASK + 1),
                Modf(frequency * sample.z, HASHMASK + 1));

    int32 ix0 = (int32)floor(sample.x);
    int32 iy0 = (int32)floor(sample.y);
    int32 iz0 = (int32)floor(sample.z);
    ix0 = ix0 & HASHMASK;
    iy0 = iy0 & HASHMASK;
    iz0 = iz0 & HASHMASK;

    int32 ix1 = ix0 + 1;
    int32 iy1 = iy0 + 1;
    int32 iz1 = iz0 + 1;

    int32 h0 = hash[ix0];
    int32 h1 = hash[ix1];
    int32 h00 = hash[h0 + iy0];
    int32 h10 = hash[h1 + iy0];
    int32 h01 = hash[h0 + iy1];
    int32 h11 = hash[h1 + iy1];
    int32 h000 = hash[h00 + iz0];
    int32 h100 = hash[h10 + iz0];
    int32 h010 = hash[h01 + iz0];
    int32 h110 = hash[h11 + iz0];
    int32 h001 = hash[h00 + iz1];
    int32 h101 = hash[h10 + iz1];
    int32 h011 = hash[h01 + iz1];
    int32 h111 = hash[h11 + iz1];

    real32 tx = Smooth(sample.x - ix0);
    real32 ty = Smooth(sample.y - iy0);
    real32 tz = Smooth(sample.z - iz0);

    return Lerp(Lerp(Lerp(h000, h100, tx), Lerp(h010, h110, tx), ty),
                      Lerp(Lerp(h001, h101, tx), Lerp(h011, h111, tx), ty),
                      tz) * INV_HASHMASK;
        
}

inline vec4 Value3DDerivative(vec3 sample, real32 frequency) {
    sample = V3(Modf(frequency * sample.x, HASHMASK + 1),
                Modf(frequency * sample.y, HASHMASK + 1),
                Modf(frequency * sample.z, HASHMASK + 1));

    int32 ix0 = (int32)floor(sample.x);
    int32 iy0 = (int32)floor(sample.y);
    int32 iz0 = (int32)floor(sample.z);
    ix0 = ix0 & HASHMASK;
    iy0 = iy0 & HASHMASK;
    iz0 = iz0 & HASHMASK;

    int32 ix1 = ix0 + 1;
    int32 iy1 = iy0 + 1;
    int32 iz1 = iz0 + 1;

    int32 h0 = hash[ix0];
    int32 h1 = hash[ix1];
    int32 h00 = hash[h0 + iy0];
    int32 h10 = hash[h1 + iy0];
    int32 h01 = hash[h0 + iy1];
    int32 h11 = hash[h1 + iy1];
    int32 h000 = hash[h00 + iz0];
    int32 h100 = hash[h10 + iz0];
    int32 h010 = hash[h01 + iz0];
    int32 h110 = hash[h11 + iz0];
    int32 h001 = hash[h00 + iz1];
    int32 h101 = hash[h10 + iz1];
    int32 h011 = hash[h01 + iz1];
    int32 h111 = hash[h11 + iz1];

    real32 a = h000;
    real32 b = h100 - h000;
    real32 c = h010 - h000;
    real32 d = h001 - h000;
    real32 e = h110 - h010 - h100 + h000;
    real32 f = h101 - h001 - h100 + h000;
    real32 g = h011 - h001 - h010 + h000;
    real32 h = h111 - h011 - h101 + h001 - h110 + h010 + h100 - h000;

    real32 tx = Smooth(sample.x - ix0);
    real32 ty = Smooth(sample.y - iy0);
    real32 tz = Smooth(sample.z - iz0);
    real32 dtx = SmoothDerivative(sample.x - ix0);
    real32 dty = SmoothDerivative(sample.y - iy0);
    real32 dtz = SmoothDerivative(sample.z - iz0);
        
    vec4 result = V4(0, 0, 0, 0);
    result.w = a + b * tx + (c + e * tx) * ty + (d + f * tx + (g + h * tx) * ty) * tz;
    result.x = (b + e * ty + (f + h * ty) * tz) * dtx;
    result.y = (c + e * tx + (g + h * tx) * tz) * dty;
    result.z = (d + f * tx + (g + h * tx) * ty) * dtz;

    result.x *= frequency;
    result.y *= frequency;
    result.z *= frequency;

    return result;
}

static real32 gradients1D[] = {
    1.0f, -1.0f,
};

static vec2 gradients2D[] = {
    V2( 1.0f, 0.0f),
    V2(-1.0f, 0.0f),
    V2( 0.0f, 1.0f),
    V2( 0.0f,-1.0f),
    Normalize(V2( 1.0f, 1.0f)),
    Normalize(V2(-1.0f, 1.0f)),
    Normalize(V2( 1.0f,-1.0f)),
    Normalize(V2(-1.0f,-1.0f)),
};


static vec3 gradients3D[] = {
    V3( 1.0f, 1.0f, 0.0f),
    V3(-1.0f, 1.0f, 0.0f),
    V3( 1.0f,-1.0f, 0.0f),
    V3(-1.0f,-1.0f, 0.0f),

    V3( 1.0f, 0.0f, 1.0f),
    V3(-1.0f, 0.0f, 1.0f),
    V3( 1.0f, 0.0f,-1.0f),
    V3(-1.0f, 0.0f,-1.0f),

    V3( 0.0f, 1.0f, 1.0f),
    V3( 0.0f,-1.0f, 1.0f),
    V3( 0.0f, 1.0f,-1.0f),
    V3( 0.0f,-1.0f,-1.0f),

    V3( 1.0f, 1.0f, 0.0f),
    V3(-1.0f, 1.0f, 0.0f),
    V3( 0.0f,-1.0f, 1.0f),
    V3( 0.0f,-1.0f,-1.0f),
};

static vec4 gradients4D[] = {
    Normalize(V4( 1.0f, 1.0f, 0.0f, 1.0f)),
    Normalize(V4(-1.0f, 0.0f, 1.0f,-1.0f)),
    Normalize(V4( 1.0f, 0.0f, 0.0f,-1.0f)),
    Normalize(V4( 0.0f, 1.0f, 1.0f, 0.0f)),
    Normalize(V4(-1.0f,-1.0f, 0.0f, 1.0f)),
    Normalize(V4( 0.0f, 1.0f, 1.0f,-1.0f)),

    Normalize(V4(-1.0f,-1.0f, 1.0f, 0.0f)),
    Normalize(V4( 1.0f,-1.0f,-1.0f,-1.0f)),
    Normalize(V4(-1.0f, 1.0f, 0.0f, 1.0f)),
    Normalize(V4( 0.0f,-1.0f,-1.0f, 1.0f)),
    Normalize(V4( 1.0f, 0.0f, 1.0f,-1.0f)),
    Normalize(V4(-1.0f, 1.0f,-1.0f, 1.0f)),

    Normalize(V4( 0.0f, 1.0f,-1.0f, 0.0f)),
    Normalize(V4(-1.0f, 0.0f, 0.0f,-1.0f)),
    Normalize(V4( 1.0f, 1.0f, 1.0f, 1.0f)),
    Normalize(V4( 0.0f,-1.0f, 0.0f,-1.0f)),
};
        
#define GRADIENTSMASK_1D 1
#define GRADIENTSMASK_2D 7
#define GRADIENTSMASK_3D 15
#define GRADIENTSMASK_4D 15


inline real32 Perlin1f(real32 sample, real32 frequency) {
    sample = sample * frequency;

    int32 i0 = (int32)Floorf(sample) & HASHMASK;
    int32 i1 = i0 + 1;

    real32 g0 = gradients1D[hash[i0] & GRADIENTSMASK_1D];
    real32 g1 = gradients1D[hash[i1] & GRADIENTSMASK_1D];

    real32 tx0 = sample - Floorf(sample);
    real32 tx1 = tx0 - 1;

    real32 v0 = g0 * tx0;
    real32 v1 = g1 * tx1;

    real32 tx = Smooth(tx0);
    return Lerp(v0, v1, tx) * 2.0f;
}

inline real32 Perlin1DDerivative(real32 sample, real32 frequency, real32 *value) {
    sample = sample * frequency;

    int32 i0 = (int32)Floorf(sample) & HASHMASK;
    int32 i1 = i0 + 1;

    real32 g0 = gradients1D[hash[i0] & GRADIENTSMASK_1D];
    real32 g1 = gradients1D[hash[i1] & GRADIENTSMASK_1D];

    real32 tx0 = sample - Floorf(sample);
    real32 tx1 = tx0 - 1;

    real32 v0 = g0 * tx0;
    real32 v1 = g1 * tx1;

    real32 a = v0;
    real32 b = v1 - v0;

    real32 da = g0;
    real32 db = g1 - g0;

    real32 tx = Smooth(tx0);
    real32 dtx = SmoothDerivative(tx0);

    real32 derivative = (da + db * tx + b * dtx) * frequency * 2.0f;

    if (value != 0) {
        *value = (a + b * tx) * 2.0f;
    }

    return derivative;
}

inline real32 Perlin2f(vec2 sample, real32 frequency) {
    sample = sample * frequency;

    int32 ix0 = (int32)Floorf(sample.x) & HASHMASK;
    int32 iy0 = (int32)Floorf(sample.y) & HASHMASK;
    int32 ix1 = ix0 + 1;
    int32 iy1 = iy0 + 1;
                
    int32 h0 = hash[ix0];
    int32 h1 = hash[ix1];
    vec2 g00 = gradients2D[hash[h0 + iy0] & GRADIENTSMASK_2D];
    vec2 g10 = gradients2D[hash[h1 + iy0] & GRADIENTSMASK_2D];
    vec2 g01 = gradients2D[hash[h0 + iy1] & GRADIENTSMASK_2D];
    vec2 g11 = gradients2D[hash[h1 + iy1] & GRADIENTSMASK_2D];

    real32 tx0 = sample.x - Floorf(sample.x);
    real32 ty0 = sample.y - Floorf(sample.y);
    real32 tx1 = tx0 - 1.0f;
    real32 ty1 = ty0 - 1.0f;

    real32 v00 = Dot(g00, V2(tx0, ty0));
    real32 v10 = Dot(g10, V2(tx1, ty0));
    real32 v01 = Dot(g01, V2(tx0, ty1));
    real32 v11 = Dot(g11, V2(tx1, ty1));
                
    real32 tx = Smooth(tx0);
    real32 ty = Smooth(ty0);
    return Lerp(Lerp(v00, v10, tx),
                      Lerp(v01, v11, tx),
                      ty) * SQRT2;
}

inline vec2 Perlin2DDerivative(vec2 sample, real32 frequency, real32 *value = NULL) {
    sample = sample * frequency;

    int32 ix0 = (int32)Floorf(sample.x) & HASHMASK;
    int32 iy0 = (int32)Floorf(sample.y) & HASHMASK;
    int32 ix1 = ix0 + 1;
    int32 iy1 = iy0 + 1;
                
    int32 h0 = hash[ix0];
    int32 h1 = hash[ix1];

    vec2 g00 = gradients2D[hash[h0 + iy0] & GRADIENTSMASK_2D];
    vec2 g10 = gradients2D[hash[h1 + iy0] & GRADIENTSMASK_2D];
    vec2 g01 = gradients2D[hash[h0 + iy1] & GRADIENTSMASK_2D];
    vec2 g11 = gradients2D[hash[h1 + iy1] & GRADIENTSMASK_2D];

    real32 tx0 = sample.x - Floorf(sample.x);
    real32 ty0 = sample.y - Floorf(sample.y);
    real32 tx1 = tx0 - 1.0f;
    real32 ty1 = ty0 - 1.0f;

    real32 v00 = Dot(g00, V2(tx0, ty0));
    real32 v10 = Dot(g10, V2(tx1, ty0));
    real32 v01 = Dot(g01, V2(tx0, ty1));
    real32 v11 = Dot(g11, V2(tx1, ty1));

    real32 a = v00;
    real32 b = v10 - v00;
    real32 c = v01 - v00;
    real32 d = v11 - v01 - v10 + v00;

    vec2 da = g00;
    vec2 db = g10 - g00;
    vec2 dc = g01 - g00;
    vec2 dd = g11 - g01 - g10 + g00;
                
    real32 tx = Smooth(tx0);
    real32 ty = Smooth(ty0);
    real32 dtx = SmoothDerivative(tx0);
    real32 dty = SmoothDerivative(ty0);

    vec2 derivative = da + db * tx + (dc + dd * tx) * ty;
    derivative.x += (b + d * ty) * dtx;
    derivative.y += (c + d * tx) * dty;
    derivative = derivative * frequency * SQRT2;

    if (value != NULL) {
        *value = (a + b * tx + (c + d * tx) * ty) * SQRT2;
    }

    return derivative;
}

inline real32 Perlin3f(vec3 sample, real32 frequency) {
    sample = sample * frequency;

    int32 ix0 = (int32)Floorf(sample.x) & HASHMASK;
    int32 iy0 = (int32)Floorf(sample.y) & HASHMASK;
    int32 iz0 = (int32)Floorf(sample.z) & HASHMASK;
    int32 ix1 = ix0 + 1;
    int32 iy1 = iy0 + 1;
    int32 iz1 = iz0 + 1;
                
    int32 h0 = hash[ix0];
    int32 h1 = hash[ix1];
    int32 h00 = hash[h0 + iy0];
    int32 h10 = hash[h1 + iy0];
    int32 h01 = hash[h0 + iy1];
    int32 h11 = hash[h1 + iy1];

    vec3 g000 = gradients3D[hash[h00 + iz0] & GRADIENTSMASK_3D];
    vec3 g100 = gradients3D[hash[h10 + iz0] & GRADIENTSMASK_3D];
    vec3 g010 = gradients3D[hash[h01 + iz0] & GRADIENTSMASK_3D];
    vec3 g110 = gradients3D[hash[h11 + iz0] & GRADIENTSMASK_3D];
    vec3 g001 = gradients3D[hash[h00 + iz1] & GRADIENTSMASK_3D];
    vec3 g101 = gradients3D[hash[h10 + iz1] & GRADIENTSMASK_3D];
    vec3 g011 = gradients3D[hash[h01 + iz1] & GRADIENTSMASK_3D];
    vec3 g111 = gradients3D[hash[h11 + iz1] & GRADIENTSMASK_3D];

    real32 tx0 = sample.x - Floorf(sample.x);
    real32 ty0 = sample.y - Floorf(sample.y);
    real32 tz0 = sample.z - Floorf(sample.z);
    real32 tx1 = tx0 - 1.0f;
    real32 ty1 = ty0 - 1.0f;
    real32 tz1 = tz0 - 1.0f;

    real32 v000 = Dot(g000, V3(tx0, ty0, tz0));
    real32 v100 = Dot(g100, V3(tx1, ty0, tz0));
    real32 v010 = Dot(g010, V3(tx0, ty1, tz0));
    real32 v110 = Dot(g110, V3(tx1, ty1, tz0));
    real32 v001 = Dot(g001, V3(tx0, ty0, tz1));
    real32 v101 = Dot(g101, V3(tx1, ty0, tz1));
    real32 v011 = Dot(g011, V3(tx0, ty1, tz1));
    real32 v111 = Dot(g111, V3(tx1, ty1, tz1));

    real32 tx = Smooth(tx0);
    real32 ty = Smooth(ty0);
    real32 tz = Smooth(tz0);

    return Lerp(Lerp(Lerp(v000, v100, tx),
                     Lerp(v010, v110, tx),
                     ty),
                Lerp(Lerp(v001, v101, tx),
                     Lerp(v011, v111, tx),
                     ty),
                tz);
}

inline vec3 Perlin3DDerivative(vec3 sample, real32 frequency, real32 *value = NULL) {
    sample = sample * frequency;

    int32 ix0 = (int32)Floorf(sample.x) & HASHMASK;
    int32 iy0 = (int32)Floorf(sample.y) & HASHMASK;
    int32 iz0 = (int32)Floorf(sample.z) & HASHMASK;
    int32 ix1 = ix0 + 1;
    int32 iy1 = iy0 + 1;
    int32 iz1 = iz0 + 1;
                
    int32 h0 = hash[ix0];
    int32 h1 = hash[ix1];
    int32 h00 = hash[h0 + iy0];
    int32 h10 = hash[h1 + iy0];
    int32 h01 = hash[h0 + iy1];
    int32 h11 = hash[h1 + iy1];

    vec3 g000 = gradients3D[hash[h00 + iz0] & GRADIENTSMASK_3D];
    vec3 g100 = gradients3D[hash[h10 + iz0] & GRADIENTSMASK_3D];
    vec3 g010 = gradients3D[hash[h01 + iz0] & GRADIENTSMASK_3D];
    vec3 g110 = gradients3D[hash[h11 + iz0] & GRADIENTSMASK_3D];
    vec3 g001 = gradients3D[hash[h00 + iz1] & GRADIENTSMASK_3D];
    vec3 g101 = gradients3D[hash[h10 + iz1] & GRADIENTSMASK_3D];
    vec3 g011 = gradients3D[hash[h01 + iz1] & GRADIENTSMASK_3D];
    vec3 g111 = gradients3D[hash[h11 + iz1] & GRADIENTSMASK_3D];

    real32 tx0 = sample.x - Floorf(sample.x);
    real32 ty0 = sample.y - Floorf(sample.y);
    real32 tz0 = sample.z - Floorf(sample.z);
    real32 tx1 = tx0 - 1.0f;
    real32 ty1 = ty0 - 1.0f;
    real32 tz1 = tz0 - 1.0f;

    real32 v000 = Dot(g000, V3(tx0, ty0, tz0));
    real32 v100 = Dot(g100, V3(tx1, ty0, tz0));
    real32 v010 = Dot(g010, V3(tx0, ty1, tz0));
    real32 v110 = Dot(g110, V3(tx1, ty1, tz0));
    real32 v001 = Dot(g001, V3(tx0, ty0, tz1));
    real32 v101 = Dot(g101, V3(tx1, ty0, tz1));
    real32 v011 = Dot(g011, V3(tx0, ty1, tz1));
    real32 v111 = Dot(g111, V3(tx1, ty1, tz1));

    real32 a = v000;
    real32 b = v100 - v000;
    real32 c = v010 - v000;
    real32 d = v001 - v000;
    real32 e = v110 - v010 - v100 + v000;
    real32 f = v101 - v001 - v100 + v000;
    real32 g = v011 - v001 - v010 + v000;
    real32 h = v111 - v011 - v101 + v001 - v110 + v010 + v100 - v000;

    vec3 da = g000;
    vec3 db = g100 - g000;
    vec3 dc = g010 - g000;
    vec3 dd = g001 - g000;
    vec3 de = g110 - g010 - g100 + g000;
    vec3 df = g101 - g001 - g100 + g000;
    vec3 dg = g011 - g001 - g010 + g000;
    vec3 dh = g111 - g011 - g101 + g001 - g110 + g010 + g100 - g000;

    real32 tx = Smooth(tx0);
    real32 ty = Smooth(ty0);
    real32 tz = Smooth(tz0);
    real32 dtx = SmoothDerivative(tx0);
    real32 dty = SmoothDerivative(ty0);
    real32 dtz = SmoothDerivative(tz0);

    vec3 derivative = da + db * tx + (dc + de * tx) * ty + (dd + df * tx + (dg + dh * tx) * ty) * tz;
    derivative.x += (b + e * ty + (f + h * ty) * tz) * dtx;
    derivative.y += (c + e * tx + (g + h * tx) * tz) * dty;
    derivative.z += (d + f * tx + (g + h * tx) * ty) * dtz;
    derivative = derivative * frequency;

    if (value != NULL) {
        *value = a + b * tx + (c + e * tx) * ty + (d + f * tx + (g + h * tx) * ty) * tz;
    }

    return derivative;
}

inline real32 Perlin4f(vec4 sample, real32 frequency) {
    sample = sample * frequency;

    int32 ix0 = (int32)Floorf(sample.x) & HASHMASK;
    int32 iy0 = (int32)Floorf(sample.y) & HASHMASK;
    int32 iz0 = (int32)Floorf(sample.z) & HASHMASK;
    int32 iw0 = (int32)Floorf(sample.w) & HASHMASK;
    int32 ix1 = ix0 + 1;
    int32 iy1 = iy0 + 1;
    int32 iz1 = iz0 + 1;
    int32 iw1 = iw0 + 1;
                
    int32 h0 = hash[ix0];
    int32 h1 = hash[ix1];
    int32 h00 = hash[h0 + iy0];
    int32 h10 = hash[h1 + iy0];
    int32 h01 = hash[h0 + iy1];
    int32 h11 = hash[h1 + iy1];
    int32 h000 = hash[h00 + iz0] & HASHMASK;
    int32 h100 = hash[h10 + iz0] & HASHMASK;
    int32 h010 = hash[h01 + iz0] & HASHMASK;
    int32 h110 = hash[h11 + iz0] & HASHMASK;
    int32 h001 = hash[h00 + iz1] & HASHMASK;
    int32 h101 = hash[h10 + iz1] & HASHMASK;
    int32 h011 = hash[h01 + iz1] & HASHMASK;
    int32 h111 = hash[h11 + iz1] & HASHMASK;

    vec4 g0000 = gradients4D[hash[h000 + iw0] & GRADIENTSMASK_4D];
    vec4 g1000 = gradients4D[hash[h100 + iw0] & GRADIENTSMASK_4D];
    vec4 g0100 = gradients4D[hash[h010 + iw0] & GRADIENTSMASK_4D];
    vec4 g1100 = gradients4D[hash[h110 + iw0] & GRADIENTSMASK_4D];
    vec4 g0010 = gradients4D[hash[h001 + iw0] & GRADIENTSMASK_4D];
    vec4 g1010 = gradients4D[hash[h101 + iw0] & GRADIENTSMASK_4D];
    vec4 g0110 = gradients4D[hash[h011 + iw0] & GRADIENTSMASK_4D];
    vec4 g1110 = gradients4D[hash[h111 + iw0] & GRADIENTSMASK_4D];
    vec4 g0001 = gradients4D[hash[h000 + iw1] & GRADIENTSMASK_4D];
    vec4 g1001 = gradients4D[hash[h100 + iw1] & GRADIENTSMASK_4D];
    vec4 g0101 = gradients4D[hash[h010 + iw1] & GRADIENTSMASK_4D];
    vec4 g1101 = gradients4D[hash[h110 + iw1] & GRADIENTSMASK_4D];
    vec4 g0011 = gradients4D[hash[h001 + iw1] & GRADIENTSMASK_4D];
    vec4 g1011 = gradients4D[hash[h101 + iw1] & GRADIENTSMASK_4D];
    vec4 g0111 = gradients4D[hash[h011 + iw1] & GRADIENTSMASK_4D];
    vec4 g1111 = gradients4D[hash[h111 + iw1] & GRADIENTSMASK_4D];

    real32 tx0 = sample.x - Floorf(sample.x);
    real32 ty0 = sample.y - Floorf(sample.y);
    real32 tz0 = sample.z - Floorf(sample.z);
    real32 tw0 = sample.w - Floorf(sample.w);
    real32 tx1 = tx0 - 1.0f;
    real32 ty1 = ty0 - 1.0f;
    real32 tz1 = tz0 - 1.0f;
    real32 tw1 = tw0 - 1.0f;

    real32 v0000 = Dot(g0000, V4(tx0, ty0, tz0, tw0));
    real32 v1000 = Dot(g1000, V4(tx1, ty0, tz0, tw0));
    real32 v0100 = Dot(g0100, V4(tx0, ty1, tz0, tw0));
    real32 v1100 = Dot(g1100, V4(tx1, ty1, tz0, tw0));
    real32 v0010 = Dot(g0010, V4(tx0, ty0, tz1, tw0));
    real32 v1010 = Dot(g1010, V4(tx1, ty0, tz1, tw0));
    real32 v0110 = Dot(g0110, V4(tx0, ty1, tz1, tw0));
    real32 v1110 = Dot(g1110, V4(tx1, ty1, tz1, tw0));
    real32 v0001 = Dot(g0001, V4(tx0, ty0, tz0, tw1));
    real32 v1001 = Dot(g1001, V4(tx1, ty0, tz0, tw1));
    real32 v0101 = Dot(g0101, V4(tx0, ty1, tz0, tw1));
    real32 v1101 = Dot(g1101, V4(tx1, ty1, tz0, tw1));
    real32 v0011 = Dot(g0011, V4(tx0, ty0, tz1, tw1));
    real32 v1011 = Dot(g1011, V4(tx1, ty0, tz1, tw1));
    real32 v0111 = Dot(g0111, V4(tx0, ty1, tz1, tw1));
    real32 v1111 = Dot(g1111, V4(tx1, ty1, tz1, tw1));

    real32 tx = Smooth(tx0);
    real32 ty = Smooth(ty0);
    real32 tz = Smooth(tz0);
    real32 tw = Smooth(tw0);
            
    return Lerp(Lerp(Lerp(Lerp(v0000, v1000, tx), Lerp(v0100, v1100, tx), ty),
                                 Lerp(Lerp(v0010, v1010, tx), Lerp(v0110, v1110, tx), ty),
                                 tz),
                      Lerp(Lerp(Lerp(v0001, v1001, tx), Lerp(v0101, v1101, tx), ty),
                                 Lerp(Lerp(v0011, v1011, tx), Lerp(v0111, v1111, tx), ty),
                                 tz),
                      tw);
}


// Octaves
typedef real32 Noise1f(real32, real32);
typedef real32 Noise2f(vec2, real32);
typedef real32 Noise3f(vec3, real32);
typedef real32 Noise4f(vec4, real32);


inline real32 Sum1f(Noise1f function, real32 sample, real32 frequency, int32 octaves, real32 lacunarity = 2.0f, real32 persistence = 0.5f) {
    real32 sum = function(sample, frequency);
    real32 amplitude = 1.0f;
    real32 range = 1.0f;

    for (int32 i = 1; i < octaves; i++) {
        frequency *= lacunarity;
        amplitude *= persistence;
        range += amplitude;
        sum += function(sample, frequency) * amplitude;
    }

    real32 result = sum / range;
    return result;
}

inline real32 Sum2f(Noise2f function, vec2 sample, real32 frequency, int32 octaves, real32 lacunarity = 2.0f, real32 persistence = 0.5f) {
    real32 sum = function(sample, frequency);
    real32 amplitude = 1.0f;
    real32 range = 1.0f;

    for (int32 i = 1; i < octaves; i++) {
        frequency *= lacunarity;
        amplitude *= persistence;
        range += amplitude;
        sum += function(sample, frequency) * amplitude;
    }

    real32 result = sum / range;
    return result;
}

inline real32 Sum3f(Noise3f function, vec3 sample, real32 frequency, int32 octaves, real32 lacunarity = 2.0f, real32 persistence = 0.5f) {
    real32 sum = function(sample, frequency);
    real32 amplitude = 1.0f;
    real32 range = 1.0f;

    for (int32 i = 1; i < octaves; i++) {
        frequency *= lacunarity;
        amplitude *= persistence;
        range += amplitude;
        sum += function(sample, frequency) * amplitude;
    }

    real32 result = sum / range;
    return result;
}

inline real32 Sum4f(Noise4f function, vec4 sample, real32 frequency, int32 octaves, real32 lacunarity = 2.0f, real32 persistence = 0.5f) {
    real32 sum = function(sample, frequency);
    real32 amplitude = 1.0f;
    real32 range = 1.0f;

    for (int32 i = 1; i < octaves; i++) {
        frequency *= lacunarity;
        amplitude *= persistence;
        range += amplitude;
        sum += function(sample, frequency) * amplitude;
    }

    real32 result = sum / range;
    return result;
}


// Blue Noise
uint32 BlueNoise2D(vec2 min, vec2 max, real32 minPointDistance, vec2 *points, uint32 pointCount) {

    static uint32 maxAttempts = 1024;

    uint32 generatedCount = 0;

    // Bridson 2007 Method
#if 0
    real32 cellSize = minPointDistance / sqrtf(2.0f);
    uint32 gridWidth = (uint32)Ceilf((max.x - min.x) / cellSize);
    uint32 gridHeight = (uint32)Ceilf((max.y - min.y) / cellSize);

    int32 pointIndices[gridWidth * gridHeight];
    for (int i = 0; i < gridWidth * gridHeight; i++) {
        pointIndices[i] = -1;
    }

    int32 activeList[pointCount];
    uint32 activeCount = 0;

    // Insert initial point
    if (pointCount > 0) {
        vec2 point = Lerp(min, max, 0.5f);

        int32 gridX = (int32)Floorf((point.x - min.x) / cellSize);
        int32 gridY = (int32)Floorf((point.y - min.y) / cellSize);
        int32 gridIndex = gridY * gridWidth + gridX;

        points[generatedCount] = point;
        pointIndices[gridIndex] = generatedCount;
        activeList[activeCount] = generatedCount;

        activeCount++;
        generatedCount++;
    }

    int32 neighborCount = 0;
    int32 neighborIndices[8] = {};

    while (activeCount > 0 && generatedCount < pointCount) {
        int32 activeIndex = Randi(activeCount);
        vec2 activePoint = points[activeIndex];

        bool insertedPoint = false;
        for (int attempt = 0; attempt < maxAttempts; attempt++) {
            real32 randRadius = RandfRange(minPointDistance, 2.0f * minPointDistance);
            real32 randAngle = RandfRange(2 * _PI);
            vec2 point = V2(activePoint.x + randRadius * cosf(randAngle),
                            activePoint.y + randRadius * sinf(randAngle));

            int32 gridX = (int32)Floorf((point.x - min.x) / cellSize);
            int32 gridY = (int32)Floorf((point.y - min.y) / cellSize);
            int32 gridIndex = gridY * gridWidth + gridX;

            // Make sure grid position is in range
            if (gridY < 0 || gridX >= gridWidth || gridY < 0 || gridY >= gridHeight) {
                continue;
            }

            // Make sure there isn't a point in the grid cell yet
            if (pointIndices[gridIndex] >= 0) {
                continue;
            }

            // Find Neighboring Cells
            neighborCount = 0;
            if (gridX > 0) {
                if (gridY > 0) {
                    neighborIndices[neighborCount++] = gridIndex - gridWidth - 1;
                }

                neighborIndices[neighborCount++] = gridIndex - 1;

                if (gridY < gridHeight - 1) {
                    neighborIndices[neighborCount++] = gridIndex + gridWidth - 1;
                }
            }

            if (gridY > 0) {
                neighborIndices[neighborCount++] = gridIndex - gridWidth;
            }

            if (gridY < gridHeight - 1) {
                neighborIndices[neighborCount++] = gridIndex + gridWidth;
            }


            if (gridX < gridWidth - 1) {
                if (gridY > 0) {
                    neighborIndices[neighborCount++] = gridIndex - gridWidth + 1;
                }

                neighborIndices[neighborCount++] = gridIndex + 1;

                if (gridY < gridHeight - 1) {
                    neighborIndices[neighborCount++] = gridIndex + gridWidth + 1;
                }
            }

            // Make sure the generated point is outside the range of any neighbors
            bool neighborTooClose = false;
            for (int n = 0; n < neighborCount; n++) {
                int32 neighborIndex = pointIndices[neighborIndices[n]];
                if (neighborIndex < 0) {
                    continue;
                }

                if (Distance(point, points[neighborIndex]) < minPointDistance) {
                    neighborTooClose = true;
                    break;
                }
            }

            if (neighborTooClose) {
                continue;
            }

            // Add the point
            points[generatedCount] = point;
            pointIndices[gridIndex] = generatedCount;
            activeList[activeCount] = generatedCount;

            activeCount++;
            generatedCount++;

            insertedPoint = true;
            break;
        }

        // Remove activePoint from active list if a new point wasn't
        // generated near it
        if (!insertedPoint) {
            if (activeCount >= 2) {
                activeList[activeIndex] = activeList[activeCount - 1];
            }

            activeCount--;
        }
    }
#endif

    // Stupid Dart Throwing
#if 1
    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        vec2 point = V2(RandfRange(min.x, max.x), RandfRange(min.y, max.y));

        bool neighborTooClose = false;
        for (int i = 0; i < generatedCount; i++) {
            if (Distance(point, points[i]) < minPointDistance) {
                neighborTooClose = true;
                break;
            }
        }

        if (!neighborTooClose) {
            points[generatedCount] = point;
            generatedCount++;
        }

        if (generatedCount == pointCount) {
            break;
        }
    }
#endif

    return generatedCount;
}

uint32 BlueNoise2D(real32 minX, real32 maxX, real32 minY, real32 maxY, real32 minPointDistance, vec2 *points, uint32 pointCount) {
    return BlueNoise2D(V2(minX, minY), V2(maxX, maxY), minPointDistance, points, pointCount);
}

float SeamlessPerlinNoise(float frequency, float x1, float y1, float r1, float r2, float s, float t) {
    float lacunarity = 2.0f;
    float persistence = 0.5f;
    int32 numOctaves = 6;

    float nx = x1 + cos(s * 2.0f * _PI) * r1 / (2.0f * _PI);
    float ny = y1 + cos(t * 2.0f * _PI) * r2 / (2.0f * _PI);
    float nz = x1 + sin(s * 2.0f * _PI) * r1 / (2.0f * _PI);
    float nw = y1 + sin(t * 2.0f * _PI) * r2 / (2.0f * _PI);

    vec4 sample = V4(nx, ny, nz, nw);
    float noise = Sum4f(Perlin4f, sample, frequency, numOctaves, lacunarity, persistence);
    float result = noise * 0.5f + 0.5f;

    return result;
}

#endif

