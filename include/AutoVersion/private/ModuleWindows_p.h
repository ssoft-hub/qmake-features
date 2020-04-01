#pragma once
#ifndef AUTO_VERSION_MODULE_WINDOWS_IMPL
#define AUTO_VERSION_MODULE_WINDOWS_IMPL

#include <libloaderapi.h>

namespace AutoVersion
{

    inline FARPROC resolveMethod ( LPCSTR module, LPCSTR method )
    {
        HMODULE handle = GetModuleHandleA( module );
        if ( !handle )
            return FARPROC();
        return GetProcAddress( handle, method );
    }

    inline FARPROC resolveMethod ( LPCWSTR module, LPCSTR method )
    {
        HMODULE handle = GetModuleHandleW( module );
        if ( !handle )
            return FARPROC();
        return GetProcAddress( handle, method );
    }
}

#endif
