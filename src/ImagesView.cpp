/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#include "ImagesView.h"

ImagesView::ImagesView( QWidget *parent )
    : QListView( parent )
{
    setViewMode( QListView::IconMode );
    setMovement( QListView::Static );
    setResizeMode( QListView::Adjust );
    setWrapping( true );
    setIconSize( QSize(150, 150) );
    setGridSize( QSize(170, 170) );
    setSpacing( 10 );
}

void ImagesView::removeSelectedImages()
{
    QModelIndexList indexes = selectedIndexes();
}
