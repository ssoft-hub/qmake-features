#pragma once
#ifndef AUTO_VERSION_STRUCTS
#define AUTO_VERSION_STRUCTS

#include <algorithm>
#include <cstring>
#include <list>

namespace AutoVersion
{
    typedef const char * Chars;

    struct String
    {
        Chars c_str;

        String ( Chars chars = Chars() )
            : c_str( chars )
        {}

        String & operator = ( Chars chars )
        {
            c_str = chars;
            return *this;
        }

        operator Chars () const
        {
            return c_str;
        }

        operator bool () const
        {
            return !!c_str;
        }

        bool operator == ( const String & other ) const
        {
            return ( c_str == other.c_str )
                ? true
                : ( !c_str || !other.c_str )
                    ? false
                    : strcmp( c_str, other.c_str ) == 0;
        }

        bool operator != ( const String & other ) const
        {
            return !( *this == other );
        }
    };
}

namespace AutoVersion
{
    struct Info
    {
        String product;
        String version;
        String revision;
        String date;
        String time;
        String vendor;
        String copyright;
        String license;
        String description;

        Info ()
            : product()
            , version()
            , revision()
            , date()
            , time()
            , vendor()
            , copyright()
            , license()
            , description()
        {}
    };
}

namespace AutoVersion
{
    struct Dependency
    {
        Info build;
        Info runtime;
    };

    typedef ::std::list< Dependency > Dependencies;
}

namespace AutoVersion
{
    struct Component;
    typedef ::std::list< Component > Components;

    struct Component
    {
        Info info;
        Dependencies dependencies;
        Components components;
    };

    typedef ::std::list< Component > Components;
}

namespace AutoVersion
{
    extern Component component ();
}

namespace AutoVersion
{
    // old C++ support
    struct Status
    {
        enum Enum
        {
            Invalid,    //!< Different revision
            Different,  //!< Different information (except revision)
            Valid       //!< Perfect coincidence
        };
    };

    inline Status::Enum versionStatus ( const Dependency & value )
    {
        const Info & build = value.build;
        const Info & runtime = value.runtime;

        if ( build.revision != runtime.revision )
            return Status::Invalid;

        if ( build.product == runtime.product
            && build.version == runtime.version
            && build.date == runtime.date
            && build.time == runtime.time
            && build.vendor == runtime.vendor
            && build.copyright == runtime.copyright
            && build.license == runtime.license
            && build.description == runtime.description )
        {
            return Status::Valid;
        }

        return Status::Different;
    }

    inline Status::Enum versionStatus ( const Component & value )
    {
        Status::Enum result = Status::Valid;

        // old C++ support
        for ( Dependencies::const_iterator dep_iter = value.dependencies.cbegin();
            result != Status::Invalid && dep_iter != value.dependencies.cend(); ++dep_iter )
        {
            result = ::std::min( result, versionStatus( *dep_iter ) );
        }

        // old C++ support
        for ( Components::const_iterator comp_iter = value.components.cbegin();
            result != Status::Invalid && comp_iter != value.components.cend(); ++comp_iter )
        {
            result = ::std::min( result, versionStatus( *comp_iter ) );
        }

        return result;
    }
}

#endif
