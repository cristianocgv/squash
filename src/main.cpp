/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/


#include "SqueezeWindow.h"

#include <QApplication>

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    SqueezeWindow::instance()->show();

    return app.exec();
}

