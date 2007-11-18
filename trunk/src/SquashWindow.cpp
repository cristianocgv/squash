/***************************************************************************
 * copyright            : (C) 2007 Seb Ruiz <ruiz@kde.org>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 ***************************************************************************/

#include "ImagesModel.h"
#include "ImagesView.h"
#include "SquashWindow.h"

#include <QDebug>
#include <QtGui>
#include <QSettings>

SquashWindow *SquashWindow::s_instance = 0;

SquashWindow::SquashWindow( QWidget *parent, Qt::WindowFlags flags )
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

    setAcceptDrops( true );
    setCentralWidget( m_imageView );

    actionStatusSetter();
}

void SquashWindow::closeEvent( QCloseEvent *event )
{
    writeSettings();
    event->accept();
}

QString SquashWindow::getSettingsPath()
{
#ifdef Q_WS_WIN
    return QDir::current().filePath( "squash.ini" );
#else
    QDir dir = QDir::home();
    if( !dir.cd( ".qt" ) )
    {
        dir.mkdir( ".qt" );
        dir.cd( ".qt" );
    }

    return dir.filePath( "squashrc" );
#endif
}

void SquashWindow::writeSettings()
{
#ifdef Q_WS_WIN
    // on windows save to ini file so that the application will be portable
    QSettings settings( getSettingsPath(), QSettings::IniFormat );
#else
    QSettings settings( getSettingsPath(), QSettings::NativeFormat );
#endif

    settings.setValue( "resize/x-percent", widthPercentage() );
    settings.setValue( "resize/y-percent", heightPercentage() );
    settings.setValue( "resize/aspect-lock", ( m_aspectLock->isChecked() ) );

    settings.setValue( "save/directory", saveDirectory() );
    settings.setValue( "save/suffix", fileSuffix() );
    settings.setValue( "save/overwrite", ( m_overwriteFiles->isChecked() ) );

    settings.setValue( "filechooser/dialog", m_dialogSettings );

    settings.setValue( "general/size", size() );
}

void SquashWindow::readSettings()
{
#ifdef Q_WS_WIN
    // on windows save to ini file so that the application will be portable
    QSettings settings( getSettingsPath(), QSettings::IniFormat );
#else
    QSettings settings( getSettingsPath(), QSettings::NativeFormat );
#endif

    int x_percent = settings.value( "resize/x-percent", 50 ).toInt();
    int y_percent = settings.value( "resize/y-percent", 50 ).toInt();
    bool lockAspect = settings.value( "resize/aspect-lock", true ).toBool();

    QString save = settings.value( "save/directory", QDir::toNativeSeparators( QDir::homePath() ) ).toString();
    QString suffix = settings.value( "save/suffix", QString("") ).toString();
    bool overwrite = settings.value( "save/overwrite", false ).toBool();

    QSize size = settings.value( "general/size", QSize(750,500) ).toSize();

    m_dialogSettings = settings.value( "filechooser/dialog", QByteArray() ).toByteArray();

    // set the aspect locking before the values
    m_aspectLock->setChecked( lockAspect );
    m_resizeX->setValue( x_percent );
    m_resizeY->setValue( y_percent );

    m_saveDirectory->setText( save );
    m_fileSuffix->setText( suffix );
    m_overwriteFiles->setCheckState( overwrite ? Qt::Checked : Qt::Unchecked );

    resize( size );
}

void SquashWindow::createToolBar()
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

    m_resizeCombo = new QComboBox();
    m_resizeCombo->addItem( tr( "Percent" ), QVariant( PERCENT ) );
    m_resizeCombo->addItem( tr( "Pixels" ), QVariant( PIXEL ) );
    m_resizeCombo->addItem( tr( "Set height" ), QVariant( HEIGHT ) );
    m_resizeCombo->addItem( tr( "Set width" ), QVariant( WIDTH ) );
    m_resizeCombo->addItem( tr( "Set maximum side length" ), QVariant( MAX ) );

    connect( m_resizeCombo, SIGNAL( activated( int ) ), this, SLOT( resizeMethodChanged( int ) ) );

    m_resizeXLabel = new QLabel( tr( "Width" ) );
    m_resizeX = new QDoubleSpinBox;
    m_resizeX->setMaximum( 10000 );
    m_resizeX->setSuffix( tr( "%" ) );

    m_resizeYLabel = new QLabel( tr( "Height" ) );
    m_resizeY = new QDoubleSpinBox;
    m_resizeY->setMaximum( 10000 );
    m_resizeY->setSuffix( tr( "%" ) );

    connect( m_resizeX, SIGNAL( valueChanged( double ) ), this, SLOT( resizeWidthChanged( double )  ) );
    connect( m_resizeY, SIGNAL( valueChanged( double ) ), this, SLOT( resizeHeightChanged( double ) ) );

    m_aspectLock = new QPushButton( QIcon( ":/aspect-lock.png" ), QString() );
    m_aspectLock->setCheckable( true );
    m_aspectLock->setToolTip( tr( "Lock Aspect Ratio" ) );

    resizeLayout->addWidget( m_resizeCombo, 1, 1, 1, 3 );
    resizeLayout->addWidget( m_resizeXLabel, 2, 1 );
    resizeLayout->addWidget( m_resizeX, 2, 2 );
    resizeLayout->addWidget( m_resizeYLabel, 3, 1 );
    resizeLayout->addWidget( m_resizeY, 3, 2 );
    resizeLayout->addWidget( m_aspectLock, 2, 3, 2, 1 );

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

