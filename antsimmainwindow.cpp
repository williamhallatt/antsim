/* Copyright (c) 2013 by William Hallatt.
 *
 * This file forms part of "AntSim".
 *
 * The official website for this project is <http://www.goblincoding.com> and,
 * although not compulsory, it would be appreciated if all works of whatever
 * nature using this source code (in whole or in part) include a reference to
 * this site.
 *
 * Should you wish to contact me for whatever reason, please do so via:
 *
 *                 <http://www.goblincoding.com/contact>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program (GNUGPL.txt).  If not, see
 *
 *                    <http://www.gnu.org/licenses/>
 */

#include "antsimmainwindow.h"
#include "ui_antsimmainwindow.h"
#include "world/antworldscene.h"
#include "world/graphicsantitem.h"
#include "ants/antworld.h"
#include "utils/antconfig.h"

#include <QTimer>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDomDocument>
#include <QTextStream>

/*--------------------------------------------------------------------------------------*/

AntSimMainWindow::AntSimMainWindow( QWidget* parent )
: QMainWindow     ( parent ),
  ui              ( new Ui::AntSimMainWindow ),
  m_scene         ( new GraphicsAntWorldScene( this ) ),
  m_synchTimer    ( new QTimer( this ) ),
  m_totalTimer    (),
  m_elapsedTime   ( 0, 0, 0, 0 ),
  m_fileName      ( "" ),
  m_stopped       ( true )
{
  ui->setupUi( this );
  showMaximized();

  connect( ui->actionExit, SIGNAL( triggered() ), this, SLOT( close() ) );
  connect( ui->actionNew,  SIGNAL( triggered() ), this, SLOT( initialise() ) );
  connect( ui->actionOpen, SIGNAL( triggered() ), this, SLOT( open() ) );
  connect( ui->actionSave, SIGNAL( triggered() ), this, SLOT( save() ) );
  connect( ui->actionSaveAs, SIGNAL( triggered() ), this, SLOT( saveAs() ) );
  connect( ui->startPushButton, SIGNAL( clicked() ), this, SLOT( startStopSim() ) );
  connect( ui->resetPushButton, SIGNAL( clicked() ), this, SLOT( reset() ) );
  connect( ui->pheromoneCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( togglePheromones( bool ) ) );
  connect( ui->smartPheromoneCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( toggleSmartPheromones( bool ) ) );

  QButtonGroup* buttonGroup = new QButtonGroup( this );
  buttonGroup->addButton( ui->pathPushButton, 0 );
  buttonGroup->addButton( ui->wallPushButton, 1 );
  buttonGroup->addButton( ui->hazardPushButton, 2 );
  buttonGroup->addButton( ui->foodPushButton, 3 );
  buttonGroup->addButton( ui->spawnPushButton, 4 );
  buttonGroup->addButton( ui->cursorPushButton, 5 );
  connect( buttonGroup, SIGNAL( buttonClicked( int ) ), this, SLOT( buttonClicked( int ) ) );

  connect( ui->advanceRateSpinBox, SIGNAL( valueChanged( int ) ), m_synchTimer, SLOT( start( int ) ) );
  connect( m_synchTimer, SIGNAL( timeout() ), this, SLOT( advance() ) );
  m_synchTimer->setInterval( ui->advanceRateSpinBox->value() );

  connect( ui->evaporationSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( setEvaporationRate( double ) ) );
  connect( ui->maxNodesSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( setMaxNodesRemembered( int ) ) );

  QTimer::singleShot( 100, this, SLOT( initialise() ) );
}

/*--------------------------------------------------------------------------------------*/

