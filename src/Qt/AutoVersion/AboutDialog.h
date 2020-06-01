#pragma once
#ifndef AUTO_VERSION_ABOUT_DIALOG_H
#define AUTO_VERSION_ABOUT_DIALOG_H

#include "AboutWidget.h"
#include <QDialog>

namespace Ui { class AboutWidget; }

namespace AutoVersion
{
    class AboutDialog
        : public QDialog
    {
        Q_OBJECT
        Q_DISABLE_COPY( AboutDialog )

    private:
        typedef AboutDialog ThisType;
        typedef QDialog ParentType;

    private:
        explicit AboutDialog ( QWidget * const product, QWidget * const parent, Qt::WindowFlags flags = Qt::WindowFlags() );

    public:
        static void execute ( QWidget * const parent, Qt::WindowFlags flags = Qt::WindowFlags() );
        static void execute ( QWidget * const product, QWidget * const parent, Qt::WindowFlags flags = Qt::WindowFlags() );
    };
}

#endif
