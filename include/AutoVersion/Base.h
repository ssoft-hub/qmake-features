#pragma once
#ifndef AUTO_VERSION_BASE
#define AUTO_VERSION_BASE

#include <cstring>
#include <list>
#include <map>
#include <set>
#include <string>
#include <ostream>

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
        typedef ::std::wstring Attribute;
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

    struct Module
    {
        typedef ::std::wstring Alias;
        typedef ::std::set< Alias > Aliases;

        Info m_info;
        Aliases m_aliases;

        Module () : m_info() , m_aliases() { }

        static Alias alias ( char * value ) { return Alias( value, value + strlen( value ) ); }
        static Alias alias ( const char * value ) { return Alias( value, value + strlen( value ) ); }
        static Alias alias ( wchar_t * value ) { return Alias( value, value + wcslen( value ) ); }
        static Alias alias ( const wchar_t * value ) { return Alias( value, value + wcslen( value ) ); }
        template < typename _Container > static Alias alias ( const _Container & container ) { return Alias( container.begin(), container.end() ); }
    };
}

namespace AutoVersion
{
    struct CompileVersion
    {
        typedef ::AutoVersion::Info Info;
        typedef::AutoVersion::CompileVersion Dependency;
        typedef ::std::list< Dependency > Dependencies;
        typedef Dependencies * OptionalDependencies;
        typedef OptionalDependencies (*DependenciesMethod) ();

        Info m_info;
        OptionalDependencies m_dependencies;

        CompileVersion () : m_info(), m_dependencies() {}
    };

    struct RuntimeVersion
    {
        typedef ::AutoVersion::Module Module;
        typedef ::std::map< Module::Alias, Module > Modules;
        Modules m_modules;
    };

    struct Version
    {
        CompileVersion m_compile;
        RuntimeVersion m_runtime;
    };

    struct VersionView
    {
        const CompileVersion & m_compile;
        const RuntimeVersion & m_runtime;

        VersionView ( const CompileVersion & compile, const RuntimeVersion & runtime )
            : m_compile( compile ), m_runtime( runtime )
        {}
    };
}

namespace AutoVersion
{
    /*!
     * Status enumeration.
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
            || compile[ Info::key( "revision.moment" ) ] != runtime[ Info::key( "revision.moment" ) ] )
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
    inline Status::Enum versionStatus ( const VersionView & version )
    {
        typedef const Info & InfoRef;
        typedef const Info * InfoPtr;

        const CompileVersion & compile = version.m_compile;
        const RuntimeVersion & runtime = version.m_runtime;

        InfoRef compile_info_ref = compile.m_info;
        RuntimeVersion::Modules::const_iterator iter = runtime.m_modules.find( compile_info_ref[ Info::key( "product" ) ] );
        InfoPtr runtime_info_ptr = iter != runtime.m_modules.end() ? &(iter->second.m_info) : InfoPtr();

        Status::Enum result = Status::Undefined;
        if ( runtime_info_ptr )
            result = infoStatus( compile_info_ref, *runtime_info_ptr );
        else if ( compile_info_ref[ Info::key( "compile.mode" ) ] == Info::attribute( "static" ) )
            result = Status::Valid;

        // C++98 support
        if ( compile.m_dependencies )
        {
            for ( CompileVersion::Dependencies::const_iterator iter = compile.m_dependencies->begin();
                  result != Status::Invalid && iter != compile.m_dependencies->end(); ++iter )
            {
                result = ::std::min( result, versionStatus( VersionView( *iter/*->m_version_refer*/, runtime ) ) );
            }
        }

        return result;
    }

    inline Status::Enum versionStatus ( const Version & version )
    {
        return versionStatus( VersionView( version.m_compile, version.m_runtime ) );
    }
}

namespace AutoVersion
{
    template < typename _Type >
    struct Block
    {
        typedef _Type Value;
        const Value & m_value;
        Block ( const Value & value ) : m_value( value ) {}
    };

    template < typename _Key, typename _Value >
    struct Record
    {
        typedef _Key Key;
        typedef _Value Value;
        const Key & m_key;
        const Value & m_value;
        Record ( const Key & key, const Value & value ) : m_key( key ), m_value( value ) {}
    };

    template < typename _Type >
    Block< _Type > block ( const _Type & value )
    {
        return Block< _Type >( value );
    }

    template < typename _Key, typename _Value >
    Record< _Key, _Value > record ( const _Key & key, const _Value & value )
    {
        return Record< _Key, _Value >( key, value );
    }

    template < typename _Stream >
    struct StringOutHelper
    {
        typedef _Stream Stream;
        Stream & m_stream;

        StringOutHelper ( Stream & stream ) : m_stream( stream ) {}
        void output ( const char * value ) { m_stream << value; }
        void output ( const wchar_t * value ) { m_stream << value; }
        void output ( const ::std::string & value ) { m_stream << value; }
        void output ( const ::std::wstring & value ) { m_stream << value; }
    };

    template <>
    struct StringOutHelper< ::std::ostream >
    {
        typedef ::std::ostream Stream;
        Stream & m_stream;

