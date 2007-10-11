/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#ifndef SQUEEZE_MAINWINDOW
#define SQUEEZE_MAINWINDOW

#include <QtGui>

class ImagesView;
class ImagesModel;

class SqueezeWindow : public QMainWindow
{
    Q_OBJECT

    public:
       ~SqueezeWindow() { }
       static SqueezeWindow *instance()
       {
           if( !s_instance )
               s_instance = new SqueezeWindow();
           return s_instance;
       }

       void setStatusBarText( const QString &text );
       void resetAddImagesButton();
       void resetResizeImagesButton();

       double  widthPercentage()  const { return m_resizeX->value(); }
       double  heightPercentage() const { return m_resizeY->value(); }
       QString saveDirectory()    const { return m_saveDirectory->text(); }
       QString fileSuffix()       const { return m_fileSuffix->text(); }
       bool    overwrite()        const { return m_overwriteFiles->checkState() == Qt::Checked; }

    private slots:
        void addImages();
        void resizeImages();
        void resizeWidthChanged( double width );
        void resizeHeightChanged( double height );

        void actionStatusSetter();

        void chooseSaveDirectory();

    protected:
        void closeEvent( QCloseEvent *event );

    private:
        SqueezeWindow( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

        static SqueezeWindow *s_instance;

        void createToolBar();

        void readSettings();
        void writeSettings();

        ImagesModel *m_imagesModel;
        ImagesView  *m_imageView;
        QToolBar    *m_toolBar;

        /// ADD IMAGES ACTION

        QAction   *m_addImages;
        bool       m_stopImageAdd;

        /// SIZE SETTINGS

        QDoubleSpinBox *m_resizeX; // new width of images
        QDoubleSpinBox *m_resizeY; // new height of images
        QCheckBox      *m_aspectLock; // lock the aspect ratio

        /// STORE SETTINGS

        QLineEdit   *m_saveDirectory;
        QPushButton *m_directoryChooser;
        QCheckBox   *m_overwriteFiles;
        QLineEdit   *m_fileSuffix;

        /// RESIZE IMAGES ACTION

        QAction   *m_resizeImages;
        bool       m_stopImageResize;
};

#endif

