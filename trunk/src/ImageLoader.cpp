/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#include <QtGui>
#include <QDebug>

#include "ImageLoader.h"

ImageLoader::ImageLoader( QObject *parent )
    : QThread( parent )
    , m_abort( false )
{
}

ImageLoader::~ImageLoader()
{
    cancel();
}

void ImageLoader::load( QString filename )
{
    QMutexLocker locker(&m_mutex);

    m_fileList.append( filename );

    if( !isRunning() )
        start( LowPriority );
    else
        m_condition.wakeOne();
}

void ImageLoader::init()
{
    QMutexLocker locker(&m_mutex);
    m_failCount = 0;
}

void ImageLoader::cancel()
{
    m_mutex.lock();

    m_abort = true;
    m_fileList.clear();
    m_condition.wakeOne();

    m_mutex.unlock();

    wait();
}

void ImageLoader::run()
{
    forever
    {
        m_mutex.lock();
        if( m_fileList.size() == m_failCount && !m_abort )
            m_condition.wait( &m_mutex );

        QString filename = m_fileList.takeFirst();
        m_mutex.unlock();

        if( m_abort )
            return;

        QImage image( filename );
        if( image.isNull() )
        {
            qDebug() << "Image " << filename << " failed to load";
            m_mutex.lock();
            ++m_failCount;
            m_fileList.prepend( filename );
            m_mutex.unlock();
            emit imageLoadFailed( filename );
            continue;
        }

        QImage scaled = image.scaled( 150, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation );

        QStringList suffixes; suffixes << "B" << "KB" << "MB" << "GB";
        QString suffix = suffixes.takeFirst();

        QFileInfo info( filename );
        double size = (double) info.size();

        while( size > 1024 && !suffixes.isEmpty() )
        {
            suffix = suffixes.takeFirst();
            size /= (double) 1024;
        }

        QString description = QString( "\n%1 %2" ).arg( QString::number( size, 'g', 3 ), suffix );

        emit imageLoaded( filename, scaled, description );
    }
}
