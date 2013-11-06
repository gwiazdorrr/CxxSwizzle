// CxxSwizzle
// Copyright (c) 2013, Piotr Gwiazdowski <gwiazdorrr+github at gmail.com>

#include <boost/test/unit_test.hpp>

#include "setup.h"

BOOST_AUTO_TEST_SUITE(SamplesFromStandard)

BOOST_AUTO_TEST_CASE(Par_5_4_2__Constructors)
{
    int _int = 1;
    float _float = 2;
    vec2 _vec2(3.0f, 4.0f);
    vec3 _vec3(5.0f, 6.0f, 7.0f);
    vec4 _vec4(8.0f, 9.0f, 10.0f, 11.0f);
    vec4 _ivec4(12, 13, 14, 15);
    double _double = 16;
    dvec2 _dvec2(17, 18);
    dvec3 _dvec3(19, 20, 21);
    dvec4 _dvec4(22, 23, 24, 25);
    mat2 _mat2(26, 27, 28, 29);

    mat4x4 _mat4x4;
    mat4x2 _mat4x2;
    mat3x3 _mat3x3;
    
    (vec3(_float)); // initializes each component of the vec3 with the float
    (vec4(_ivec4)); // makes a vec4 with component-wise conversion
    // (vec4(_mat2)); // the vec4 is column 0 followed by column 1 <-- THIS DOES NOT WORK
    vec2(_float, _float); // initializes a vec2 with 2 floats
    ivec3(_int, _int, _int); // initializes an ivec3 with 3 ints
    bvec4(_int, _int, _float, _float); // uses 4 Boolean conversions
    (vec2(_vec3)); // drops the third component of a vec3
    (vec3(_vec4)); // drops the fourth component of a vec4
    vec3(_vec2, _float); // vec3.x = vec2.x, vec3.y = vec2.y, vec3.z = float
    vec3(_float, _vec2); // vec3.x = float, vec3.y = vec2.x, vec3.z = vec2.y
    vec4(_vec3, _float);
    vec4(_float, _vec3);
    vec4(_vec2, _vec2);

    vec4 color = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 rgba = vec4(1.0); // sets each component to 1.0
    vec3 rgb = vec3(color); // drop the 4th component

    (mat2(_float));
    (mat3(_float));
    (mat4(_float));

    mat2(_vec2, _vec2); // one column per argument
    mat3(_vec3, _vec3, _vec3); // one column per argument
    mat4(_vec4, _vec4, _vec4, _vec4); // one column per argument
    mat3x2(_vec2, _vec2, _vec2); // one column per argument
    dmat2(_dvec2, _dvec2);
    dmat3(_dvec3, _dvec3, _dvec3);
    dmat4(_dvec4, _dvec4, _dvec4, _dvec4);
    mat2(_float, _float, // first column
        _float, _float); // second column
    mat3(_float, _float, _float, // first column
        _float, _float, _float, // second column
        _float, _float, _float); // third column
    mat4(_float, _float, _float, _float, // first column
        _float, _float, _float, _float, // second column
        _float, _float, _float, _float, // third column
        _float, _float, _float, _float); // fourth column
    mat2x3(_vec2, _float, // first column
        _vec2, _float); // second column
    dmat2x4(_dvec3, _double, // first column
        _double, _dvec3); // second column

    (mat3x3(_mat4x4)); // takes the upper-left 3x3 of the mat4x4
    (mat2x3(_mat4x2)); // takes the upper-left 2x2 of the mat4x4, last row is 0,0
    (mat4x4(_mat3x3)); // puts the mat3x3 in the upper-left, sets the lower right
    // component to 1, and the rest to 0 
}

BOOST_AUTO_TEST_CASE(Par_5_5__Vector_and_Scalar_Components_and_Length)
{
    {
        vec2 pos;
        float height;
        pos.x; // is legal
        // pos.z // is illegal
        // height.x; // is legal  <-- THIS DOES NOT WORK
        // height.y // is illegal
    }

    {
        vec4 v4;
        v4.rgba; // is a vec4 and the same as just using v4,
        v4.rgb; // is a vec3,
        v4.b; // is a float,
        v4.xy; // is a vec2,
        // v4.xgba; // is illegal - the component names do not come from 
    }

    {
        vec4 pos = vec4(1.0, 2.0, 3.0, 4.0);
        vec4 swiz= pos.wzyx; BOOST_ASSERT(swiz == vec4(4.0, 3.0, 2.0, 1.0));
        vec4 dup = pos.xxyy; BOOST_ASSERT(dup == vec4(1.0, 1.0, 2.0, 2.0));
        float f = 1.2;
        // vec4 dup = f.xxxx; // dup = (1.2, 1.2, 1.2, 1.2) <-- THIS DOES NOT WORK
    }

    {
        vec4 pos = vec4(1.0, 2.0, 3.0, 4.0);
        pos.xw = vec2(5.0, 6.0); BOOST_ASSERT(pos == vec4(5.0, 2.0, 3.0, 6.0));
        pos.wx = vec2(7.0, 8.0); BOOST_ASSERT(pos == vec4(8.0, 2.0, 3.0, 7.0));
        //pos.xx = vec2(3.0, 4.0); // illegal - 'x' used twice
        //pos.xy = vec3(1.0, 2.0, 3.0); // illegal - mismatch between vec2 and vec3
    }

    {
        vec3 v;
        // const int L = v.length();  <-- THIS DOES NOT WORK
    }
}

