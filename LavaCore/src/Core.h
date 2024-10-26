#pragma once

#ifdef LAVA_PLATFORM_WINDOWS
	#ifdef LAVA_DYNAMIC_LIBRARY
		#ifdef LAVA_BUILD_DLL
			#define LAVA_API __declspec(dllexport)
		#else
			#define LAVA_API __declspec(dllimport)
		#endif
	#else
		#define LAVA_API
	#endif
#else
		#error Lava only support Windows!
#endif


#define BIT(x) 1 << (x)

#define LAVA_DEBUGBREAK __debugbreak();
