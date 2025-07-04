// stolen from optix-toolkit https://github.com/NVIDIA/optix-toolkit/blob/master/ShaderUtil/include/OptiXToolkit/ShaderUtil/Transform4.h
#pragma once
#include "vec_math.h"

/// 4x4 homogeneous coordinate transformation matrix
class Transform4 {
public:
    float4 m[4];

    SUTIL_INLINE SUTIL_HOSTDEVICE void to_float12(float to[12]);
};

SUTIL_INLINE SUTIL_HOSTDEVICE void Transform4::to_float12(float to[12]) {
    to[0] = m[0].x;
    to[1] = m[0].y;
    to[2] = m[0].z;
    to[3] = m[0].w;

    to[4] = m[1].x;
    to[5] = m[1].y;
    to[6] = m[1].z;
    to[7] = m[1].w;

    to[8] = m[2].x;
    to[9] = m[2].y;
    to[10] = m[2].z;
    to[11] = m[2].w;
}

SUTIL_INLINE SUTIL_HOSTDEVICE bool operator==( const Transform4& t1, const Transform4& t2 ) {
    const float4( &lhs )[4] = t1.m;
    const float4( &rhs )[4] = t2.m;

    return lhs[0] == rhs[0] &&
           lhs[1] == rhs[1] &&
           lhs[2] == rhs[2] &&
           lhs[3] == rhs[3];
}

SUTIL_INLINE SUTIL_HOSTDEVICE bool operator!=( const Transform4& lhs, const Transform4& rhs ) {
    return !( lhs == rhs );
}

SUTIL_INLINE SUTIL_HOSTDEVICE Transform4 identity() {
    constexpr float zero{};
    return Transform4{ make_float4( 1.0f, zero, zero, zero ), make_float4( zero, 1.0f, zero, zero ),
        make_float4( zero, zero, 1.0f, zero ), make_float4( zero, zero, zero, 1.0f ) };
}

SUTIL_INLINE SUTIL_HOSTDEVICE Transform4 translate(float x = 0.0f, float y = 0.0f, float z = 0.0f) {
    Transform4 result{identity()};
    result.m[0].w = x;
    result.m[1].w = y;
    result.m[2].w = z;
    return result;
}

SUTIL_INLINE SUTIL_HOSTDEVICE Transform4 scale(float x = 1.0f, float y = 1.0f, float z = 1.0f) {
    Transform4 result{identity()};
    result.m[0].x = x;
    result.m[1].y = y;
    result.m[2].z = z;
    return result;
}

// stolen from glm https://github.com/g-truc/glm/blob/master/glm/ext/matrix_transform.inl
// axis should be normalized
SUTIL_INLINE SUTIL_HOSTDEVICE Transform4 rotate(float angle, float3 const& axis) {
    float const a = angle;
    float const c = cos(a);
    float const s = sin(a);

    float3 temp = (1 - c) * axis;

    Transform4 Rotate{identity()};
    Rotate.m[0].x = c + temp.x * axis.x;
    Rotate.m[0].y = temp.x * axis.y + s * axis.z;
    Rotate.m[0].z = temp.x * axis.z - s * axis.y;

    Rotate.m[1].x = temp.y * axis.x - s * axis.z;
    Rotate.m[1].y = c + temp.y * axis.y;
    Rotate.m[1].z = temp.y * axis.z + s * axis.x;

    Rotate.m[2].x = temp.z * axis.x + s * axis.y;
    Rotate.m[2].y = temp.z * axis.y - s * axis.x;
    Rotate.m[2].z = c + temp.z * axis.z;

    return Rotate;
}

SUTIL_INLINE SUTIL_HOSTDEVICE Transform4 operator-(const Transform4& transform) {
    return Transform4{-transform.m[0], -transform.m[1], -transform.m[2], -transform.m[3]};
}

SUTIL_INLINE SUTIL_HOSTDEVICE float4 operator*( const Transform4& lhs, const float4& rhs ) {
    return make_float4( dot( lhs.m[0], rhs ), dot( lhs.m[1], rhs ), dot( lhs.m[2], rhs ), dot( lhs.m[3], rhs ) );
}

SUTIL_INLINE SUTIL_HOSTDEVICE float4 operator*( const Transform4& lhs, const float2& rhs ) {
    return lhs * make_float4( rhs.x, rhs.y, 0.0f, 1.0f );
}

SUTIL_INLINE SUTIL_HOSTDEVICE float4 operator*( const Transform4& lhs, const float3& rhs ) {
    return lhs * make_float4( rhs.x, rhs.y, rhs.z, 1.0f );
}

