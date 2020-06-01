#pragma once
#ifndef AUTO_VERSION_ABOUT_WIDGET_H
#define AUTO_VERSION_ABOUT_WIDGET_H

#ifdef AUTO_VERSION_USED
#include <AutoVersion/Base.h>
#endif

#include <QScopedPointer>
#include <QWidget>

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
        QWidget * m_product_widget;

    public:
        explicit AboutWidget ( QWidget * const parent, Qt::WindowFlags flags = Qt::WindowFlags() );
        virtual ~AboutWidget ();

        void setProductWidget ( QWidget * widget );
        QWidget * productWidget ();
    };
}

#endif
