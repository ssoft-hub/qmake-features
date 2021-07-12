#pragma once
#ifndef AUTO_VERSION_MODULE_WINDOWS_IMPL
#define AUTO_VERSION_MODULE_WINDOWS_IMPL

#include <libloaderapi.h>

namespace AutoVersion
{
    namespace Windows { typedef void * void_ptr; }

    inline Windows::void_ptr resolveMethod ( LPCSTR module, LPCSTR method )
    {
        HMODULE handle = GetModuleHandleA( module );
        if ( !handle )
            return Windows::void_ptr();
        return Windows::void_ptr( GetProcAddress( handle, method ) );
    }

    inline Windows::void_ptr resolveMethod ( LPCWSTR module, LPCSTR method )
    {
        HMODULE handle = GetModuleHandleW( module );
        if ( !handle )
            return Windows::void_ptr();
        return Windows::void_ptr( GetProcAddress( handle, method ) );
    }
}

#endif