BOOST_AUTO_TEST_CASE(Par_5_6__Matrix_Components)
{
    {
        mat4 m;
        m[1] = vec4(2.0); // sets the second column to all 2.0
        m[0][0] = 1.0; // sets the upper left element to 1.0
        m[2][3] = 2.0; // sets the 4th element of the third column to 2.0
    }

    {
        mat3x4 v;
        // const int L = v.length();   <-- THIS DOES NOT WORK
    }
}

BOOST_AUTO_TEST_CASE(Par_5_10__Vector_and_Matrix_Operations)
{
    {
        vec3 v1, v2;
        {
            vec3 v(1), u(2);
            float f = 3;
            v = u + f;
            v1 = v;
        }
        // will be equivalent to 
        {
            vec3 v(1), u(2);
            float f = 3;
            v.x = u.x + f;
            v.y = u.y + f;
            v.z = u.z + f;
            v2 = v;
        }
        BOOST_ASSERT( v1 == v2 );
    }

    {
        vec3 v1, v2;
        {
            vec3 v(1), u(2), w(3);
            w = v + u;
            v1 = w;
        }
        {
            vec3 v(1), u(2), w(3);
            w.x = v.x + u.x;
            w.y = v.y + u.y;
            w.z = v.z + u.z;
            v2 = w;
        }
        BOOST_ASSERT( v1 == v2 );
    }

    {
        vec3 u1, u2;
        {
            vec3 v(1), u(2);
            mat3 m(-1);
            u = v * m;
            u1 = u;
        }
        
        {
            vec3 v(1), u(2);
            mat3 m(-1);
            u.x = dot(v, m[0]); // m[0] is the left column of m
            u.y = dot(v, m[1]); // dot(a,b) is the inner (dot) product of a and b
            u.z = dot(v, m[2]);
            u2 = u;
        }
        BOOST_ASSERT(u1 == u2);

    }

    {
        vec3 u1, u2;
        {
            vec3 v(1), u(2);
            mat3 m(-1);
            u = m * v;
            u1 = u;
        }
        {
            vec3 v(1), u(2);
            mat3 m(-1);
            u.x = m[0].x * v.x + m[1].x * v.y + m[2].x * v.z;
            u.y = m[0].y * v.x + m[1].y * v.y + m[2].y * v.z;
            u.z = m[0].z * v.x + m[1].z * v.y + m[2].z * v.z;
            u2 = u;
        }
        BOOST_ASSERT(u1 == u2);
    }

    {
        mat3 r1, r2;
        {
            mat3 m(1,2,3,4,5,6,7,8,9), n(11,12,13,14,15,16,17,18,19), r;
            r = m * n;
            r1 = r;
        }
        {
            mat3 m(1,2,3,4,5,6,7,8,9), n(11,12,13,14,15,16,17,18,19), r;
            r[0].x = m[0].x * n[0].x + m[1].x * n[0].y + m[2].x * n[0].z;
            r[1].x = m[0].x * n[1].x + m[1].x * n[1].y + m[2].x * n[1].z;
            r[2].x = m[0].x * n[2].x + m[1].x * n[2].y + m[2].x * n[2].z;
            r[0].y = m[0].y * n[0].x + m[1].y * n[0].y + m[2].y * n[0].z;
            r[1].y = m[0].y * n[1].x + m[1].y * n[1].y + m[2].y * n[1].z;
            r[2].y = m[0].y * n[2].x + m[1].y * n[2].y + m[2].y * n[2].z;
            r[0].z = m[0].z * n[0].x + m[1].z * n[0].y + m[2].z * n[0].z;
            r[1].z = m[0].z * n[1].x + m[1].z * n[1].y + m[2].z * n[1].z;
            r[2].z = m[0].z * n[2].x + m[1].z * n[2].y + m[2].z * n[2].z;
            r2 = r;
        }
        BOOST_ASSERT(r1 == r2);
    }
}

BOOST_AUTO_TEST_SUITE_END()