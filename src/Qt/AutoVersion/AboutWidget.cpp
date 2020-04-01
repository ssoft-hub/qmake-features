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
    {
        m_ui->setupUi( this );

        int pixmap_size = 128;//4 * name_font.pointSize();
        setWindowIcon( QIcon( QString::fromUtf8( ":/Images/Images/topaz-logo-trans.png" ) ) );
        m_ui->pixmap_label->setPixmap( QPixmap( ( QString::fromUtf8( ":/Images/Images/topaz-logo-trans.png" ) ) )
            .scaled( pixmap_size, pixmap_size, Qt::KeepAspectRatio ) );

        m_ui->version_tab_widget->setTabText( 0, trUtf8( "Дерево версий" ) );
        m_ui->version_tab_widget->setTabText( 1, trUtf8( "Модули" ) );

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
}

namespace AutoVersion
{
    AboutDialog::AboutDialog ( QWidget * const parent, Qt::WindowFlags flags )
        : ParentType( parent, flags )
    {
        setLayout( new QVBoxLayout( this ) );
        layout()->setMargin( 0 );
        layout()->addWidget( new AboutWidget( this ) );

        if ( parent )
            setWindowTitle( trUtf8( "О программе \"%1\"" ).arg( parent->windowTitle() ) );
        else
            setWindowTitle( trUtf8( "О программе" ) );
    }
}
