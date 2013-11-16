CxxSwizzle
==========

CxxSwizzle (a reality-friendly way of writing down "C++ Swizzle") is a header-only, dependency free extensible library bringing shader languages' (GLSL, HSLS) vector "swizzle" syntax into C++. Basically, you can do this in C++ now:

	vec4 foo(0);		 		        // 0,0,0,0
	foo.yx = vec2(2, 1); 		        // 1,2,0,0
	foo.zw = foo.xy * 2; 				// 1,2,2,4
	vec2 bar = max(foo.xw, foo.yz).yx;  // 4,2
	bar = clamp(foo.xw, 0, 2);			// 1,2	
	mat2 m(foo.xyz, 1);					// 1,2,2,1
	bar *= m;							// 5,2
	// etc.

What's the use? Familiar and easy to use syntax for non-critical pieces of code. Also, given GLSL/HLSL similarity to C you can execute most shaders directly as C++ code, with just a little help from the preprocessor (keyword substitution). That gives debugging and flexibility capabilities unpararelled by existing shader debugging solutions - you can use IDE of your choice, add logging, assertions, breakpoints (even conditional ones!), unit tests for functions etc.

The following snippet will compile both in C++ and GLSL.

	#ifdef __cplusplus
	#include <cassert>
	#include <iostream>
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

		#ifdef __cplusplus
		
        // test trigonometric symmetry
		const float c_pi = 3.14159265358979323846f;
        const vec4 c_eps = vec4(0.0001);

        vec4 c = cos(c_pi/2 - a);
		if ( any(greaterThan(abs(c - b), c_eps)) )
		{
			// programmatic breakpoint
			__asm int 3;
		}
		
        #endif
	}

In fact you can pretty much take any shader from http://glsl.heroku.com or http://shadertoy.com (the second one takes loooong to load) and execute it as C++ code. The sample project is a simplistic example of how to do exactly that. For instance, this shader from sample, can be run both as GLSL (try it here: http://glsl.heroku.com/e#10661.0) and as C++ (in sample, shaders/leadlight.frag):

	uniform float time;
	uniform vec2 mouse;
	uniform vec2 resolution;

	// Leadlight by @hintz 2013-05-02

	void main()
	{
			vec2 position = gl_FragCoord.xy / resolution.x - 0.5;
			
			float r = length(position);
			float a = atan(position.y, position.x);
			float t = time + 100.0/(r+1.0);
			
			float light = 15.0*abs(0.05*(sin(t)+sin(time+a*8.0)));
			vec3 color = vec3(-sin(r*5.0-a-time+sin(r+t)), sin(r*3.0+a-cos(time)+sin(r+t)), cos(r+a*2.0+log(5.001-(a/4.0))+time)-sin(r+t));
			
			gl_FragColor = vec4((normalize(color)+0.3) * light , 1.0);
	}

Note that contrary to the headers the sample needs SDL library. 
	
HLSL can be compiled as well, but likely not without some changes. There's no way to make semantics valid in C++, for instance. Also, named cbuffers would need some work. I am still looking into this.

The library is written in C++11 subset supported by VS2010 - most notably the lack of variadic templates was a great pain and limitation. It works with g++ 4.8.1 and most likely will work with clang, too. There are no external dependencies.

Compability
---------------------------------------------------

When using g++, following flags must be added to the compiler command line:

	-std=c++11 -fno-operator-names

The less obvious second flag disables alterantive operators names, such as `and` ar `not` which, coincidentally, are also the names of some GLSL functions.

If using CMake, just set CMAKE_CXX_FLAGS variable.

Diferences between GLM
---------------------------------------------------

This shows up a lot in comments - why use CxxSwizzle and not GLM (http://glm.g-truc.net)? Well, here are main differences:

* GLM swizzling seems kind of half done and has its quirks. Bottom line, there's nowhere near as much flexibility as you have with GLSL/HLSL and CxxSwizzle.
* GLM is highly susceptible to ambiguity errors. In GLSL/HLSL number literals are contextual, i.e. `0.5` can mean any precision floating point number. Not in C++.
* GLM can't handle some vector construction cases.
* GLM has more features. Covers greater array of GLSL functions, as well as "support" OGL and GLU functions. Something CxxSwizzle will hopefully catch up soon.
* GLM has an optional SIMD support. Focus of CxxSwizzle was not a performance; it *is going to be painfully slow* regardless if you are going to emulate shader or even structure your code similarly to shaders. SSE is not going to help here much. However, I do recognise the room for improvement and am aware of naivity of current math implementation - hence the `swizzle::glsl::naive` namespace.
* GLM is mature

Other, "ideological" differences:
* GLM is code and macro heavy (for what it provides), while CxxSwizzle is compact, clean and mostly template based
