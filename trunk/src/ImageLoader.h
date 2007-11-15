/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#ifndef SQUASH_IMAGELOADER
#define SQUASH_IMAGELOADER

#include <QMutex>
#include <QSize>
#include <QStringList>
#include <QThread>
#include <QWaitCondition>

class QImage;
class QIcon;

class ImageLoader : public QThread
{
    Q_OBJECT

    public:
        ImageLoader( QObject *parent = 0 );
       ~ImageLoader();

        void init();
        void load( QString filename );
        void cancel();

    signals:
        void imageLoaded( const QString &filename, const QImage &thumbnail, const QString &description );
        void imageLoadFailed( const QString &filename );

    protected:
        void run();

    private:
        QMutex          m_mutex;
        QWaitCondition  m_condition;

        bool            m_abort;
        int             m_failCount;

        QStringList     m_fileList;
};

#endif
