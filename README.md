CxxSwizzle
==========

CxxSwizzle is a header-only extensible library bringing shader language's (GLSL, HSLS) vector "swizzle" syntax into C++. Basically, you can do this in C++ now:

	vec4 foo(0);		 		        // 0,0,0,0
	foo.yx = vec2(2, 1); 		        // 1,2,0,0
	foo.zw = foo.xy * 2; 				// 1,2,2,4
	vec2 bar = max(foo.xw, foo.yz).yx   // 4,2
	bar = clamp(foo.xw, 0, 2);			// 1,2	
	// etc.

What's the use? Given GLSL/HLSL similarity to C you can, with a little help from the preprocessor, execute most shaders directly as C++ code. That gives debugging and flexibility capabilities unpararelled by existing shader debugging solutions. 

The library is written in C++11 subset supported by VS2010 - most notably the lack of variadic templates was a great pain and limitation in this case.

As far as compability with GLSL goes, there are some huge gaps at the moment:
* matrices not implemented
* textures not implemented
* some functions missing

None of these is actually dificult, there simply wasn't enough time. *Work is still in progress*. Still the library can be used to debug most shaders found on http://glsl.heroku.com or http://shadertoy.com, some of which are included in the sample.