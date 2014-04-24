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

#include "antworldscene.h"
#include "graphicsworldtile.h"
#include "world/graphicsantitem.h"
#include "world/graphicspheromoneitem.h"
#include "utils/antconfig.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>

/*--------------------------------------------------------------------------------------*/

const qreal marginPerc = 0.02;   // percentage used for determining margins surrounding the scene

/*--------------------------------------------------------------------------------------*/

GraphicsAntWorldScene::GraphicsAntWorldScene( QObject* parent )
: QGraphicsScene   ( parent ),
  AntWorld         (),
  m_type           ( AntWorldTile::None ),
  m_spawnPoints    (),
  m_currentTileRect() {}

/*--------------------------------------------------------------------------------------*/

void GraphicsAntWorldScene::constructWorldGrid()
{
  reset();

  qreal totalWidth = sceneRect().width() * ( 1.0 - marginPerc );
  qreal totalHeight = sceneRect().height() * ( 1.0 - marginPerc );

  qreal x( ( sceneRect().width() - totalWidth ) / 2 );

  while( x < totalWidth )
  {
    qreal y( ( sceneRect().height() - totalHeight ) / 2 );

    while( y < totalHeight )
    {
      m_currentTileRect = QRectF( x, y, AntConfig::TileSize, AntConfig::TileSize );
      registerWorldTile( calculateCentreCoordinate( m_currentTileRect ), AntWorldTile::Wall );
      y += AntConfig::TileSize;
    }

    x += AntConfig::TileSize;
  }
}

/*--------------------------------------------------------------------------------------*/

void GraphicsAntWorldScene::reset()
{
  resetAntRegister();
  resetPheromoneRegister();
  resetWorldTileRegister();
  clear();
  m_spawnPoints.clear();
}

/*--------------------------------------------------------------------------------------*/

void GraphicsAntWorldScene::addWorldTile( QDomElement element )
{
  if( !element.isNull() )
  {
    m_currentTileRect = QRectF( qreal( element.attribute( "x" ).toDouble() ),
                                qreal( element.attribute( "y" ).toDouble() ),
                                qreal( element.attribute( "width" ).toDouble() ),
                                qreal( element.attribute( "height" ).toDouble() ) );

    registerWorldTile( calculateCentreCoordinate( m_currentTileRect ), static_cast< AntWorldTile::TileType >( element.attribute( "type" ).toInt() ) );
  }
}

/*--------------------------------------------------------------------------------------*/

std::vector< const AntWorldTile* > GraphicsAntWorldScene::neighbours( const AntPosition& position ) const
{
  QPointF pointF( qreal( position.x() ), qreal( position.y() ) );

  /* Since we're working in a 2D grid, we can get all 4 neighbour tiles by offsetting
   * the relevant coordinate with the amount equal to the square edge of a tile (which
   * should place us roughly near the centre of whichever neighbour we're searching for. */
  QPointF northNeighbour( pointF + QPointF( 0.0, AntConfig::TileSize ) );
  QPointF southNeighbour( pointF - QPointF( 0.0, AntConfig::TileSize ) );
  QPointF westNeighbour( pointF + QPointF( AntConfig::TileSize, 0.0 ) );
  QPointF eastNeighbour( pointF - QPointF( AntConfig::TileSize, 0.0 ) );

  QList< QPointF > neighboursPoints = QList< QPointF >() << northNeighbour << southNeighbour << westNeighbour << eastNeighbour;
  std::vector< const AntWorldTile* > neighbours;

  for( int i = 0; i < neighboursPoints.size(); ++i )
  {
    QList< QGraphicsItem* > itemList = items( neighboursPoints.at( i ) );

    for( int j = 0; j < itemList.size(); ++j )
    {
      GraphicsWorldTile* tile = dynamic_cast< GraphicsWorldTile* >( itemList.at( j ) );
      if( tile ) neighbours.push_back( tile );
    }
  }

  return neighbours;
}

/*--------------------------------------------------------------------------------------*/

const QList< const GraphicsWorldTile *> &GraphicsAntWorldScene::spawnPoints() const
{
  return m_spawnPoints;
}

/*--------------------------------------------------------------------------------------*/

void GraphicsAntWorldScene::setTileType( AntWorldTile::TileType type )
{
  m_type = type;
}

/*--------------------------------------------------------------------------------------*/

void GraphicsAntWorldScene::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
  changeItemType( event->scenePos() );
}

/*--------------------------------------------------------------------------------------*/

void GraphicsAntWorldScene::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
  changeItemType( event->scenePos() );
}

/*--------------------------------------------------------------------------------------*/

AntBot* GraphicsAntWorldScene::createAnt( const AntPosition& position )
{
  QPixmap pixMap( ":/resources/ant.png" );
  GraphicsAntItem* ant = new GraphicsAntItem( this,
                                              position,
                                              pixMap.scaled( AntConfig::AntSize, AntConfig::AntSize, Qt::KeepAspectRatio ) );

  addItem( ant );
  return ant;
}

/*--------------------------------------------------------------------------------------*/

AntPheromone* GraphicsAntWorldScene::createPheromone( const AntPosition& position, AntPheromone::PheromoneType type )
{
  GraphicsPheromoneItem* pheromone = new GraphicsPheromoneItem( position, type );
  addItem( pheromone );
  return pheromone;
}

/*--------------------------------------------------------------------------------------*/

AntWorldTile* GraphicsAntWorldScene::createWorldTile( const AntPosition& position, AntWorldTile::TileType type )
{
  GraphicsWorldTile* tile = new GraphicsWorldTile( position, type );
  tile->setRect( m_currentTileRect );
  addItem( tile );    // takes ownership

  if( tile->tileType() == GraphicsWorldTile::Spawn )
  {
    m_spawnPoints.append( tile );
  }

  return tile;
}

/*--------------------------------------------------------------------------------------*/

AntPosition GraphicsAntWorldScene::calculateCentreCoordinate( const QRectF& rect )
{
  AntPosition centre( rect.x() + rect.width() / 2,
                      rect.y() + rect.height() / 2 );
  return centre;
}

/*--------------------------------------------------------------------------------------*/

void GraphicsAntWorldScene::changeItemType( const QPointF& point )
{
  if( m_type == GraphicsWorldTile::None )
  {
    return;
  }

  QList< QGraphicsItem* > itemList = items( point );

  for( int i = 0; i < itemList.size(); ++i )
  {
    GraphicsWorldTile* tile = dynamic_cast< GraphicsWorldTile* >( itemList.at( i ) );

    if( tile )
    {
      if( tile->tileType() == GraphicsWorldTile::Spawn )
      {
        m_spawnPoints.removeAll( tile );
      }

      if( m_type == GraphicsWorldTile::Spawn )
      {
        m_spawnPoints.append( tile );
      }

      tile->setTileType( m_type );
    }
  }
}

/*--------------------------------------------------------------------------------------*/
