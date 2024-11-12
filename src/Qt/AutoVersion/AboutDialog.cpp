#include <QVBoxLayout>
#include "AboutDialog.h"

namespace AutoVersion
{
    AboutDialog::AboutDialog ( QWidget * const product, QWidget * const parent, Qt::WindowFlags flags )
        : ParentType( parent, flags )
    {
        setLayout( new QVBoxLayout( this ) );
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        layout()->setContentsMargins(QMargins());
#else
        layout()->setMargin(0);
#endif
        AboutWidget * about_widget = new AboutWidget( this );
        layout()->addWidget( about_widget );

        if ( product )
            about_widget->setProductWidget( product );

        if ( parent )
            setWindowTitle( tr( "About program \"%1\"" ).arg( parent->windowTitle() ) );
        else
            setWindowTitle( tr( "About program" ) );
    }

    AboutDialog::AboutDialog ( QWidget * const product, QWidget * const parent, const QString & info, Qt::WindowFlags flags )
        : ParentType( parent, flags )
    {
        setLayout( new QVBoxLayout( this ) );
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        layout()->setContentsMargins(QMargins());
#else
        layout()->setMargin(0);
#endif
        AboutWidget * about_widget = new AboutWidget( this );
        layout()->addWidget( about_widget );

        if ( product )
            about_widget->setProductWidget( product );

        if ( parent )
            setWindowTitle( tr( "About program \"%1\"" ).arg( parent->windowTitle() ) );
        else
            setWindowTitle( tr( "About program" ) );

        about_widget->setProductInformation( info );
    }

    void AboutDialog::execute ( QWidget * const parent, Qt::WindowFlags flags )
    {
        AboutDialog::execute( parent, QString(), flags );
    }

    void AboutDialog::execute ( QWidget * const parent, const QString & info, Qt::WindowFlags flags )
    {
        typedef QWidget * WidgetPrt;
        AboutDialog::execute( WidgetPrt(), parent, info, flags );
    }

    void AboutDialog::execute ( QWidget * const product, QWidget * const parent, Qt::WindowFlags flags )
    {
        AboutDialog::execute( product, parent, QString(), flags );
    }

    void AboutDialog::execute ( QWidget * const product, QWidget * const parent, const QString & info, Qt::WindowFlags flags )
    {
        AboutDialog dialog( product, parent, info, flags );
        dialog.exec();
    }
}