        StringOutHelper ( Stream & stream ) : m_stream( stream ) {}
        void output ( const char * value ) { m_stream << value; }
        void output ( const wchar_t * value ) { m_stream << ::std::string( value, value + wcslen( value ) ); }
        void output ( const ::std::string & value ) { m_stream << value; }
        void output ( const ::std::wstring & value ) { m_stream << ::std::string( value.begin(), value.end() ); }
    };

    template <>
    struct StringOutHelper< ::std::wostream >
    {
        typedef ::std::wostream Stream;
        Stream & m_stream;

        StringOutHelper ( Stream & stream ) : m_stream( stream ) {}
        void output ( const char * value ) { m_stream << ::std::wstring( value, value + strlen( value ) ); }
        void output ( const wchar_t * value ) { m_stream << value; }
        void output ( const ::std::string & value ) { m_stream << ::std::wstring( value.begin(), value.end() ); }
        void output ( const ::std::wstring & value ) { m_stream << value; }
    };

    template < typename _Stream >
    inline StringOutHelper< _Stream > stringOutHelper( _Stream & stream )
    {
        return StringOutHelper< _Stream >( stream );
    }

    inline StringOutHelper< ::std::ostream > stringOutHelper( ::std::ostream & stream )
    {
        return StringOutHelper< ::std::ostream >( stream );
    }

    inline StringOutHelper< ::std::wostream > stringOutHelper( ::std::wostream & stream )
    {
        return StringOutHelper< ::std::wostream >( stream );
    }

    template < typename _Stream >
    class FormatedOutStream
    {
        typedef FormatedOutStream ThisType;
        typedef _Stream Stream;

    public:
        struct Format
        {
            size_t m_tab_count;
            ::std::string m_tab_fill;
            ::std::string m_value_fill;
            ::std::string m_block_begin;
            ::std::string m_block_end;

            Format ()
                : m_tab_count()
                , m_tab_fill( "    " )
                // TODO:
                //, m_block_pattern( "$${tab}{\n$${block}\n$${tab}}\n" )
                //, m_record_pattern( "$${tab}$${key}: $${value}\n" )
                , m_value_fill( ": " )
                , m_block_begin( "{" )
                , m_block_end( "}" )
            {}
        };

    public:
        Stream & m_stream;
        Format m_format;

        FormatedOutStream ( Stream & stream )
            : m_stream( stream )
            , m_format()
        {}

        ThisType & operator << ( const char * value )
        {
            if ( value )
                stringOutHelper( m_stream ).output( value );
            return *this;
        }

        ThisType & operator << ( const wchar_t * value )
        {
            if ( value )
                stringOutHelper( m_stream ).output( value );
            return *this;
        }

        ThisType & operator << ( const ::std::string & value )
        {
            if ( !value.empty() )
                stringOutHelper( m_stream ).output( value );
            return *this;
        }

        ThisType & operator << ( const ::std::wstring & value )
        {
            if ( !value.empty() )
                stringOutHelper( m_stream ).output( value );
            return *this;
        }

        template < typename _Type >
        ThisType & operator << ( const _Type & value )
        {
            m_stream << value;
            return *this;
        }

        template < typename _Key, typename _Value >
        ThisType & operator << ( const ::AutoVersion::Record< _Key, _Value > & value )
        {
            for ( size_t i = 0; i < m_format.m_tab_count; ++i )
                *this << m_format.m_tab_fill;

            *this
                << value.m_key
                << m_format.m_value_fill
                << value.m_value
                << '\n';
            return *this;
        }

        template < typename _Type >
        ThisType & operator << ( const ::AutoVersion::Block< _Type > & value )
        {
            for ( size_t i = 0; i < m_format.m_tab_count; ++i )
                *this << m_format.m_tab_fill;
            *this
                << m_format.m_block_begin
                << '\n';

            ++m_format.m_tab_count;
            *this
                << value.m_value;

            --m_format.m_tab_count;
            for ( size_t i = 0; i < m_format.m_tab_count; ++i )
                *this << m_format.m_tab_fill;

            *this
                << m_format.m_block_end
                << '\n';

            return *this;
        }    };

    template < typename _Stream >
    FormatedOutStream< _Stream > formatedOutStream ( _Stream & stream )
    {
        return FormatedOutStream< _Stream >( stream );
    }

    template < typename _Stream >
    FormatedOutStream< _Stream > & formatedOutStream ( FormatedOutStream< _Stream > & stream )
    {
        return stream;
    }
}

