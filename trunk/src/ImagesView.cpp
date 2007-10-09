/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#include "ImagesView.h"

#include <QDebug>
#include <QMenu>
#include <QModelIndex>
#include <QItemSelectionModel>

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
    qDebug() << "Here i should remove the selected images...";
}

void ImagesView::contextMenuEvent( QContextMenuEvent *e )
{
    QModelIndex current = indexAt( e->pos() );
    if( current.isValid() )
    {
        QItemSelectionModel *selectModel = selectionModel();
        selectModel->select( current, QItemSelectionModel::Select );
    }

    QModelIndexList indexes = selectedIndexes();

    if( indexes.isEmpty() )
        return;

    QMenu context( tr( "Actions" ), this );
    context.addAction( tr( "Remove %1 image(s)" ).arg( indexes.size() ), this, SLOT( removeSelectedImages() ) );

    context.exec( e->globalPos() );
}
