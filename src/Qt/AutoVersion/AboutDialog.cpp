#include <QVBoxLayout>
#include "AboutDialog.h"

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
        typedef QWidget * WidgetPrt;
        AboutDialog::execute( WidgetPrt(), parent, flags );
    }

    void AboutDialog::execute ( QWidget * const product, QWidget * const parent, Qt::WindowFlags flags )
    {
        AboutDialog dialog( product, parent, flags );
        dialog.exec();
    }
}
