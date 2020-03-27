#pragma once
#ifndef AUTO_VERSION_APPLICATION_IMPL
#define AUTO_VERSION_APPLICATION_IMPL

#include "Base.h"

namespace AutoVersion
{
    // implemented inside application
    extern ::AutoVersion::Info info ();
    extern ::AutoVersion::CompileVersion compileVersion ();
    extern ::AutoVersion::RuntimeVersion runtimeVersion ();
    extern ::AutoVersion::Version version ();
}

#if defined( _WIN64 ) || defined( _WIN32 )
    #include "private/ApplicationWindows_p.h"
#else
    #include "private/ApplicationUnix_p.h"
#endif

#endif
