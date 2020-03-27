#pragma once
#ifndef AUTO_VERSION_BASE
#define AUTO_VERSION_BASE

#include <cstring>
#include <list>
#include <map>
#include <set>
#include <string>

namespace AutoVersion
{
    /*!
     * Information about program module.
     *
     * This struct is typically handled by automatic procedure
     * and rarely needs to be modified or handled by manual.
     */
    struct Info
    {
        typedef ::std::string Key;
        typedef typename ::std::wstring Attribute;
        typedef ::std::map< Key, Attribute > Attributes;

        Attributes m_attributes;

        Attribute & operator [] ( const Key & key )
        {
            return m_attributes[ key ];
        }

        const Attribute & operator [] ( const Key & key ) const
        {
            static const Attribute dummy;
            Attributes::const_iterator iter = m_attributes.find( key );
            return iter == m_attributes.end()
                ? dummy
                : iter->second;
        }

        static Key key ( char * value ) { return Key( value, value + strlen( value ) ); }
        static Key key ( const char * value ) { return Key( value, value + strlen( value ) ); }
        static Key key ( wchar_t * value ) { return Key( value, value + wcslen( value ) ); }
        static Key key ( const wchar_t * value ) { return Key( value, value + wcslen( value ) ); }
        template < typename _Container > static Key key ( const _Container & container ) { return Key( container.begin(), container.end() ); }

        static Attribute attribute ( char * value ) { return Attribute( value, value + strlen( value ) ); }
        static Attribute attribute ( const char * value ) { return Attribute( value, value + strlen( value ) ); }
        static Attribute attribute ( wchar_t * value ) { return Attribute( value, value + wcslen( value ) ); }
        static Attribute attribute ( const wchar_t * value ) { return Attribute( value, value + wcslen( value ) ); }
        template < typename _Container > static Attribute attribute ( const _Container & container ) { return Attribute( container.begin(), container.end() ); }
    };
}

namespace AutoVersion
{
    struct CompileVersion
    {
        typedef ::AutoVersion::Info Info;
        typedef ::std::list< CompileVersion > CompileVersions;

        Info m_info;
        CompileVersions m_dependencies;
    };

    struct RuntimeVersion
    {
        struct Module
        {
            typedef ::AutoVersion::Info::Attribute Alias;
            typedef ::std::set< Alias > Aliases;

            Info m_info;
            Aliases m_aliases;

            Module () : m_info() , m_aliases() { }
        };

        typedef ::std::map< Module::Alias, Module > Modules;
        Modules m_modules;
    };

    struct Version
    {
        CompileVersion m_compile;
        RuntimeVersion m_runtime;
    };
}

namespace AutoVersion
{
    /*!
     * Version status enumeration.
     */
    struct Status
    {
        // C++98 support
        enum Enum
        {
            Invalid,    //!< Different revision
            Different,  //!< Different information (except revision)
            Valid,      //!< Perfect coincidence
            Undefined   //!< Undefined status
        };
    };

    /*!
     * Returns status of compile-time and run-time information.
     *
     * If build-time revision not coincidences with run-time revision then
     * method returns Status::Invalid. Else if submodule status is not valid
     * (Status::Invalid or Status::Different) then method returns status of
     * submodule. Else if build-time information fully coincidences with run-time
     * information then method returns Status::Valid. In oposite case method
     * returns Status::Different.
     */
    inline Status::Enum infoStatus ( const Info & compile,  const Info & runtime )
    {
        if ( compile[ Info::key( "product" ) ].empty() )
            return Status::Undefined;

        if ( compile[ Info::key( "revision" ) ] != runtime[ Info::key( "revision" ) ] )
            return Status::Invalid;

        if ( compile[ Info::key( "version" ) ] != runtime[ Info::key( "version" ) ]
            || compile[ Info::key( "revision moment" ) ] != runtime[ Info::key( "revision moment" ) ] )
        {
            return Status::Different;
        }

        return Status::Valid;
    }

    /*!
     * Returns status of module version.
     *
     * Searches all status of module dependencies and returnds worst of them.
     */
    inline Status::Enum versionStatus ( const CompileVersion & compile, const RuntimeVersion & runtime )
    {
        typedef const Info & InfoRef;
        typedef const Info * InfoPtr;

        InfoRef compile_info_ref = compile.m_info;
        RuntimeVersion::Modules::const_iterator iter = runtime.m_modules.find( compile_info_ref[ Info::key( "product" ) ] );
        InfoPtr runtime_info_ptr = iter != runtime.m_modules.end() ? &(iter->second.m_info) : InfoPtr();

        Status::Enum result = runtime_info_ptr ? infoStatus( compile_info_ref, *runtime_info_ptr ) : Status::Undefined;

        // C++98 support
        for ( CompileVersion::CompileVersions::const_iterator iter = compile.m_dependencies.begin();
            result != Status::Invalid && iter != compile.m_dependencies.end(); ++iter )
        {
            result = ::std::min( result, versionStatus( *iter, runtime ) );
        }

        return result;
    }
}

#endif
