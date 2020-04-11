#include <QApplication>
#include <QFont>
#include <QIcon>
#include <QString>
#include <QVariant>
#include "VersionModel.h"

namespace AutoVersion
{
    static QString itemString ( const ::AutoVersion::Info::Key & key )
    {
        return QString::fromStdString( key );
    }

    static QString itemString ( const ::AutoVersion::Info::Attribute & key )
    {
        return QString::fromStdWString( key );
    }

    static QString itemImagePath ( ::AutoVersion::Status::Enum status )
    {
        switch ( status )
        {
        case ::AutoVersion::Status::Undefined: return QString::fromLatin1( ":AutoVersion/image/undefined.png" );
        case ::AutoVersion::Status::Valid: return QString::fromLatin1( ":AutoVersion/image/valid.png" );
        case ::AutoVersion::Status::Different: return QString::fromLatin1( ":AutoVersion/image/different.png" );
        case ::AutoVersion::Status::Invalid: return QString::fromLatin1( ":AutoVersion/image/invalid.png" );
        }
        return QString();
    }

    static QString itemToolTip (
        const ::AutoVersion::Info & compile_info,
        const ::AutoVersion::Info & runtime_info,
        const ::AutoVersion::Info::Key & key )
    {
        if ( key == "product" )
        {
            QString tool_tip;
            tool_tip += QString::fromLatin1( "<b>%1</b>" ).arg( VersionModel::trUtf8( "Cборка:" ) );
            tool_tip += QString::fromLatin1( "<table>" );
            for ( ::AutoVersion::Info::Attributes::const_iterator iter = compile_info.m_attributes.begin();
                iter != compile_info.m_attributes.end(); ++iter )
            {
                tool_tip += QString::fromLatin1( "<tr>" );
                tool_tip += QString::fromLatin1( "<td><i>%1:</i></td><td>\"%2\"</td>" )
                    .arg( itemString( iter->first ) )
                    .arg( itemString( iter->second ) );
                tool_tip += QString::fromLatin1( "</tr>" );
            }
            tool_tip += QString::fromLatin1( "</table>" );
            tool_tip += QString::fromLatin1( "<hr>" );

            tool_tip += QString::fromLatin1( "<b>%1</b>" ).arg( VersionModel::trUtf8( "Выполнение:" ) );
            tool_tip += QString::fromLatin1( "<table>" );
            for ( ::AutoVersion::Info::Attributes::const_iterator iter = runtime_info.m_attributes.begin();
                iter != runtime_info.m_attributes.end(); ++iter )
            {
                tool_tip += QString::fromLatin1( "<tr>" );
                tool_tip += QString::fromLatin1( "<td><i>%1:</i></td><td>\"%2\"</td>" )
                    .arg( itemString( iter->first ) )
                    .arg( itemString( iter->second ) );
                tool_tip += QString::fromLatin1( "</tr>" );
            }
            tool_tip += QString::fromLatin1( "</table>" );
            return tool_tip;
        }
        else if ( compile_info[ "compile.mode" ] == ::AutoVersion::Info::attribute( "static" ) )
        {
            return QString::fromLatin1( "<b>%1:</b> %2" )
                .arg( VersionModel::trUtf8( "Сборка:" ) )
                .arg( itemString( compile_info[ key ] ) );
        }

        return QString::fromLatin1( "<b>%1:</b> %2<br/><b>%3:</b> %4" )
            .arg( VersionModel::trUtf8( "Cборка:" ) )
            .arg( itemString( compile_info[ key ] ) )
            .arg( VersionModel::trUtf8( "Выполнение:" ) )
            .arg( itemString( runtime_info[ key ] ) );
    }

    static QString itemText (
        const ::AutoVersion::Info & compile_info,
        const ::AutoVersion::Info & runtime_info,
        const ::AutoVersion::Info::Key & key )
    {
        if ( compile_info[ "compile.mode" ] == ::AutoVersion::Info::attribute( "static" ) || runtime_info[ key ].empty() )
            return itemString( compile_info[ key ] );
        return itemString( runtime_info[ key ] );
    }

    static QFont itemFont (
        const ::AutoVersion::Info & compile_info,
        const ::AutoVersion::Info & runtime_info,
        const ::AutoVersion::Info::Key & key )
    {
        QFont font = QApplication::font();
        if ( compile_info[ "compile.mode" ] == ::AutoVersion::Info::attribute( "static" ) )
            font.setItalic( !font.italic() );
        else if ( !runtime_info.m_attributes.empty() && runtime_info[ key ] != compile_info[ key ] )
            font.setBold( !font.bold() );
        return font;
    }
}

namespace AutoVersion
{
    VersionModel::VersionModel ( const Version & version, QObject * const parent )
        : QAbstractItemModel( parent )
        , m_version( version )
        , m_root( m_version.m_compile )
    {
        init( m_root, m_version.m_compile );
    }

