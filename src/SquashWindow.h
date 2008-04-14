/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#ifndef SQUASH_MAINWINDOW
#define SQUASH_MAINWINDOW

#include <QtGui>

class ImagesView;
class ImagesModel;
class QSettings;

class SquashWindow : public QMainWindow
{
    Q_OBJECT

    public:
        ~SquashWindow() { }
        static SquashWindow *instance()
        {
            if( !s_instance )
                s_instance = new SquashWindow();
            return s_instance;
        }

        void setStatusBarText( const QString &text );
        void resetAddImagesButton();
        void resetResizeImagesButton();

        double  resizeWidth()      const { return m_resizeX->value(); }
        double  resizeHeight()     const { return m_resizeY->value(); }
        QString saveDirectory()    const { return m_saveDirectory->text(); }
        QString fileSuffix()       const { return m_fileSuffix->text(); }
        bool    overwrite()        const { return m_overwriteFiles->checkState() == Qt::Checked; }
        int     resizeMethod()     const { return m_resizeMethod; }

    protected:
        virtual void closeEvent( QCloseEvent *event );
        virtual void dragEnterEvent( QDragEnterEvent *event );
        virtual void dropEvent( QDropEvent *event );
        virtual void keyPressEvent( QKeyEvent *event );

    private slots:
        void addImages();
        void resizeImages();
        void resizeMethodChanged( int index );
        void resizeWidthChanged( double width );
        void resizeHeightChanged( double height );

        void actionStatusSetter();

        void chooseSaveDirectory();

    private:
        SquashWindow( QWidget *parent = 0, Qt::WindowFlags flags = 0 );

        static SquashWindow *s_instance;

        void createToolBar();

        QString getSettingsPath();
        void readSettings();
        void writeSettings();

        void enableSettings( const bool enable );
        void modifyResizeMethods( bool showWidth, bool showHeight, bool isPixel );

        void addImages( QStringList images );
        bool validUrls( QList<QUrl> list );

        ImagesModel *m_imagesModel;
        ImagesView  *m_imageView;
        QToolBar    *m_toolBar;

        /// SIZE SETTINGS

        enum ResizeScale
        {
            PERCENT,
            PIXEL,
            WIDTH,
            HEIGHT,
            MAX
        };
        int             m_resizeMethod;

        QComboBox      *m_resizeCombo;
        QDoubleSpinBox *m_resizeX; // new width of images
        QDoubleSpinBox *m_resizeY; // new height of images
        QLabel         *m_resizeXLabel;
        QLabel         *m_resizeYLabel;
        QPushButton    *m_aspectLock; // lock the aspect ratio

        /// STORE SETTINGS

        QLineEdit   *m_saveDirectory;
        QPushButton *m_directoryChooser;
        QCheckBox   *m_overwriteFiles;
        QLineEdit   *m_fileSuffix;

        /// ADD IMAGES ACTION

        QAction   *m_addImages;
        bool       m_stopImageAdd;
        QByteArray m_dialogSettings;

        /// RESIZE IMAGES ACTION

        QAction   *m_resizeImages;
        bool       m_stopImageResize;
};

#endif

