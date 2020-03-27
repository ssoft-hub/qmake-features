#pragma once
#ifndef AUTO_VERSION_UNIX_IMPL
#define AUTO_VERSION_UNIX_IMPL

namespace AutoVersion
{
    struct Application
    {
        typedef ::AutoVersion::RuntimeVersion::Module Module;
        typedef ::AutoVersion::RuntimeVersion::Modules Modules;

        RuntimeVersion m_runtime;

        Application ()
        {
            // ...
        }

        ::AutoVersion::RuntimeVersion runtimeVersion ()
        {
            return m_runtime;
        }
    };
}

#endif
