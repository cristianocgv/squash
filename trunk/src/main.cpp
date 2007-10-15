/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/


#include "SqueezeWindow.h"

#include <QApplication>
#include <QCoreApplication>

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    QTranslator qtTranslator;
    qtTranslator.load( "qt_" + QLocale::system().name() );
    app.installTranslator( &qtTranslator );

    QTranslator squeezeTranslator;
    squeezeTranslator.load( "tr/squeeze_" + QLocale::system().name() );
    app.installTranslator( &squeezeTranslator );

    QCoreApplication::setOrganizationName("SebRuiz");
    QCoreApplication::setOrganizationDomain("sebruiz.net");
    QCoreApplication::setApplicationName("Squeeze");

    SqueezeWindow::instance()->show();

    return app.exec();
}

