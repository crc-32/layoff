//This file includes libnx and stubs gcc attributes not supported by visual studio when building for windows
#pragma once

#if !defined(__SWITCH__)
	#include "..\Stubs.h"
#else
	#include <switch.h>
#endif