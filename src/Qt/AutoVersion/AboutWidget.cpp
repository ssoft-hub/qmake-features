#include <AutoVersion/Application.h>
#include <Qt/AutoVersion/ModuleModel.h>
#include <Qt/AutoVersion/VersionModel.h>
#include <QSortFilterProxyModel>
#include "AboutWidget.h"
#include "ui_AboutWidget.h"

namespace AutoVersion
{
#ifdef AUTO_VERSION_USED
    static QString itemString ( const ::AutoVersion::Info::Attribute & key )
    {
        return QString::fromStdWString( key );
    }
#endif
}

namespace AutoVersion
{
    AboutWidget::AboutWidget ( QWidget * const parent, Qt::WindowFlags flags )
        : ParentType( parent, flags )
        , m_ui( new Ui::AboutWidget )
        , m_product_widget()
    {
        m_ui->setupUi( this );

    #ifdef AUTO_VERSION_USED
        ::AutoVersion::Version version = ::AutoVersion::version();
        QSortFilterProxyModel * version_sort_model = new QSortFilterProxyModel( this );
        QAbstractItemModel * version_model = new VersionModel( version, this );
        version_sort_model->setSourceModel( version_model );
        m_ui->version_tree_view->setModel( version_sort_model );

        QSortFilterProxyModel * module_sort_model = new QSortFilterProxyModel( this );
        QAbstractItemModel * module_model = new ModuleModel( version.m_runtime, this );
        module_sort_model->setSourceModel( module_model );
        m_ui->module_tree_view->setModel( module_sort_model );

        ////////////

        QFont name_font = font();
        name_font.setBold( true );
        m_ui->name_label->setFont( name_font );

        m_ui->name_label->setText( itemString( version.m_compile.m_info[ "product" ] ) );
        m_ui->version_label->setText( itemString( version.m_compile.m_info[ "version" ] ) );
        m_ui->note_label->setText( itemString( version.m_compile.m_info[ "description" ] ) );
        m_ui->revision_moment_label->setText( QString::fromLatin1( "%1" )
            .arg( itemString( version.m_compile.m_info[ "revision.moment" ] ) ) );
        m_ui->compile_moment_label->setText( QString::fromLatin1( "%1" )
            .arg( itemString( version.m_compile.m_info[ "compile.moment" ] ) ) );
        m_ui->revision_label->setText( itemString( version.m_compile.m_info[ "revision" ] ) );
        m_ui->license_label->setText( itemString( version.m_compile.m_info[ "license" ] ) );
        m_ui->vendor_label->setText( itemString( version.m_compile.m_info[ "vendor" ] ) );
        m_ui->copyright_label->setText( itemString( version.m_compile.m_info[ "copyright" ] ) );
    #endif
    }

    AboutWidget::~AboutWidget ()
    {
        // explicit destruct AboutWidgetScoped m_ui;
    }

    void AboutWidget::setProductWidget ( QWidget * widget )
    {
        if ( m_product_widget == widget )
            return;
        if ( m_product_widget )
            delete m_product_widget;
        m_product_widget = widget;
        m_product_widget->setParent( this, Qt::WindowFlags() );
        m_ui->product_layout->insertWidget( 0, m_product_widget );
    }
}

namespace AutoVersion
{
    AboutDialog::AboutDialog ( QWidget * const product, QWidget * const parent, Qt::WindowFlags flags )
        : ParentType( parent, flags )
    {
        setLayout( new QVBoxLayout( this ) );
        layout()->setMargin( 0 );
        AboutWidget * about_widget = new AboutWidget( this );
        layout()->addWidget( about_widget );

        if ( product )
            about_widget->setProductWidget( product );

        if ( parent )
            setWindowTitle( trUtf8( "About program \"%1\"" ).arg( parent->windowTitle() ) );
        else
            setWindowTitle( trUtf8( "About program" ) );

    }

    void AboutDialog::execute ( QWidget * const parent, Qt::WindowFlags flags )
    {
        AboutDialog::execute( 0, parent, flags );
    }

    void AboutDialog::execute ( QWidget * const product, QWidget * const parent, Qt::WindowFlags flags )
    {
        AboutDialog dialog( product, parent, flags );
        QSize size_hint = dialog.sizeHint();
        if ( 3 * size_hint.height() > 2 * size_hint.width()  )
        {
            size_hint.setWidth( size_hint.height() * 3 / 2 );
            size_hint.setHeight( dialog.minimumHeight() );
            dialog.resize( size_hint );
        }
        dialog.exec();
    }
}
