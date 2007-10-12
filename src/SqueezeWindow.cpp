/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#include "ImagesModel.h"
#include "ImagesView.h"
#include "SqueezeWindow.h"

#include <QDebug>
#include <QtGui>
#include <QSettings>

SqueezeWindow *SqueezeWindow::s_instance = 0;

SqueezeWindow::SqueezeWindow( QWidget *parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags )
    , m_stopImageAdd( false )
    , m_stopImageResize( false )
{
    s_instance    = this;

    m_imageView   = new ImagesView( this );
    m_imagesModel = new ImagesModel( this );
    m_imageView->setModel( m_imagesModel );

    connect( m_imagesModel, SIGNAL( imageAdded()   ), this, SLOT( actionStatusSetter() ) );
    connect( m_imagesModel, SIGNAL( imageRemoved() ), this, SLOT( actionStatusSetter() ) );

    createToolBar();
    statusBar(); // this tells QMainWindow to initialise the statusbar

    readSettings();

    setCentralWidget( m_imageView );

    actionStatusSetter();
}

void SqueezeWindow::closeEvent( QCloseEvent *event )
{
    writeSettings();
    event->accept();
}

void SqueezeWindow::writeSettings()
{
    QSettings settings( QDir::current().filePath( "squeeze.ini" ), QSettings::IniFormat );
    settings.setValue( "resize/x-percent", widthPercentage() );
    settings.setValue( "resize/y-percent", heightPercentage() );
    settings.setValue( "resize/aspect-lock", ( m_aspectLock->checkState() == Qt::Checked ) );

    settings.setValue( "save/directory", saveDirectory() );
    settings.setValue( "save/suffix", fileSuffix() );
    settings.setValue( "save/overwrite", ( m_overwriteFiles->checkState() == Qt::Checked ) );

    settings.setValue( "general/size", size() );
}

void SqueezeWindow::readSettings()
{
    QSettings settings( QDir::current().filePath( "squeeze.ini" ), QSettings::IniFormat );
    int x_percent = settings.value( "resize/x-percent", 50 ).toInt();
    int y_percent = settings.value( "resize/y-percent", 50 ).toInt();
    bool lockAspect = settings.value( "resize/aspect-lock", true ).toBool();

    QString save = settings.value( "save/directory", QDir::toNativeSeparators( QDir::homePath() ) ).toString();
    QString suffix = settings.value( "save/suffix", QString("") ).toString();
    bool overwrite = settings.value( "save/overwrite", false ).toBool();

    QSize size = settings.value( "general/size", QSize(750,500) ).toSize();

    // set the aspect locking before the values
    m_aspectLock->setCheckState( lockAspect ? Qt::Checked : Qt::Unchecked );
    m_resizeX->setValue( x_percent );
    m_resizeY->setValue( y_percent );

    m_saveDirectory->setText( save );
    m_fileSuffix->setText( suffix );
    m_overwriteFiles->setCheckState( overwrite ? Qt::Checked : Qt::Unchecked );

    resize( size );
}

void SqueezeWindow::createToolBar()
{
    m_toolBar = new QToolBar( this );
    m_toolBar->setMovable( false );
    m_toolBar->setFloatable( false );
    m_toolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    m_toolBar->toggleViewAction()->setVisible( false );
    m_toolBar->setIconSize( QSize( 64, 64 ) );
    m_toolBar->setContentsMargins( 10, 0, 10, 0 );

    /// ADD IMAGES ACTION

    m_addImages = m_toolBar->addAction( tr( "&Add Images" ), this, SLOT( addImages() ) );
    m_addImages->setIcon( QIcon( ":/insert-image.png" ) );

    m_toolBar->addSeparator();

    /// SIZE SETTINGS

    QWidget     *resizeElement = new QWidget();
    QGridLayout *resizeLayout  = new QGridLayout;

    QLabel *resizeXLabel = new QLabel( tr( "Width" ) );
    m_resizeX = new QDoubleSpinBox;
    m_resizeX->setMaximum( 10000 );
    m_resizeX->setSuffix( tr( "%" ) );

    QLabel *resizeYLabel = new QLabel( tr( "Height" ) );
    m_resizeY = new QDoubleSpinBox;
    m_resizeY->setMaximum( 10000 );
    m_resizeY->setSuffix( tr( "%" ) );

    connect( m_resizeX, SIGNAL( valueChanged( double ) ), this, SLOT( resizeWidthChanged( double )  ) );
    connect( m_resizeY, SIGNAL( valueChanged( double ) ), this, SLOT( resizeHeightChanged( double ) ) );

    m_aspectLock = new QCheckBox( tr( "&Lock Aspect Ratio" ) );

    resizeLayout->addWidget( resizeXLabel, 1, 1 );
    resizeLayout->addWidget( m_resizeX, 1, 2 );
    resizeLayout->addWidget( resizeYLabel, 2, 1 );
    resizeLayout->addWidget( m_resizeY, 2, 2 );
    resizeLayout->addWidget( m_aspectLock, 3, 1, 1, 2 );

    resizeElement->setLayout( resizeLayout );

    m_toolBar->addWidget( resizeElement );

    /// STORE SETTINGS

    m_toolBar->addSeparator();

    QWidget     *storeElement = new QWidget();
    QGridLayout *storeLayout  = new QGridLayout;
    QHBoxLayout *dLayout      = new QHBoxLayout;
    dLayout->setSpacing( 0 );

    QLabel *destLabel = new QLabel( tr( "Save Directory" ) );
    m_saveDirectory   = new QLineEdit;

    QCompleter *completer = new QCompleter( this );
    QDirModel  *completerModel = new QDirModel( completer );
    completerModel->setFilter( QDir::AllDirs | QDir::NoDotAndDotDot );
    completer->setModel( completerModel );
    m_saveDirectory->setCompleter( completer );

    connect( m_saveDirectory, SIGNAL( textChanged( const QString & ) ), this, SLOT( actionStatusSetter() ) );

    m_directoryChooser = new QPushButton( storeElement );
    m_directoryChooser->setIconSize( QSize( 16, 16 ) );
    m_directoryChooser->setIcon( QIcon( ":/document-open.png" ) );
    m_directoryChooser->setFlat( true );
    m_directoryChooser->setMaximumHeight( 22 );
    m_directoryChooser->setMaximumWidth( 22 );

    connect( m_directoryChooser, SIGNAL( clicked() ), this, SLOT( chooseSaveDirectory() ) );

    dLayout->addWidget( m_saveDirectory );
    dLayout->addWidget( m_directoryChooser );

    m_overwriteFiles    = new QCheckBox( tr( "&Overwrite files" ) );

    QLabel *suffixLabel = new QLabel( tr( "File Suffix" ) );
    m_fileSuffix        = new QLineEdit;

    storeLayout->addWidget( destLabel, 1, 1 );
    storeLayout->addLayout( dLayout, 1, 2 );
    storeLayout->addWidget( suffixLabel, 2, 1 );
    storeLayout->addWidget( m_fileSuffix, 2, 2 );
    storeLayout->addWidget( m_overwriteFiles, 3, 1, 1, 2 );

    storeElement->setLayout( storeLayout );

    m_toolBar->addWidget( storeElement );

    /// RESIZE ACTION

    m_toolBar->addSeparator();

    m_resizeImages = m_toolBar->addAction( tr( "&Resize Images" ), this, SLOT( resizeImages() ) );
    m_resizeImages->setIcon( QIcon( ":/resize-image.png" ) );

    addToolBar( m_toolBar );
}

