#ifndef ABOUT_WIDGET_H
#define ABOUT_WIDGET_H

#ifdef AUTO_VERSION_USED
#include <AutoVersion/Base.h>
#endif

#include <QDialog>
#include <QTreeWidget>

namespace Ui { class AboutWidget; }

namespace AutoVersion
{
    class AboutWidget
        : public QWidget
    {
        Q_OBJECT
        Q_DISABLE_COPY( AboutWidget )

    private:
        typedef AboutWidget ThisType;
        typedef QWidget ParentType;

        typedef QScopedPointer< ::Ui::AboutWidget > AboutWidgetScoped;

    private:
        AboutWidgetScoped m_ui;

    public:
        explicit AboutWidget ( QWidget * const parent, Qt::WindowFlags flags = Qt::WindowFlags() );
        virtual ~AboutWidget ();
    };
}

namespace AutoVersion
{
    class AboutDialog
        : public QDialog
    {
        Q_OBJECT
        Q_DISABLE_COPY( AboutDialog )

    private:
        typedef AboutWidget ThisType;
        typedef QDialog ParentType;

    public:
        explicit AboutDialog ( QWidget * const parent, Qt::WindowFlags flags = Qt::WindowFlags() );
    };
}

#endif
