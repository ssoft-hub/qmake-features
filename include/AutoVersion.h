#pragma once
#ifndef AUTO_VERSION_STRUCTS
#define AUTO_VERSION_STRUCTS

#include <cstring>
#include <set>

namespace AutoVersion
{
    namespace Private
    {
        inline bool isEqual ( const char * right, const char * left )
        {
            return ( right == left )
                ? true
                : ( !right || !left )
                    ? false
                    : !strcmp( right, left );
        }

        inline bool isLess ( const char * right, const char * left )
        {
            return ( right == left )
                ? false
                : ( !right || !left )
                    ? right < left
                    : strcmp( right, left ) < 0;
        }

        inline bool isMore ( const char * right, const char * left )
        {
            return ( right == left )
                ? false
                : ( !right || !left )
                    ? right > left
                    : strcmp( right, left ) > 0;
        }
    }
}
namespace AutoVersion
{
    struct Info
    {
        const char * name;
        const char * version;
        const char * revision;
        const char * date;
        const char * time;
        const char * vendor;
        const char * copyright;
        const char * license;
        const char * description;

        Info ()
        : name()
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
            return ::AutoVersion::Private::isEqual( name, other.name )
                && ::AutoVersion::Private::isEqual( version, other.version )
                && ::AutoVersion::Private::isEqual( revision, other.revision )
                && ::AutoVersion::Private::isEqual( date, other.date )
                && ::AutoVersion::Private::isEqual( time, other.time )
                && ::AutoVersion::Private::isEqual( vendor, other.vendor )
                && ::AutoVersion::Private::isEqual( copyright, other.copyright )
                && ::AutoVersion::Private::isEqual( description, other.description );
        }

        inline bool operator < ( const Info & other ) const
        {
            if ( ::AutoVersion::Private::isLess( name, other.name ) )
                return true;
            if ( ::AutoVersion::Private::isMore( name, other.name ) )
                return false;

            if ( ::AutoVersion::Private::isLess( version, other.version ) )
                return true;
            if ( ::AutoVersion::Private::isMore( version, other.version ) )
                return false;

            if ( ::AutoVersion::Private::isLess( revision, other.revision ) )
                return true;
            if ( ::AutoVersion::Private::isMore( revision, other.revision ) )
                return false;

            if ( ::AutoVersion::Private::isLess( date, other.date ) )
                return true;
            if ( ::AutoVersion::Private::isMore( date, other.date ) )
                return false;

            if ( ::AutoVersion::Private::isLess( time, other.time ) )
                return true;
            if ( ::AutoVersion::Private::isMore( time, other.time ) )
                return false;

            if ( ::AutoVersion::Private::isLess( vendor, other.vendor ) )
                return true;
            if ( ::AutoVersion::Private::isMore( vendor, other.vendor ) )
                return false;

            if ( ::AutoVersion::Private::isLess( copyright, other.copyright ) )
                return true;
            if ( ::AutoVersion::Private::isMore( copyright, other.copyright ) )
                return false;

            if ( ::AutoVersion::Private::isLess( description, other.description ) )
                return true;
            if ( ::AutoVersion::Private::isMore( description, other.description ) )
                return false;

            return false;
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
            if ( build == other.build
                && runtime < other.runtime )
                    return true;
            return false;
        }
    };

    typedef ::std::set< InfoPair > InfoPairs;
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
            if ( info == other.info
                && dependencies < other.dependencies )
                    return true;
            return false;
        }
    };

    typedef ::std::set< Component > Components;
}

namespace AutoVersion
{
    extern  Components components ();
}

#endif