    void VersionModel::init ( Item & item, const CompileVersion & version )
    {
        RuntimeVersion::Modules::const_iterator iter
            = m_version.m_runtime.m_modules.find( version.m_info[ "product" ] );
        if ( iter != m_version.m_runtime.m_modules.end() )
            item.m_runtime_ptr = &iter->second;

        int local_row = int();
        if ( version.m_dependencies )
        {
            item.m_items.reserve( version.m_dependencies->size() );
            for ( CompileVersions::const_iterator iter = version.m_dependencies->begin();
                iter != version.m_dependencies->end(); ++iter )
            {
                const CompileVersion & dependency = *iter;
                item.m_items.push_back( Item( dependency, &item, local_row++ ) );
                init( item.m_items.back(), dependency );
            }
        }
    }

    QVariant VersionModel::data ( const QModelIndex & index, int role ) const
    {
        ItemPtr item_ptr = ItemPtr( index.internalPointer() );
        if ( !item_ptr || index.column() >= Column::Count )
            return QVariant();

        static const Info::Key keys[ Column::Count ] =
        {
            "product",
            "version",
            "revision",
            "revision.moment",
            "compile.moment",
            "vendor",
            "copyright",
            "license",
            "description"
        };

        static const ::AutoVersion::Info empty_info;
        const ::AutoVersion::Info & build_info = item_ptr->m_compile_ref.m_info;
        const ::AutoVersion::Info & runtime_info = item_ptr->m_runtime_ptr ? item_ptr->m_runtime_ptr->m_info : empty_info;

        switch ( role )
        {
        case Qt::DisplayRole:
            return QVariant::fromValue( itemText( build_info, runtime_info, keys[ index.column() ] ) );
        case Qt::ToolTipRole:
            return QVariant::fromValue( itemToolTip( build_info, runtime_info, keys[ index.column() ] ) );
        case Qt::FontRole:
            return QVariant::fromValue( itemFont( build_info, runtime_info, keys[ index.column() ] ) );
        case Qt::DecorationRole:
            if ( index.column() == Column::Product )
                return QVariant::fromValue( QIcon( itemImagePath( ::AutoVersion::versionStatus( ::AutoVersion::VersionView( item_ptr->m_compile_ref, m_version.m_runtime ) ) ) ) );
            break;
        default:
            break;
        }

        return QVariant();
    }

    Qt::ItemFlags VersionModel::flags ( const QModelIndex & /*index*/ ) const
    {
        return Qt::ItemFlags( Qt::ItemIsEnabled );
    }

    QVariant VersionModel::headerData ( int section, Qt::Orientation orientation, int role  ) const
    {
        static const QString column_header[ Column::Count ] =
        {
            trUtf8( "Product" ),
            trUtf8( "Version" ),
            trUtf8( "Revision" ),
            trUtf8( "Revision moment" ),
            trUtf8( "Compile moment" ),
            trUtf8( "Vendor" ),
            trUtf8( "Copyright" ),
            trUtf8( "License" ),
            trUtf8( "Description" )
        };

        switch ( orientation )
        {
        case Qt::Horizontal:
            {
                switch ( role )
                {
                case Qt::DisplayRole:
                    return section < Column::Count ? QVariant::fromValue( column_header[ section ] ) : QVariant();
                default:
                    break;
                }
            }
            break;
        case Qt::Vertical:
            break;
        }
        return QVariant();
    }

    QModelIndex VersionModel::index ( int row, int column, const QModelIndex & parent ) const
    {
        if ( !parent.isValid() )
        {
            if ( column < Column::Count )
                return createIndex( row, column, (void*)&m_root );
        }
        else if ( column < Column::Count )
        {
            ItemPtr item_ptr = ItemPtr( parent.internalPointer() );
            if ( item_ptr && row < int( item_ptr->m_items.size() ) )
            {
                Item::Items::const_iterator iter = item_ptr->m_items.begin();
                return createIndex( row, column, (void*)&*( iter + row ) );
            }
        }
        return QModelIndex();
    }

    QModelIndex VersionModel::parent ( const QModelIndex & child ) const
    {
        ItemPtr item_ptr = ItemPtr( child.internalPointer() );
        if ( !item_ptr || !item_ptr->m_parent )
            return QModelIndex();
        return createIndex( item_ptr->m_parent->m_local_row, Column::Product, item_ptr->m_parent );
    }

    int VersionModel::rowCount ( const QModelIndex & parent ) const
    {
        if ( !parent.isValid() )
        {
            return 1;
        }
        else
        {
            ItemPtr item_ptr = ItemPtr( parent.internalPointer() );
            if ( item_ptr /*&& parent.column() != Column::Product*/ )
                return int( item_ptr->m_items.size() );
        }
        return int();
    }

    int VersionModel::columnCount ( const QModelIndex & /*parent*/ ) const
    {
        return Column::Count;
    }
}