void SquashWindow::keyPressEvent( QKeyEvent *event )
{
    if( event->key() == Qt::Key_Q && event->modifiers() & Qt::ControlModifier )
    {
        event->accept();
        qApp->quit();
        return;
    }
    QMainWindow::keyPressEvent( event );
}

void SquashWindow::dragEnterEvent( QDragEnterEvent *event )
{
    qDebug() << "drag enter event: " << event->mimeData()->formats() << " has Image? " << event->mimeData()->hasImage();
    if( validUrls( event->mimeData()->urls() ) )
    {
        event->acceptProposedAction();
    }
}

void SquashWindow::dropEvent( QDropEvent *event )
{
    const QMimeData *mimeData = event->mimeData();
    QStringList images;

    if( mimeData->hasUrls() )
    {
        QList<QUrl> urlList = mimeData->urls();
        foreach( QUrl url, urlList )
        {
            if( !url.isValid() )
                continue;
            images.append( url.toLocalFile() );
        }
    }

    event->acceptProposedAction();

    addImages( images );
}

bool SquashWindow::validUrls( QList<QUrl> list )
{
    foreach( QUrl url, list )
    {
        if( url.isValid() )
            return true;
    }
    return false;
}

void SquashWindow::setStatusBarText( const QString &text )
{
    statusBar()->showMessage( text );
}

void SquashWindow::addImages() // SLOT
{
    if( m_stopImageAdd )
    {
        m_imagesModel->stopAddImages();
        resetAddImagesButton();
        return;
    }

    QFileDialog dialog( this, tr("Select Images") );
    dialog.restoreState( m_dialogSettings );
    dialog.setFilter( tr("Images (*.png *.jpg *.jpeg *.bmp)") );
    dialog.setAcceptMode( QFileDialog::AcceptOpen );
    dialog.setFileMode( QFileDialog::ExistingFiles );

    QStringList images;
    if( dialog.exec() )
    {
        images = dialog.selectedFiles();
        m_dialogSettings = dialog.saveState();
    }

    if( !images.isEmpty() )
    {
        addImages( images );
    }
}

void SquashWindow::addImages( QStringList images )
{
    m_stopImageAdd = true;
    m_addImages->setEnabled( false );
    m_imagesModel->addImages( images );
}

void SquashWindow::resetAddImagesButton()
{
    m_addImages->setEnabled( true );
    m_stopImageAdd = false;
}

void SquashWindow::resizeImages() // SLOT
{
    enableSettings( m_stopImageResize );

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

void SquashWindow::actionStatusSetter() // SLOT
{
    bool hasSaveDir = !m_saveDirectory->text().isEmpty();
    bool hasImages  = m_imagesModel->imageCount() > 0;
    bool isResizing = m_imagesModel->isResizing();
    bool isAdding   = m_imagesModel->isAdding();

    bool enableResize = !isResizing && !isAdding && hasSaveDir && hasImages;
    bool enableAdd    = !isAdding && !isResizing;

    m_resizeImages->setEnabled( enableResize );
    m_addImages->setEnabled( enableAdd  );

    enableSettings( !isResizing );
}

void SquashWindow::enableSettings( const bool enable )
{
    m_resizeX->setEnabled( enable );
    m_resizeY->setEnabled( enable );
    m_aspectLock->setEnabled( enable );
    m_saveDirectory->setEnabled( enable );
    m_directoryChooser->setEnabled( enable );
    m_fileSuffix->setEnabled( enable );
    m_overwriteFiles->setEnabled( enable );
}

void SquashWindow::resetResizeImagesButton()
{
    m_addImages->setEnabled( m_stopImageResize );
    m_resizeImages->setEnabled( true );
    m_stopImageResize = false;
}

void SquashWindow::resizeMethodChanged( int index )
{
    QVariant data = m_resizeCombo->itemData( index );
    if( !data.isValid() )
        return;

    switch( data.toInt() )
    {
        case WIDTH:
            m_resizeXLabel->setEnabled( true );
            m_resizeX->setEnabled( true );
            m_resizeYLabel->setEnabled( false );
            m_resizeY->setEnabled( false );
            m_aspectLock->setEnabled( false );
            break;

        case HEIGHT:
            m_resizeXLabel->setEnabled( false );
            m_resizeX->setEnabled( false );
            m_resizeYLabel->setEnabled( true );
            m_resizeY->setEnabled( true );
            m_aspectLock->setEnabled( false );
            break;

        case MAX:
            m_aspectLock->setEnabled( false );
            break;

        case PIXEL:
            m_aspectLock->setEnabled( true );
            //fall through

        case PERCENT:
            //fall through

        default:
            m_resizeXLabel->setEnabled( true );
            m_resizeX->setEnabled( true );
            m_resizeYLabel->setEnabled( true );
            m_resizeY->setEnabled( true );
            m_aspectLock->setEnabled( true );
            break;
    }
}

void SquashWindow::resizeWidthChanged( double width ) // SLOT
{
    if( m_aspectLock->isChecked() )
        m_resizeY->setValue( width );
}

void SquashWindow::resizeHeightChanged( double height ) // SLOT
{
    if( m_aspectLock->isChecked() )
        m_resizeX->setValue( height );
}

void SquashWindow::chooseSaveDirectory() // SLOT
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

