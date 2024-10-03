#pragma once

#ifdef LAVA_PLATFORM_WINDOWS
	#ifdef LAVA_API
		#define LAVA_BUILD_DLL __declspec(dllexport)
	#else
		#define LAVA_BUILD_DLL __declspec(dllimport)
	#endif
#else
	#error Lava only support Windows!
#endif