void SqueezeWindow::setStatusBarText( const QString &text )
{
    statusBar()->showMessage( text );
}

void SqueezeWindow::addImages() // SLOT
{
    if( m_stopImageAdd )
    {
        m_imagesModel->stopAddImages();
        resetAddImagesButton();
        return;
    }

    QFileDialog dialog( this, tr("Select Images") );
    dialog.setFilter( tr("Images (*.png *.jpg *.jpeg *.bmp)") );
    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setFileMode( QFileDialog::ExistingFiles );

    QStringList images;
    if( dialog.exec() )
        images = dialog.selectedFiles();

    if( !images.isEmpty() )
    {
        m_stopImageAdd = true;
        m_addImages->setEnabled( false );
        m_imagesModel->addImages( images );
    }
}

void SqueezeWindow::resetAddImagesButton()
{
    m_addImages->setEnabled( true );
    m_stopImageAdd = false;
}

void SqueezeWindow::resizeImages() // SLOT
{
    m_addImages->setEnabled( m_stopImageResize );
    m_resizeX->setEnabled( m_stopImageResize );
    m_resizeY->setEnabled( m_stopImageResize );
    m_aspectLock->setEnabled( m_stopImageResize );
    m_saveDirectory->setEnabled( m_stopImageResize );
    m_fileSuffix->setEnabled( m_stopImageResize );
    m_overwriteFiles->setEnabled( m_stopImageResize );

    if( m_stopImageResize ) // stop
    {
        m_imagesModel->stopResizeImages();
        resetResizeImagesButton();
    }
    else // start
    {
        m_resizeImages->setEnabled( false );
        m_stopImageResize = true;
        m_imagesModel->resizeImages();
    }
}

void SqueezeWindow::actionStatusSetter() // SLOT
{
    bool hasSaveDir = !m_saveDirectory->text().isEmpty();
    bool hasImages  = m_imagesModel->imageCount() > 0;
    bool isResizing = m_imagesModel->isResizing();
    bool isAdding   = m_imagesModel->isAdding();

    m_resizeImages->setEnabled( hasSaveDir && hasImages && !isResizing && !isAdding );
}

void SqueezeWindow::resetResizeImagesButton()
{
    m_addImages->setEnabled( m_stopImageResize );
    m_resizeX->setEnabled( m_stopImageResize );
    m_resizeY->setEnabled( m_stopImageResize );
    m_aspectLock->setEnabled( m_stopImageResize );
    m_saveDirectory->setEnabled( m_stopImageResize );
    m_fileSuffix->setEnabled( m_stopImageResize );
    m_overwriteFiles->setEnabled( m_stopImageResize );

    m_resizeImages->setEnabled( true );
    m_stopImageResize = false;
}

void SqueezeWindow::resizeWidthChanged( double width ) // SLOT
{
    if( m_aspectLock->checkState() == Qt::Checked )
        m_resizeY->setValue( width );
}

void SqueezeWindow::resizeHeightChanged( double height ) // SLOT
{
    if( m_aspectLock->checkState() == Qt::Checked )
        m_resizeX->setValue( height );
}

void SqueezeWindow::chooseSaveDirectory() // SLOT
{
    QFileDialog dialog( this, tr("Select Save Directory") );
    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setFileMode( QFileDialog::DirectoryOnly );

    if( dialog.exec() )
    {
        QStringList dir = dialog.selectedFiles();
        m_saveDirectory->setText( dir.takeFirst() );
    }
}

