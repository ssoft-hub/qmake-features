#include "ModuleModel.h"

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

    static QString itemToolTip (
        const ::AutoVersion::Info & info )
    {
        QString tool_tip;
        tool_tip += QString::fromLatin1( "<b>%1:</b>" )
            .arg( itemString( info[ "product" ] ) );
        tool_tip += QString::fromLatin1( "<table>" );
        for ( ::AutoVersion::Info::Attributes::const_iterator iter = info.m_attributes.begin();
            iter != info.m_attributes.end(); ++iter )
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
}


namespace AutoVersion
{
    ModuleModel::ModuleModel ( const RuntimeVersion & version, QObject * const parent )
        : QAbstractItemModel( parent )
        , m_version( version )
    {
        m_items.reserve( m_version.m_modules.size() );
        for ( RuntimeVersion::Modules::const_iterator iter = m_version.m_modules.begin();
            iter != m_version.m_modules.end(); ++iter )
        {
            const Module & module = iter->second;
            m_items.push_back( Item( &module ) );
            Item & module_item = m_items.back();
            module_item.m_items.reserve( module.m_aliases.size() );
            int local_row = int();
            for ( Module::Aliases::const_iterator alias_iter = module.m_aliases.begin();
                alias_iter != module.m_aliases.end(); ++alias_iter )
            {
                module_item.m_items.push_back( Item( &*alias_iter, &module_item, local_row++ ) );
            }
        }
    }

    QVariant ModuleModel::data ( const QModelIndex & index, int role ) const
    {
        ItemPtr item_ptr = ItemPtr( index.internalPointer() );
        if ( !item_ptr )
            return QVariant();

        static const Info::Key keys[ Column::Count ] =
        {
            "product",
            "version",
            "path",
            "vendor",
            "copyright"
        };

        if ( item_ptr->m_module_ptr && index.column() < Column::Count )
        {
            const ::AutoVersion::Info & info = item_ptr->m_module_ptr->m_info;

            switch ( role )
            {
            case Qt::DisplayRole:
                return QVariant::fromValue( itemString( info[ keys[ index.column() ] ] ) );
            case Qt::ToolTipRole:
                return QVariant::fromValue( itemToolTip( info ) );
            default:
                break;
            }
        }

        if ( item_ptr->m_alias_ptr && index.column() == Column::Product )
        {
            const Module::Alias & alias = *item_ptr->m_alias_ptr;

            switch ( role )
            {
            case Qt::DisplayRole:
                return QVariant::fromValue( itemString( alias ) );
            case Qt::ToolTipRole:
                return QVariant::fromValue( itemString( alias ) );
            default:
                break;
            }
        }

        return QVariant();
    }

    Qt::ItemFlags ModuleModel::flags ( const QModelIndex & /*index*/ ) const
    {
        return Qt::ItemFlags( Qt::ItemIsEnabled );
    }

    QVariant ModuleModel::headerData ( int section, Qt::Orientation orientation, int role  ) const
    {
        static const QString column_header[ Column::Count ] =
        {
            tr( "Product" ),
            tr( "Version" ),
            tr( "Path" ),
            tr( "Vendor" ),
            tr( "Copyright" )
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

    QModelIndex ModuleModel::index ( int row, int column, const QModelIndex & parent ) const
    {
        if ( !parent.isValid() )
        {
            if ( column < Column::Count && size_t( row ) < m_items.size() )
                return createIndex( row, column, (void*)&m_items[ row ] );
        }
        else
        {
            ItemPtr item_ptr = ItemPtr( parent.internalPointer() );
            if ( item_ptr && column < Column::Count && size_t( row ) < item_ptr->m_items.size() )
                return createIndex( row, column, (void*)&item_ptr->m_items[ row ] );
        }
        return QModelIndex();
    }

    QModelIndex ModuleModel::parent ( const QModelIndex & child ) const
    {
        ItemPtr item_ptr = ItemPtr( child.internalPointer() );
        if ( !item_ptr || !item_ptr->m_parent )
            return QModelIndex();
        return createIndex( item_ptr->m_parent->m_local_row, Column::Product, item_ptr->m_parent );
    }

    int ModuleModel::rowCount ( const QModelIndex & parent ) const
    {
        if ( !parent.isValid() )
        {
            return int(m_items.size());
        }
        else
        {
            ItemPtr item_ptr = ItemPtr( parent.internalPointer() );
            if ( item_ptr /*&& parent.column() != Column::Product*/ )
                return int( item_ptr->m_items.size() );
        }
        return int();
    }

    int ModuleModel::columnCount ( const QModelIndex & /*parent*/ ) const
    {
        return Column::Count;
    }
}
