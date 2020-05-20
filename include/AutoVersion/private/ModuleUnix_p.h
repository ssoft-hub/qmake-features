#pragma once
#ifndef AUTO_VERSION_MODULE_UNIX_IMPL
#define AUTO_VERSION_MODULE_UNIX_IMPL

#include <dlfcn.h>

namespace AutoVersion
{
    inline void * resolveMethod ( const char * /*module*/, const char * method )
    {
        return ::dlsym( RTLD_DEFAULT, method );
    }
}

#endif
