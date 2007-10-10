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
    setSelectionMode( QAbstractItemView::ExtendedSelection );
}

void ImagesView::removeSelectedImages()
{
    QModelIndexList indexes = selectedIndexes();

    if( indexes.isEmpty() )
        return;

    foreach( QModelIndex i, indexes )
        model()->removeRows( i.row(), 1, QModelIndex() );
}

void ImagesView::contextMenuEvent( QContextMenuEvent *event )
{
    QModelIndex current = indexAt( event->pos() );
    if( current.isValid() )
    {
        QItemSelectionModel *selectModel = selectionModel();
        selectModel->select( current, QItemSelectionModel::Select );
    }

    QModelIndexList indexes = selectedIndexes();

    if( indexes.isEmpty() )
        return;

    QMenu context( tr( "Actions" ), this );
    QString text = tr( "Remove %n image(s)", "", indexes.size() );
    context.addAction( text, this, SLOT( removeSelectedImages() ) );

    context.exec( event->globalPos() );
}

void ImagesView::keyPressEvent( QKeyEvent *event )
{
    if( event->matches( QKeySequence::Delete ) )
    {
        removeSelectedImages();
        return;
    }
    QListView::keyPressEvent( event );
}
