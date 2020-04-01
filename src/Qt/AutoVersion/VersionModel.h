#pragma once
#ifndef AUTO_VERSION_MODEL_H
#define AUTO_VERSION_MODEL_H

#include <AutoVersion/Base.h>
#include <QAbstractItemModel>

namespace AutoVersion
{
    class VersionModel
        : public QAbstractItemModel
    {
    public:
        struct Column
        {
            enum Enum
            {
                Product,
                Version,
                Revision,
                RevisionMoment,
                CompileMoment,
                Vendor,
                Copyright,
                License,
                Description,
                Count
            };
        };

        struct Item;
        typedef Item * ItemPtr;

        typedef ::AutoVersion::Info Info;
        typedef ::AutoVersion::Version Version;
        typedef ::AutoVersion::RuntimeVersion RuntimeVersion;
        typedef ::AutoVersion::CompileVersion CompileVersion;
        typedef CompileVersion::Dependencies CompileVersions;
        typedef const CompileVersion & CompileRef;
        typedef const RuntimeVersion::Module * RuntimePtr;

        struct Item
        {
            typedef ::std::vector< Item > Items;

            ItemPtr m_parent;
            CompileRef m_compile_ref;
            RuntimePtr m_runtime_ptr;
            int m_local_row;
            Items m_items;

            Item ( CompileRef compile, ItemPtr parent = ItemPtr(), int row = int() )
                : m_parent( parent ), m_compile_ref( compile ), m_runtime_ptr(), m_local_row( row ), m_items() {}
        };

    private:
        const Version m_version;
        Item m_root;

    public:
        explicit VersionModel ( const Version & version, QObject * const parent = 0 );
        virtual ~VersionModel () {}

        virtual QVariant data ( const QModelIndex & index, int role ) const; // was pure
        virtual Qt::ItemFlags flags ( const QModelIndex & index ) const; // was pure

        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

        virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const; // was pure
        virtual QModelIndex parent ( const QModelIndex & child ) const; // was pure

        virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const; // was pure
        virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const; // was pure

        virtual bool insertRows ( int /*row*/, int /*count*/, const QModelIndex & /*parent*/ = QModelIndex() ) { return false; } // was pure
        virtual bool removeRows ( int /*row*/, int /*count*/, const QModelIndex & /*parent*/ = QModelIndex() ) { return false; } // was pure

    private:
        void init ( Item & item, const CompileVersion & version );

    };
}

#endif
