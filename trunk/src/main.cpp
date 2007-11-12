/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/


#include "SquashWindow.h"

#include <QApplication>
#include <QCoreApplication>

Q_IMPORT_PLUGIN(qjpeg)

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    QTranslator qtTranslator;
    qtTranslator.load( "qt_" + QLocale::system().name() );
    app.installTranslator( &qtTranslator );

    QTranslator squashTranslator;
    squashTranslator.load( "tr/squash_" + QLocale::system().name() );
    app.installTranslator( &squashTranslator );

    QCoreApplication::setOrganizationName("SebRuiz");
    QCoreApplication::setOrganizationDomain("sebruiz.net");
    QCoreApplication::setApplicationName("Squash");

    app.setWindowIcon( QIcon( ":/squash.png" ) );

    SquashWindow::instance()->show();

    return app.exec();
}

