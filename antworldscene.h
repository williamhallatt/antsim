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

#ifndef GRAPHICSANTWORLDSCENE_H
#define GRAPHICSANTWORLDSCENE_H

#include "ants/antworld.h"
#include "graphicsworldtile.h"

#include <QGraphicsScene>
#include <QList>

/*! \brief AntWorldScene is responsible for the world's graphical representation. */

class GraphicsAntWorldScene : public QGraphicsScene,
                              public AntWorld
{
Q_OBJECT
public:
  /*! Constructor. */
  explicit GraphicsAntWorldScene( QObject* parent = 0 );

  /*! Builds a default world (all existing world items will be deleted). */
  void constructWorldGrid();

  /*! Clears and resets the current scene to a default state. */
  void reset();

  /*! Creates and adds a GraphicsWorldTile object from the given DOM element. */
  void addWorldTile( QDomElement element );

  /*! Returns a list of all the world tiles immediately adjacent to that of the
   *  tile with centre node position "position". */
  std::vector< const AntWorldTile* > neighbours( const AntPosition& position ) const;

  /*! Returns a list of all the tiles that are user-defined spawn points. */
  const QList< const GraphicsWorldTile* >& spawnPoints() const;

  public slots:
  /*! Sets the type when selecting graphics items via mouse input events.
   *  \sa mousePressEvent
   *  \sa mouseMoveEvent
   *  \sa changeItemType */
  void setTileType( AntWorldTile::TileType type );

protected:
  /*! Re-implemented from QGraphicsScene.
   *  \sa changeItemType */
  void mousePressEvent( QGraphicsSceneMouseEvent* event );

  /*! Re-implemented from QGraphicsScene.
   *  \sa changeItemType */
  void mouseMoveEvent( QGraphicsSceneMouseEvent* event );

  /*! Re-implemented from AntWorld. */
  AntBot* createAnt( const AntPosition& position );

  /*! Re-implemented from AntWorld. */
  AntPheromone* createPheromone( const AntPosition& position, AntPheromone::PheromoneType type );

  /*! Re-implemented from AntWorld. */
  AntWorldTile* createWorldTile( const AntPosition& position, AntWorldTile::TileType type );
private:
  /*! Changes the item type under "point" to the current active type
   *  \sa setTileType */
  void changeItemType( const QPointF& point );

  /*! Calculates the AntPosition at the centre of the "rect". */
  AntPosition calculateCentreCoordinate( const QRectF& rect );

private:
  AntWorldTile::TileType m_type;
  QList< const GraphicsWorldTile* > m_spawnPoints;
  QRectF m_currentTileRect;
};

#endif // GRAPHICSANTWORLDSCENE_H
