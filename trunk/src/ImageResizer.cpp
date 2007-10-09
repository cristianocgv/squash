/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#include <QtGui>
#include <QDebug>

#include "ImageResizer.h"
#include "SqueezeWindow.h"

ImageResizer::ImageResizer( QObject *parent )
    : QThread( parent )
    , m_abort( false )
{
}

ImageResizer::~ImageResizer()
{
    cancel();
}

void ImageResizer::init()
{
    QMutexLocker locker(&m_mutex);

    m_percentX      = SqueezeWindow::instance()->widthPercentage();
    m_percentY      = SqueezeWindow::instance()->heightPercentage();
    m_saveDirectory = QDir( SqueezeWindow::instance()->saveDirectory() );
    m_fileSuffix    = SqueezeWindow::instance()->fileSuffix();
    m_overwrite     = SqueezeWindow::instance()->overwrite();

    qDebug() << "save dir:    " << m_saveDirectory.absolutePath();
    qDebug() << "file suffix: " << m_fileSuffix;
    qDebug() << "w: " << m_percentX;
    qDebug() << "h: " << m_percentY;
}

void ImageResizer::load( QString filename )
{
    QMutexLocker locker(&m_mutex);

    m_fileList.append( filename );

    if( !isRunning() )
        start( LowPriority );
    else
        m_condition.wakeOne();
}

void ImageResizer::cancel()
{
    m_mutex.lock();

    m_abort = true;
    m_fileList.clear();
    m_condition.wakeOne();

    m_mutex.unlock();

    wait();
}

void ImageResizer::run()
{
    forever
    {
        m_mutex.lock();
        if( m_fileList.isEmpty() && !m_abort )
            m_condition.wait( &m_mutex );

        if( m_abort )
            return;

        QString filename = m_fileList.takeFirst();
        m_mutex.unlock();

        if( !m_saveDirectory.exists() )
            return;

        QFileInfo info( filename );
        QString scaledFilename = m_saveDirectory.filePath( info.baseName() + m_fileSuffix + "." + info.suffix() );

        QFile scaledFile( scaledFilename );

        // Yes, this logic can be compressed into four lines but this is very readable and logical
        if( scaledFile.exists() )
        {
            if( !m_overwrite )
            {
                emit imageResizeFailed( filename );
                continue;
            }
            else
            {
                bool removed = scaledFile.remove();
                if( !removed )
                {
                    emit imageResizeFailed( filename );
                    continue;
                }
            }
        }

        QImage image( filename );

        QSize originalSize = image.size();
        QSize scaledSize   = QSize( (int)( originalSize.width() * m_percentX / 100 ), (int)( originalSize.height() * m_percentY / 100 ) );

        image = image.scaled( scaledSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        qDebug() << QString( "Saving scaled image %1 to %2" ).arg( info.fileName(), scaledFilename );
        image.save( scaledFilename );

        emit imageResized( filename );
    }
}
