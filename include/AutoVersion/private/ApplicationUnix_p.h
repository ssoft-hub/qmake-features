#pragma once
#ifndef AUTO_VERSION_APPLICATION_UNIX_IMPL
#define AUTO_VERSION_APPLICATION_UNIX_IMPL

#include <dlfcn.h>

namespace AutoVersion
{
    extern ::AutoVersion::CompileVersion compileVersion ();

    struct Application
    {
        typedef ::AutoVersion::RuntimeVersion::Module Module;
        typedef ::AutoVersion::RuntimeVersion::Modules Modules;

        RuntimeVersion m_runtime;

        Application ()
        {
            CompileVersion compile = compileVersion();
            if ( compile.m_dependencies )
            {
                const CompileVersion::Dependencies & dependencies = *compile.m_dependencies;
                for ( CompileVersion::Dependencies::const_iterator iter = dependencies.begin();
                    iter != dependencies.end(); ++iter )
                {
                    const CompileVersion::Dependency & dependency = *iter;
                    // product
                    Module module;
                    Info::Attribute product = dependency.m_info[ Info::key( "product" ) ];
                    module.m_info[ Info::key( "product" ) ] = product;

                    ::std::string module_name = ::std::string( product.begin(), product.end() );
                    ::std::string version_function_name = ::std::string( "runtimeInfoAbout_" ) + module_name;

                    typedef ::AutoVersion::Info (*InfoMethod) ();
                    InfoMethod info_method = InfoMethod( ::dlsym( RTLD_DEFAULT, version_function_name.c_str() ) );
                    if ( info_method )
                        module.m_info = info_method();

                    if ( product == Info::attribute( "Qt5Core" ) || product == Info::attribute( "Qt5Cored" )
                        || product == Info::attribute( "QtCore4" ) || product == Info::attribute( "QtCored4" ) )
                    {
                        // case Qt version method
                        typedef const char * (*QtVersionMethod) ();
                        QtVersionMethod qt_version_method = QtVersionMethod( ::dlsym( RTLD_DEFAULT, "qVersion" ) );
                        if ( qt_version_method )
                            module.m_info[ Info::key( "revision" ) ] = module.m_info[ Info::key( "version" ) ] = Info::attribute( qt_version_method() );
                    }
                    m_runtime.m_modules[ product ] = module;
                }
            }
        }

        ::AutoVersion::RuntimeVersion runtimeVersion ()
        {
            return m_runtime;
        }
    };
}

#endif