AntSimMainWindow::~AntSimMainWindow()
{
  delete ui;
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::initialise()
{
  ui->graphicsView->setScene( m_scene );
  m_scene->setSceneRect( ui->graphicsView->rect() );

  m_scene->constructWorldGrid();
  ui->graphicsView->fitInView( m_scene->sceneRect(), Qt::KeepAspectRatio );

  m_scene->setEvaporationRate( ui->evaporationSpinBox->value() );
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName( this, "Load World",
                                                   QDir::currentPath(),
                                                   QString( "World Files (*.world)" ) );

  /* If user didn't cancel. */
  if( !fileName.isEmpty() )
  {
    loadWorld( fileName );
  }
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::save()
{
  if( !m_fileName.isEmpty() )
  {
    QFile file( m_fileName );

    if( file.open( QIODevice::ReadWrite | QIODevice::Text ) )
    {
      QDomDocument doc;
      QDomElement root = doc.createElement( "AntSimWorld" );
      doc.appendChild( root );

      QList< QGraphicsItem* > itemList = m_scene->items();

      for( int i = 0; i < itemList.size(); ++i )
      {
        GraphicsWorldTile* tile = dynamic_cast< GraphicsWorldTile* >( itemList.at( i ) );

        if( tile )
        {
          root.appendChild( tile->serialise() );
        }
      }

      QTextStream outStream( &file );
      outStream << doc.toString( 2 );
      file.close();
    }
    else
    {
      QMessageBox::critical( this, "Error", file.errorString() );
    }
  }
  else
  {
    saveAs();
  }
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::saveAs()
{
  m_fileName = QFileDialog::getSaveFileName( this, "Save World",
                                             QDir::currentPath(),
                                             QString( "World Files (*.world)" ) );

  /* Make sure the user didn't cancel. */
  if( !m_fileName.isEmpty() )
  {
    save();
  }
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::buttonClicked( int button )
{
  switch( button )
  {
    case 0:
      m_scene->setTileType( GraphicsWorldTile::Path );
      break;
    case 1:
      m_scene->setTileType( GraphicsWorldTile::Wall );
      break;
    case 2:
      m_scene->setTileType( GraphicsWorldTile::Hazard );
      break;
    case 3:
      m_scene->setTileType( GraphicsWorldTile::Food );
      break;
    case 4:
      m_scene->setTileType( GraphicsWorldTile::Spawn );
      break;
    default:
      m_scene->setTileType( GraphicsWorldTile::None );
  }
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::reset()
{
  setAntStats();

  m_scene->resetAntRegister();
  m_scene->resetPheromoneRegister();

  m_elapsedTime = QTime( 0, 0, 0, 0 );
  ui->elapsedTimeEdit->setText( "00:00:00" );

  // Set the flag to "false" to force "stopped" functionality.
  // This is counter-intuitive, see startStopSim.
  m_stopped = false;
  startStopSim();
  ui->startPushButton->setText( "Start" );
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::startStopSim()
{
  // If the sim was stopped, we need to restart it.
  if( m_stopped )
  {
    ui->nrAntsSpinBox->setEnabled( false );    
    ui->pheromoneCheckBox->setEnabled( false );
    ui->smartPheromoneCheckBox->setEnabled( false );
    ui->nrRunsSpinBox->setEnabled( false );
    ui->timePerRunSpinBox->setEnabled( false );

    ui->startPushButton->setText( "Stop" );
    ui->resetPushButton->setEnabled( false );

    m_stopped = false;

    // Connect this slot in case the nr of ants has changed.
    connect( m_synchTimer, SIGNAL( timeout() ), this, SLOT( spawn() ) );
    m_synchTimer->start();

    m_totalTimer.start();
  }
  else
  {
    // If the sim was running, we need to stop it.
    ui->nrAntsSpinBox->setEnabled( true );
    ui->pheromoneCheckBox->setEnabled( true );
    ui->smartPheromoneCheckBox->setEnabled( true );
    ui->nrRunsSpinBox->setEnabled( true );
    ui->timePerRunSpinBox->setEnabled( true );

    ui->startPushButton->setText( "Continue" );
    ui->resetPushButton->setEnabled( true );

    m_stopped = true;

    m_synchTimer->stop();
  }
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::spawn()
{
  if( ( m_scene->antCount() + ui->deadLineEdit->text().toInt() ) < ui->nrAntsSpinBox->value() )
  {
    if( m_scene->spawnPoints().size() > 0 )
    {
      /* Choose a random spawn point if there are more than one. */
      int point = qrand() % m_scene->spawnPoints().size();
      m_scene->registerAnt( m_scene->spawnPoints().at( point )->centre() );
    }
  }
  else
  {
    /* Once the desired nr of ants have been created, disconnect the timer from this slot. */
    disconnect( m_synchTimer, SIGNAL( timeout() ), this, SLOT( spawn() ) );
  }
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::advance()
{
  if( !m_stopped )
  {
    m_scene->tick();
    setAntStats();
  }
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::setEvaporationRate( double evaporationRate )
{
  m_scene->setEvaporationRate( evaporationRate );
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::togglePheromones( bool enable )
{
  m_scene->setPheromonesEnabled( enable );
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::toggleSmartPheromones( bool enable )
{
  m_scene->setSmartPheromonesEnabled( enable );
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::setMaxNodesRemembered( int maxNodesRemembered )
{
  m_scene->setMaxNodesRemembered( maxNodesRemembered );
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::setAntStats()
{
  ui->gatheringLineEdit->setText( QString( "%1" ).arg( m_scene->gatheringAnts() ) );
  ui->foragingLineEdit->setText( QString( "%1" ).arg( m_scene->foragingAnts() ) );
  ui->deadLineEdit->setText( QString( "%1" ).arg( m_scene->deadAnts() ) );
  ui->shortestLineEdit->setText( QString( "%1" ).arg( m_scene->shortestPathLength() ) );

  /* Sets the elapsed time. */
  m_elapsedTime = m_elapsedTime.addMSecs( m_totalTimer.elapsed() );
  ui->elapsedTimeEdit->setText( m_elapsedTime.toString( "HH:mm:ss" ) );
  m_totalTimer.restart();
}

/*--------------------------------------------------------------------------------------*/

void AntSimMainWindow::loadWorld( QString fileName )
{
  QFile file( fileName );

  if( file.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    QTextStream inStream( &file );
    QString errMsg;
    int errLine;
    int errCol;

    QDomDocument doc;

    if( doc.setContent( inStream.readAll(), &errMsg, &errLine, &errCol ) )
    {
      m_scene->reset();
      m_scene->setSceneRect( QRectF() );

      QDomNodeList elements = doc.elementsByTagName( "GraphicsWorldTile" );

      for( int i = 0; i < elements.size(); ++i )
      {
        m_scene->addWorldTile( elements.at( i ).toElement() );
      }

      ui->graphicsView->fitInView( m_scene->sceneRect(), Qt::KeepAspectRatio );
      m_fileName = fileName;
    }
    else
    {
      QMessageBox::critical( this, "Error", QString( "XML Broken: %1, line: %2, column: %3" )
        .arg( errMsg )
        .arg( errLine )
        .arg( errCol ) );
    }

    file.close();
  }
  else
  {
    QMessageBox::critical( this, "Error", file.errorString() );
  }
}

/*--------------------------------------------------------------------------------------*/
