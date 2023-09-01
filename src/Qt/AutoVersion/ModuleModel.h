#pragma once
#ifndef AUTO_VERSION_MODULE_MODEL_H
#define AUTO_VERSION_MODULE_MODEL_H

#include <AutoVersion/Base.h>
#include <QAbstractItemModel>
#include <vector>

namespace AutoVersion
{
    class ModuleModel
        : public QAbstractItemModel
    {
    public:
        struct Column
        {
            enum Enum
            {
                Product,
                Version,
                Path,
                Vendor,
                Copyright,
                Count
            };
        };

        struct Item;
        typedef Item * ItemPtr;

        typedef ::AutoVersion::Info Info;
        typedef ::AutoVersion::Version Version;
        typedef ::AutoVersion::RuntimeVersion RuntimeVersion;
        typedef RuntimeVersion::Module Module;
        typedef const Module * ModulePtr;
        typedef const Module::Alias * AliasPtr;

        struct Item
        {
            typedef ::std::vector< Item > Items;

            ItemPtr m_parent;
            ModulePtr m_module_ptr;
            AliasPtr m_alias_ptr;
            int m_local_row;
            Items m_items;

            Item ( ModulePtr module_ptr )
                : m_parent(), m_module_ptr( module_ptr ), m_alias_ptr(), m_local_row(), m_items() {}
            Item ( AliasPtr alias_ptr, ItemPtr parent, int row  )
                : m_parent( parent ), m_module_ptr(), m_alias_ptr( alias_ptr ), m_local_row( row ), m_items() {}
        };
        typedef ::std::vector< Item > Items;

    private:
        const RuntimeVersion m_version;
        Items m_items;

    public:
        explicit ModuleModel ( const RuntimeVersion & version, QObject * const parent );
        virtual ~ModuleModel () {}

        virtual QVariant data ( const QModelIndex & index, int role ) const; // was pure
        virtual Qt::ItemFlags flags ( const QModelIndex & index ) const; // was pure

        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

        virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const; // was pure
        virtual QModelIndex parent ( const QModelIndex & child ) const; // was pure

        virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const; // was pure
        virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const; // was pure

        virtual bool insertRows ( int /*row*/, int /*count*/, const QModelIndex & /*parent*/ = QModelIndex() ) { return false; } // was pure
        virtual bool removeRows ( int /*row*/, int /*count*/, const QModelIndex & /*parent*/ = QModelIndex() ) { return false; } // was pure
    };
}

#endif
