#pragma once
#ifndef AUTO_VERSION_STREAM
#define AUTO_VERSION_STREAM

#include <ostream>
#include "Base.h"

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
            ::std::string m_record_pattern;
            ::std::string m_block_pattern;

            Format ()
                : m_tab_count()
                , m_tab_fill( "    " )
                , m_record_pattern( "$${tab}$${key}: $${value}\n" )
                , m_block_pattern( "$${tab}{\n$${block}$${tab}}\n" )
            {}
        };

    public:
        Stream & m_stream;
        Format m_format;

        FormatedOutStream ( Stream & stream )
            : m_stream( stream )
            , m_format()
        {}

        void outputTab ()
        {
            for ( size_t i = 0; i < m_format.m_tab_count; ++i )
                *this << m_format.m_tab_fill;
        }

        void outputPattern ( const ::std::string & pattern, size_t begin, size_t end )
        {
            *this << ::std::string( pattern.begin() + begin, pattern.begin() + end );
        }

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

        template < typename _Key, typename _Value >
        ThisType & operator << ( const ::AutoVersion::Record< _Key, _Value > & value )
        {
            ::std::string::size_type index = ::std::string::size_type();

            do
            {
                ::std::string::size_type tab_index = m_format.m_record_pattern.find( "$${tab}", index );
                ::std::string::size_type key_index = m_format.m_record_pattern.find( "$${key}", index );
                ::std::string::size_type value_index = m_format.m_record_pattern.find( "$${value}", index );

                ::std::string::size_type begin_index = index;
                ::std::string::size_type end_index = ::std::min( tab_index, ::std::min( key_index, value_index ) );

                if ( end_index == ::std::string::npos )
                {
                    end_index = m_format.m_record_pattern.size();
                    index = ::std::string::npos;
                }
                else if ( end_index == tab_index )
                    index = end_index + 7; // $${tab}
                else if ( end_index == key_index )
                    index = end_index + 7; // $${key}
                else if ( end_index == value_index )
                    index = end_index + 9; // $${value}

                outputPattern( m_format.m_record_pattern, begin_index, end_index );
                if ( end_index == tab_index )
                    outputTab();
                if ( end_index == key_index )
                    *this << value.m_key;
                if ( end_index == value_index )
                    *this << value.m_value;
            }
            while ( index != ::std::string::npos );

            return *this;
        }

        template < typename _Type >
        ThisType & operator << ( const ::AutoVersion::Block< _Type > & value )
        {
            ::std::string::size_type index = ::std::string::size_type();

            do
            {
                ::std::string::size_type tab_index = m_format.m_block_pattern.find( "$${tab}", index );
                ::std::string::size_type block_index = m_format.m_block_pattern.find( "$${block}", index );

                ::std::string::size_type begin_index = index;
                ::std::string::size_type end_index = ::std::min( tab_index, block_index );

                if ( end_index == ::std::string::npos )
                {
                    end_index = m_format.m_block_pattern.size();
                    index = ::std::string::npos;
                }
                else if ( end_index == tab_index )
                    index = end_index + 7; // $${tab}
                else if ( end_index == block_index )
                    index = end_index + 9; // $${block}

                outputPattern( m_format.m_block_pattern, begin_index, end_index );
                if ( end_index == tab_index )
                    outputTab();
                if ( end_index == block_index )
                {
                    ++m_format.m_tab_count;
                    *this << value.m_value;
                    --m_format.m_tab_count;
                }
            }
            while ( index != ::std::string::npos );

            return *this;
        }
    };

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
#if ( __cplusplus >= 201103L )
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
#if ( __cplusplus >= 201103L )
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
#if ( __cplusplus >= 201103L )
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
#if ( __cplusplus >= 201103L )
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
#if ( __cplusplus >= 201103L )
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
#if ( __cplusplus >= 201103L )
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
