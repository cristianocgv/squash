/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#ifndef SQUEEZE_IMAGESVIEW
#define SQUEEZE_IMAGESVIEW

#include <QtGui>

class ImagesView : public QListView
{
    Q_OBJECT

    public:
        ImagesView( QWidget *parent = 0 );

    protected:
        void contextMenuEvent( QContextMenuEvent *e );

    private slots:
        void removeSelectedImages();
};

#endif