template < typename _Stream >
#if ( __cplusplus >= 1201103L )
inline _Stream & operator << ( _Stream && stream, ::AutoVersion::Status::Enum value )
{
#else
inline _Stream & operator << ( _Stream & stream, ::AutoVersion::Status::Enum value )
{
#endif
    switch ( value )
    {
    case ::AutoVersion::Status::Invalid: stream << "Invalid"; break;
    case ::AutoVersion::Status::Different: stream << "Different"; break;
    case ::AutoVersion::Status::Valid: stream << "Valid"; break;
    case ::AutoVersion::Status::Undefined: stream << "Undefined"; break;
    }
    return stream;
}

template < typename _Stream >
#if ( __cplusplus >= 1201103L )
inline _Stream & operator << ( _Stream && stream, const ::AutoVersion::Info & value )
{
#else
inline _Stream & operator << ( const _Stream & cstream, const ::AutoVersion::Info & value )
{
    _Stream & stream = const_cast< _Stream & >( cstream );
#endif
    for ( ::AutoVersion::Info::Attributes::const_iterator iter = value.m_attributes.begin();
          iter != value.m_attributes.end(); ++iter )
    {
        ::AutoVersion::formatedOutStream( stream )
            << ::AutoVersion::record( iter->first, iter->second );
    }
    return stream;
}

template < typename _Stream >
#if ( __cplusplus >= 1201103L )
inline _Stream & operator << ( _Stream && stream, const ::AutoVersion::RuntimeVersion & value )
{
#else
inline _Stream & operator << ( const _Stream & cstream, const ::AutoVersion::RuntimeVersion & value )
{
    _Stream & stream = const_cast< _Stream & >( cstream );
#endif
    for ( ::AutoVersion::RuntimeVersion::Modules::const_iterator iter = value.m_modules.begin();
          iter != value.m_modules.end(); ++iter )
    {
        ::AutoVersion::formatedOutStream( stream )
            << ::AutoVersion::record( "Module", iter->first )
            << ::AutoVersion::block( iter->second.m_info );
    }
    return stream;
}

template < typename _Stream >
#if ( __cplusplus >= 1201103L )
inline _Stream & operator << ( _Stream && stream, const ::AutoVersion::CompileVersion & value )
{
#else
inline _Stream & operator << ( const _Stream & cstream, const ::AutoVersion::CompileVersion & value )
{
    _Stream & stream = const_cast< _Stream & >( cstream );
#endif
    ::AutoVersion::formatedOutStream( stream )
        << ::AutoVersion::block( value.m_info )
        << ::AutoVersion::record( "Dependencies", "" );
    if ( value.m_dependencies )
    {
        for ( ::AutoVersion::CompileVersion::Dependencies::const_iterator iter = value.m_dependencies->begin();
              iter != value.m_dependencies->end(); ++iter )
        {
            ::AutoVersion::formatedOutStream( stream )
                << ::AutoVersion::block( *iter );
        }
    }
    return stream;
}

template < typename _Stream >
#if ( __cplusplus >= 1201103L )
inline _Stream & operator << ( _Stream && stream, const ::AutoVersion::VersionView & value )
{
#else
inline _Stream & operator << ( const _Stream & cstream, const ::AutoVersion::VersionView & value )
{
    _Stream & stream = const_cast< _Stream & >( cstream );
#endif

    const ::AutoVersion::CompileVersion & compile = value.m_compile;
    const ::AutoVersion::RuntimeVersion & runtime = value.m_runtime;

    ::AutoVersion::formatedOutStream( stream )
       << ::AutoVersion::record( "Product", compile.m_info[ ::AutoVersion::Info::key( "product" ) ] )
       << ::AutoVersion::record( "Status", ::AutoVersion::versionStatus( value ) )
       << ::AutoVersion::block( compile.m_info );

    ::AutoVersion::formatedOutStream( stream )
        << ::AutoVersion::record( "Dependencies", "" );
    if ( compile.m_dependencies )
    {
        for ( ::AutoVersion::CompileVersion::Dependencies::const_iterator iter = compile.m_dependencies->begin();
              iter != compile.m_dependencies->end(); ++iter )
        {
            ::AutoVersion::formatedOutStream( stream )
                << ::AutoVersion::block( ::AutoVersion::VersionView( *iter, runtime ) );
        }
    }
    return stream;
}

template < typename _Stream >
#if ( __cplusplus >= 1201103L )
inline _Stream & operator << ( _Stream && stream, const ::AutoVersion::Version & value )
{
#else
inline _Stream & operator << ( const _Stream & cstream, const ::AutoVersion::Version & value )
{
    _Stream & stream = const_cast< _Stream & >( cstream );
#endif
    ::AutoVersion::formatedOutStream( stream )
        << ::AutoVersion::record( "Version", "" )
        << ::AutoVersion::block( ::AutoVersion::VersionView( value.m_compile, value.m_runtime ) )
        << ::AutoVersion::record( "Modules", "" )
        << ::AutoVersion::block( value.m_runtime );
    return stream;
}

//class QDebug;
//class QTextStream;

//namespace AutoVersion
//{
//    template <>
//    struct StringOutHelper< QDebug >
//    {
//        typedef QDebug Stream;
//        QDebug & m_stream;

//        StringOutHelper ( Stream & stream ) : m_stream( stream ) {}
//        void output ( const char * value ) { m_stream << QString::fromLocal8Bit( value ); }
//        void output ( const wchar_t * value ) { m_stream << QString::fromWCharArray( value ); }
//        void output ( const ::std::string & value ) { m_stream << QString::fromStdString( value ); }
//        void output ( const ::std::wstring & value ) { m_stream << QString::fromStdWString( value ); }
//    };
//}


#endif
