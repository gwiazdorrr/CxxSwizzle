CxxSwizzle
==========

CxxSwizzle is a header-only extensible library bringing shader languages' (GLSL, HSLS) vector "swizzle" syntax into C++. Basically, you can do this in C++ now:

	vec4 foo(0);		 		        // 0,0,0,0
	foo.yx = vec2(2, 1); 		        // 1,2,0,0
	foo.zw = foo.xy * 2; 				// 1,2,2,4
	vec2 bar = max(foo.xw, foo.yz).yx;  // 4,2
	bar = clamp(foo.xw, 0, 2);			// 1,2	
	// etc.

What's the use? Given GLSL/HLSL similarity to C you can, with a little help from the preprocessor, execute most shaders directly as C++ code. That gives debugging and flexibility capabilities unpararelled by existing shader debugging solutions - you can use IDE of your choice, add logging, assertions, unit tests for functions etc.

	#ifdef __cplusplus
	#include <cassert>
	#include <iostream>
	#define CPP_DO(x) x
	#else
	#define assert(x)
	#define CPP_DO(x)
	#endif

	void test_sin(vec4 a)
	{
		vec4 b = sin(a);
		// sanity checks
		assert( all(lessThanEqual(b, vec4(1))) );
		assert( all(greaterThanEqual(b, vec4(-1))) );
		// print out
		CPP_DO( std::cout << "sin(" << a << ") == " << b << "\n"; )
	}


The library is written in C++11 subset supported by VS2010 - most notably the lack of variadic templates was a great pain and limitation in this case.

*Work is still in progress*. Still the library can be used to debug most shaders found on http://glsl.heroku.com or http://shadertoy.com, some of which are included in the sample.

Check out sample to see how GLSL shader can be used in a simplistic C++ application.

Compability
---------------------------------------------------

All the code compiles both using MSVC 10 and g++ 4.8.1. When using the latter, following flags must be added to the compiler command line:

	-std=c++11 -fno-operator-names

The less obvious second flag disables alterantive operators names, such as `and` ar `not` which, coincentally, are also the names of some GLSL functions.

If using CMake, just set CMAKE_CXX_FLAGS variable.

What can't be done without changing shader's source
---------------------------------------------------

* HLSL Semantics 

Colon following an identifier can't be removed by a preprocessor. One possible workaround:

	#ifdef __cplusplus
	#define SEM(x)
	#else
	#define SEM(x) :x
	#endif

So now:

	float4 position : POSITION;

Would have to become:

	float4 position SEM(POSITION);

Not pretty, but would work and the shader would compile both as C++ and HLSL>



