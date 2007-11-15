/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                      :     2005-2007 Trolltech - Puzzle Pieces Example  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#ifndef SQUASH_IMAGESMODEL
#define SQUASH_IMAGESMODEL

#include "ImageLoader.h"
#include "ImageResizer.h"

#include <QAbstractListModel>
#include <QIcon>
#include <QImage>
#include <QList>
#include <QPoint>

class ImagesModel : public QAbstractListModel
{
    Q_OBJECT

    public:
        ImagesModel( QObject *parent = 0 );
       ~ImagesModel() { }

        QVariant      data( const QModelIndex &index, int role = Qt::DisplayRole ) const;
        Qt::ItemFlags flags( const QModelIndex &index ) const;
        bool          removeRows( int row, int count, const QModelIndex &parent );

        int           rowCount( const QModelIndex &parent ) const;
        int           imageCount() const { return m_filenames.size(); }

        void          addImage( const QString filename );
        void          addImages( const QStringList list );
        void          stopAddImages();
        bool          isAdding() { return m_loadCount > 0; }

        void          resizeImages();
        void          stopResizeImages();
        bool          isResizing() { return m_resizeCount > 0; }

        void          removeSelectedImages();

    private slots:
        void          imageLoaded( const QString &filename, const QImage &thumbnail, const QString &description );
        void          imageLoadFailed( const QString &filename );
        void          imageResized( const QString &filename );

    signals:
        void          imageAdded();
        void          imageRemoved();
        void          imageAddFinished();
        void          imageResizeFinished();

    private:
        void          updateLoadStatus();

        QStringList   m_filenames;
        QList<QIcon>  m_thumbnails;
        QStringList   m_descriptions;

        int           m_loadCount;
        int           m_resizeCount;

        ImageLoader   m_imageLoader;
        ImageResizer  m_imageResizer;
};

#endif
