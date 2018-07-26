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
            return ( c_str == other.c_str )
                ? false
                : ( !c_str || !other.c_str )
                    ? true
                    : strcmp( c_str, other.c_str ) != 0;
        }

        bool operator < ( const String & other ) const
        {
            return ( c_str == other.c_str )
                ? false
                : ( !c_str || !other.c_str )
                    ? c_str < other.c_str
                    : strcmp( c_str, other.c_str ) < 0;
        }

        bool operator > ( const String & other ) const
        {
            return ( c_str == other.c_str )
                ? false
                : ( !c_str || !other.c_str )
                    ? c_str > other.c_str
                    : strcmp( c_str, other.c_str ) > 0;
        }
    };
}
namespace AutoVersion
{
    struct Info;

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

        inline bool operator == ( const Info & other ) const
        {
            return product == other.product
                &&  version == other.version
                &&  revision == other.revision
                &&  date == other.date
                &&  time == other.time
                &&  vendor == other.vendor
                &&  copyright == other.copyright
                &&  license == other.license
                &&  description == other.description;
        }

        inline bool operator < ( const Info & other ) const
        {
            if ( product < other.product )
                return true;
            if ( product > other.product )
                return false;

            if ( version < other.version )
                return true;
            if ( version > other.version )
                return false;

            if ( revision < other.revision )
                return true;
            if ( revision > other.revision )
                return false;

            if ( date < other.date )
                return true;
            if ( date > other.date )
                return false;

            if ( time < other.time )
                return true;
            if ( time > other.time )
                return false;

            if ( vendor < other.vendor )
                return true;
            if ( vendor > other.vendor )
                return false;

            if ( copyright < other.copyright )
                return true;
            if ( copyright > other.copyright )
                return false;

            if ( license < other.license )
                return true;
            if ( license > other.license )
                return false;

            //if ( description < other.description )
            //    return true;
            //if ( description > other.description )
            //    return false;

            //return false;

            return description < other.description;
        }
    };

    struct InfoPair
    {
        Info build;
        Info runtime;

        inline bool operator == ( const InfoPair & other ) const
        {
            return build == other.build
                && runtime == other.runtime;
        }

        inline bool operator < ( const InfoPair & other ) const
        {
            if ( build < other.build )
                return true;
            return build == other.build
                && runtime < other.runtime;
        }
    };

    typedef ::std::list< InfoPair > InfoPairs;
}

namespace AutoVersion
{
    struct Component
    {
        InfoPair info;
        InfoPairs dependencies;

        inline bool operator == ( const Component & other ) const
        {
            return info == other.info
                && dependencies == other.dependencies;
        }

        inline bool operator < ( const Component & other ) const
        {
            if ( info < other.info )
                return true;
            return info == other.info
                && dependencies < other.dependencies;
        }
    };

    typedef ::std::list< Component > Components;
}

namespace AutoVersion
{
    extern Components components ();
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

    inline Status::Enum statusOf ( const InfoPair & value )
    {
        if ( value.build.revision != value.runtime.revision )
            return Status::Invalid;
        if ( value.build == value.runtime )
            return Status::Valid;
        return Status::Different;
    }

    inline Status::Enum statusOf ( const Component & value )
    {
        Status::Enum result = statusOf( value.info );

        // old C++ support
        for ( InfoPairs::const_iterator iter = value.dependencies.cbegin();
            result != Status::Invalid && iter != value.dependencies.cend(); ++iter )
        {
            result = ::std::min( result, statusOf( *iter ) );
        }

        return result;
    }
}

#endif
