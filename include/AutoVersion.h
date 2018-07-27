#pragma once
#ifndef AUTO_VERSION_STRUCTS
#define AUTO_VERSION_STRUCTS

#include <algorithm>
#include <cstring>
#include <list>

namespace AutoVersion
{
    typedef const char * Chars;

    /*!
     * Safe wrapper (view) around 'const char *' string value.
     */
    struct String
    {
        Chars c_str;

        String ( Chars chars = Chars() )
            : c_str( chars )
        {}

        size_t length () const
        {
            return !c_str
                ? size_t()
                : strlen( c_str );
        }

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
    /*!
     * Information about program module.
     *
     * This struct is typically handled by automatic procedure
     * and rarely needs to be modified or handled by manual.
     */
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
    struct Dependency;
    typedef ::std::list< Dependency > Dependencies;

    /*!
     * Full version information about program module.
     * Version contains information about module and it's dependencies
     * for module's build-time.
     *
     * This struct is typically handled by automatic procedure
     * and rarely needs to be modified or handled by manual.
     */
    struct Version
    {
        Info info;
        Dependencies dependencies;
    };

    /*!
     * Information about dependency of program module.
     * Dependency contains information about submodule for parent module's
     * build-time and full version information about submodule for submodule's
     * build-time (parent module's run-time).
     *
     * This struct is typically handled by by automatic procedure
     * and rarely needs to be modified or handled by manual.
     */
    struct Dependency
    {
        Info info;
        Version version;
    };
}

namespace AutoVersion
{
    extern Version version ();
}

namespace AutoVersion
{
    // old C++ support
    /*!
     * Version status enumeration.
     */
    struct Status
    {
        enum Enum
        {
            Invalid,    //!< Different revision
            Different,  //!< Different information (except revision)
            Valid       //!< Perfect coincidence
        };
    };

    Status::Enum versionStatus ( const Dependency & value );
    Status::Enum versionStatus ( const Version & value );

    /*!
     * Returns status of module dependency.
     *
     * If build-time revision not coincidences with run-time revision then
     * method returns Status::Invalid. Else if submodule status is not valid
     * (Status::Invalid or Status::Different) then method returns status of
     * submodule. Else if build-time information fully coincidences with run-time
     * information then method returns Status::Valid. In oposite case method
     * returns Status::Different.
     */
    inline Status::Enum versionStatus ( const Dependency & value )
    {
        const Info & build = value.info;
        const Info & runtime = value.version.info;

        if ( build.revision != runtime.revision )
            return Status::Invalid;

        Status::Enum result = versionStatus( value.version );
        if ( result != Status::Valid )
            return result;

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

    /*!
     * Returns status of module version.
     *
     * Searches all status of module dependencies and returnds worst of them.
     */
    inline Status::Enum versionStatus ( const Version & value )
    {
        Status::Enum result = Status::Valid;

        // old C++ support
        for ( Dependencies::const_iterator iter = value.dependencies.cbegin();
            result != Status::Invalid && iter != value.dependencies.cend(); ++iter )
        {
            result = ::std::min( result, versionStatus( *iter ) );
        }

        return result;
    }
}

#endif
