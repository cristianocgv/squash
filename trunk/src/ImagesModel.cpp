/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                      :     2005-2007 Trolltech - Puzzle Pieces Example  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#include <QtGui>
#include <QDebug>

#include "SqueezeWindow.h"
#include "ImagesModel.h"
#include "ImageLoader.h"

ImagesModel::ImagesModel(QObject *parent)
    : QAbstractListModel( parent )
    , m_loadCount( 0 )
    , m_resizeCount( 0 )
{
    connect( &m_imageLoader,  SIGNAL( imageLoaded( const QString &, const QImage &, const QString & ) ),
              this,             SLOT( imageLoaded( const QString &, const QImage &, const QString & ) ) );
    connect( &m_imageResizer, SIGNAL( imageResized( const QString & ) ),
              this,             SLOT( imageResized( const QString & ) ) );
}

QVariant ImagesModel::data( const QModelIndex &index, int role ) const
{
    if( !index.isValid() )
        return QVariant();

    if( role == Qt::DecorationRole )
        return m_thumbnails.at( index.row() );

    if( role == Qt::DisplayRole )
    {
        QFileInfo info( m_filenames.at( index.row() ) );
        QString description = info.fileName() + "\n" + m_descriptions.at( index.row() );
        return description;
    }

    if( role == Qt::UserRole )
        return m_filenames.at( index.row() );

    return QVariant();
}

Qt::ItemFlags ImagesModel::flags( const QModelIndex &index ) const
{
    if( index.isValid() )
        return ( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

    return Qt::ItemIsDropEnabled;
}

int ImagesModel::rowCount( const QModelIndex &parent ) const
{
    if( parent.isValid() ) return 0;
    return m_filenames.size();
}

bool ImagesModel::removeRows( int row, int count, const QModelIndex &parent )
{
    if( parent.isValid() )
        return false;

    if( row >= m_filenames.size() || row + count <= 0 )
        return false;

    int beginRow = qMax( 0, row );
    int endRow = qMin( row + count - 1, m_filenames.size() - 1 );

    beginRemoveRows( parent, beginRow, endRow );

    while( beginRow <= endRow )
    {
        m_thumbnails.removeAt( beginRow );
        m_filenames.removeAt( beginRow );
        m_descriptions.removeAt( beginRow );
        ++beginRow;
        emit imageRemoved();
    }

    endRemoveRows();
    return true;
}

void ImagesModel::addImage( const QString filename )
{
    m_imageLoader.load( filename );
}

void ImagesModel::addImages( const QStringList list )
{
    m_loadCount += list.size();
    SqueezeWindow::instance()->setStatusBarText( tr( "Loading %n image(s)...", "", m_loadCount ) );

    foreach( QString location, list )
        addImage( location );
}

void ImagesModel::imageLoaded( const QString &filename, const QImage &thumbnail, const QString &description )
{
    int row = m_filenames.size();

    beginInsertRows( QModelIndex(), row, row );

    // we must create the icon here and not in the thread because QWidget and friends aren't reentrant.
    QIcon iconThumb = QIcon( QPixmap::fromImage( thumbnail ) );

    m_filenames.insert( row, filename );
    m_thumbnails.insert( row, iconThumb );
    m_descriptions.insert( row, description );

    endInsertRows();

    m_loadCount--;
    QString text = tr( "Loading %n image(s)...", "", m_loadCount );
    if( m_loadCount <= 0 )
    {
        SqueezeWindow::instance()->resetAddImagesButton();
        text = tr( "%n image(s) loaded", "", m_filenames.size() );
    }
    SqueezeWindow::instance()->setStatusBarText( text );

    emit imageAdded();
}

void ImagesModel::stopAddImages()
{
    m_imageLoader.cancel();
    SqueezeWindow::instance()->setStatusBarText( tr( "%n image(s) loaded", "", m_filenames.size() ) );
}

void ImagesModel::resizeImages()
{
    m_resizeCount = 0;
    SqueezeWindow::instance()->setStatusBarText( tr( "Resizing %n image(s)...", "", m_resizeCount ) );
    m_imageResizer.init();
    foreach( QString file, m_filenames )
        m_imageResizer.load( file );
}

void ImagesModel::imageResized( const QString &filename )
{
    int row = m_filenames.indexOf( filename );
    if( row < 0 ) return;

    removeRows( row, 1, QModelIndex() );

    m_resizeCount++;
    QString text = tr( "Resizing %n image(s)...", "", m_filenames.size() );
    if( m_filenames.size() <= 0 )
    {
        SqueezeWindow::instance()->resetAddImagesButton();
        text = tr( "%n image(s) resized", "", m_resizeCount );
        m_resizeCount = 0;
    }
    SqueezeWindow::instance()->setStatusBarText( text );
}

void ImagesModel::stopResizeImages()
{
    m_imageResizer.cancel();
    SqueezeWindow::instance()->setStatusBarText( tr( "%n image(s) loaded", "", m_resizeCount ) );
}