SUTIL_INLINE SUTIL_HOSTDEVICE Transform4 operator*( const Transform4& lhs, const Transform4& rhs ) {
    const float4 rhsColumns[4]{ make_float4( rhs.m[0].x, rhs.m[1].x, rhs.m[2].x, rhs.m[3].x ),    //
                                make_float4( rhs.m[0].y, rhs.m[1].y, rhs.m[2].y, rhs.m[3].y ),    //
                                make_float4( rhs.m[0].z, rhs.m[1].z, rhs.m[2].z, rhs.m[3].z ),    //
                                make_float4( rhs.m[0].w, rhs.m[1].w, rhs.m[2].w, rhs.m[3].w ) };  //

    Transform4 result{};
    for (int row = 0; row < 4; ++row)
    {
        result.m[row].x = dot( lhs.m[row], rhsColumns[0] );
        result.m[row].y = dot( lhs.m[row], rhsColumns[1] );
        result.m[row].z = dot( lhs.m[row], rhsColumns[2] );
        result.m[row].w = dot( lhs.m[row], rhsColumns[3] );
    }
    return result;
}

SUTIL_INLINE SUTIL_HOSTDEVICE Transform4 transpose(const Transform4& transform) {
    Transform4 result;
    const float4 (&a)[4] = transform.m;

    result.m[0] = make_float4(a[0].x, a[1].x, a[2].x, a[3].x);
    result.m[1] = make_float4(a[0].y, a[1].y, a[2].y, a[3].y);
    result.m[2] = make_float4(a[0].z, a[1].z, a[2].z, a[3].z);
    result.m[3] = make_float4(a[0].w, a[1].w, a[2].w, a[3].w);
    return result;
}

// cribbed from Stack Overflow: https://stackoverflow.com/questions/2624422/efficient-4x4-matrix-inverse-affine-transform
SUTIL_INLINE SUTIL_HOSTDEVICE Transform4 inverse( const Transform4& transform ) {
    const float4 (&a)[4] = transform.m;

    const float s0 = a[0].x * a[1].y - a[1].x * a[0].y;
    const float s1 = a[0].x * a[1].z - a[1].x * a[0].z;
    const float s2 = a[0].x * a[1].w - a[1].x * a[0].w;
    const float s3 = a[0].y * a[1].z - a[1].y * a[0].z;
    const float s4 = a[0].y * a[1].w - a[1].y * a[0].w;
    const float s5 = a[0].z * a[1].w - a[1].z * a[0].w;

    const float c5 = a[2].z * a[3].w - a[3].z * a[2].w;
    const float c4 = a[2].y * a[3].w - a[3].y * a[2].w;
    const float c3 = a[2].y * a[3].z - a[3].y * a[2].z;
    const float c2 = a[2].x * a[3].w - a[3].x * a[2].w;
    const float c1 = a[2].x * a[3].z - a[3].x * a[2].z;
    const float c0 = a[2].x * a[3].y - a[3].x * a[2].y;

    // Should check for 0 determinant
    const float invdet = 1.0f / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

    Transform4 result;
    float4 (&b)[4] = result.m;

    b[0].x = ( a[1].y * c5 - a[1].z * c4 + a[1].w * c3) * invdet;
    b[0].y = (-a[0].y * c5 + a[0].z * c4 - a[0].w * c3) * invdet;
    b[0].z = ( a[3].y * s5 - a[3].z * s4 + a[3].w * s3) * invdet;
    b[0].w = (-a[2].y * s5 + a[2].z * s4 - a[2].w * s3) * invdet;

    b[1].x = (-a[1].x * c5 + a[1].z * c2 - a[1].w * c1) * invdet;
    b[1].y = ( a[0].x * c5 - a[0].z * c2 + a[0].w * c1) * invdet;
    b[1].z = (-a[3].x * s5 + a[3].z * s2 - a[3].w * s1) * invdet;
    b[1].w = ( a[2].x * s5 - a[2].z * s2 + a[2].w * s1) * invdet;

    b[2].x = ( a[1].x * c4 - a[1].y * c2 + a[1].w * c0) * invdet;
    b[2].y = (-a[0].x * c4 + a[0].y * c2 - a[0].w * c0) * invdet;
    b[2].z = ( a[3].x * s4 - a[3].y * s2 + a[3].w * s0) * invdet;
    b[2].w = (-a[2].x * s4 + a[2].y * s2 - a[2].w * s0) * invdet;

    b[3].x = (-a[1].x * c3 + a[1].y * c1 - a[1].z * c0) * invdet;
    b[3].y = ( a[0].x * c3 - a[0].y * c1 + a[0].z * c0) * invdet;
    b[3].z = (-a[3].x * s3 + a[3].y * s1 - a[3].z * s0) * invdet;
    b[3].w = ( a[2].x * s3 - a[2].y * s1 + a[2].z * s0) * invdet;

    return result;
}
