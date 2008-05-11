/***************************************************************************
 * copyright            : (C) 2007-2008 Seb Ruiz <ruiz@kde.org>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#include <QtGui>
#include <QDebug>

#include "ImageResizer.h"
#include "SquashWindow.h"

ImageResizer::ImageResizer( QObject *parent )
    : QThread( parent )
    , m_resizeMethod( PERCENT )
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
    m_failCount = 0;
    m_resizeX       = SquashWindow::instance()->resizeWidth();
    m_resizeY       = SquashWindow::instance()->resizeHeight();
    m_saveDirectory = QDir( SquashWindow::instance()->saveDirectory() );
    m_fileSuffix    = SquashWindow::instance()->fileSuffix();
    m_overwrite     = SquashWindow::instance()->overwrite();
    m_resizeMethod  = SquashWindow::instance()->resizeMethod();
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
        if( m_fileList.size() == m_failCount && !m_abort )
            m_condition.wait( &m_mutex );

        if( m_abort )
            return;

        QString filename = m_fileList.takeAt( m_failCount );
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
                qDebug() << "Image write failed (would not overwrite)";
                QString error = tr( "Destination image exists" );
                m_mutex.lock();
                ++m_failCount;
                m_fileList.prepend( filename );
                m_mutex.unlock();
                emit imageResizeFailed( filename, error );
                continue;
            }
            else
            {
                bool removed = scaledFile.remove();
                if( !removed )
                {
                    qDebug() << "Image write failed (could not remove)";
                    QString error = tr( "Could not erase existing image" );
                    m_mutex.lock();
                    ++m_failCount;
                    m_fileList.prepend( filename );
                    m_mutex.unlock();
                    emit imageResizeFailed( filename, error );
                    continue;
                }
            }
        }

        QImage image( filename );

        QSize originalSize = image.size();
        QSize newSize;
        double percentage  = 0;

        switch( m_resizeMethod )
        {
            case PERCENT:
                newSize = QSize( (int)( originalSize.width() * m_resizeX / 100 ), (int)( originalSize.height() * m_resizeY / 100 ) );

            case WIDTH:
                percentage = m_resizeX / originalSize.width();
                newSize = QSize( (int)( originalSize.width() * percentage ), (int)( originalSize.height() * percentage ) );
                break;

            case HEIGHT:
                percentage = m_resizeY / originalSize.width();
                newSize = QSize( (int)( originalSize.width() * percentage ), (int)( originalSize.height() * percentage ) );
                break;

            case PIXEL:
                newSize = QSize( m_resizeX, m_resizeY );
                break;

            default:
                qDebug() << "Invalid resize method: " << m_resizeMethod;
                emit imageResizeFailed( filename, "Invalid resize operation" );
                continue;

        }

        image = image.scaled( newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
        qDebug() << QString( "Saving scaled image %1 to %2" ).arg( info.fileName(), scaledFilename );
        image.save( scaledFilename );

        emit imageResized( filename );
    }
}
