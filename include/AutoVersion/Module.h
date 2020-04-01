#pragma once
#ifndef AUTO_VERSION_MODULE_IMPL
#define AUTO_VERSION_MODULE_IMPL

#include "Base.h"

#if defined( _WIN64 ) || defined( _WIN32 )
    #include "private/ModuleWindows_p.h"
#else
    #include "private/ModuleUnix_p.h"
#endif

#endif
