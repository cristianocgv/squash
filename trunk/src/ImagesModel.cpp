/***************************************************************************
 * copyright            : (C) 2007-2008 Seb Ruiz <ruiz@kde.org>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#include <QtGui>
#include <QDebug>

#include "SquashWindow.h"
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
    connect( &m_imageLoader,  SIGNAL( imageLoadFailed( const QString & ) ),
              this,             SLOT( imageLoadFailed( const QString & ) ) );
    connect( &m_imageResizer, SIGNAL( imageResizeFailed( const QString &, const QString & ) ),
              this,             SLOT( imageResizeFailed( const QString &, const QString & ) ) );
}

QVariant ImagesModel::data( const QModelIndex &index, int role ) const
{
    if( !index.isValid() )
        return QVariant();

    if( role == Qt::DecorationRole )
        return m_thumbnails.at( index.row() );

    if( role == Qt::DisplayRole )
    {
        QString error = "\n";
        if( !m_errors.at( index.row() ).isEmpty() )
            error = tr( "Resize Failed" );
        QFileInfo info( m_filenames.at( index.row() ) );
        QString description = info.fileName() + "\n" + m_descriptions.at( index.row() );
        return description;
    }

    if( role == Qt::ToolTipRole )
        return m_errors.at( index.row() );

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
        m_errors.removeAt( beginRow );
        ++beginRow;
        emit imageRemoved();
    }
    updateLoadStatus();

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
    SquashWindow::instance()->setStatusBarText( tr( "Loading %n image(s)...", "", m_loadCount ) );
    m_imageLoader.init();

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
    m_errors.insert( row, QString() );

    endInsertRows();

    m_loadCount--;
    updateLoadStatus();

    emit imageAdded();
}

void ImagesModel::imageLoadFailed( const QString &filename )
{
    Q_UNUSED( filename );
    m_loadCount--;
    updateLoadStatus();
}

void ImagesModel::updateLoadStatus()
{
    QString text = tr( "Loading %n image(s)...", "", m_loadCount );
    if( m_loadCount <= 0 )
    {
        SquashWindow::instance()->resetAddImagesButton();
        text = tr( "%n image(s) loaded", "", m_filenames.size() );
    }
    SquashWindow::instance()->setStatusBarText( text );
}

void ImagesModel::stopAddImages()
{
    m_imageLoader.cancel();
    SquashWindow::instance()->setStatusBarText( tr( "%n image(s) loaded", "", m_filenames.size() ) );
}

void ImagesModel::resizeImages()
{
    m_resizeCount = 0;
    SquashWindow::instance()->setStatusBarText( tr( "Resizing %n image(s)...", "", m_resizeCount ) );
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
        SquashWindow::instance()->resetResizeImagesButton();
        text = tr( "%n image(s) resized", "", m_resizeCount );
        m_resizeCount = 0;
    }
    SquashWindow::instance()->setStatusBarText( text );
}

void ImagesModel::imageResizeFailed( const QString &filename, const QString &error )
{
    int row = m_filenames.indexOf( filename );
    if( row < 0 ) return;

    m_errors.replace( row, error );
}

void ImagesModel::stopResizeImages()
{
    m_imageResizer.cancel();
    SquashWindow::instance()->setStatusBarText( tr( "%n image(s) loaded", "", m_resizeCount ) );
}
