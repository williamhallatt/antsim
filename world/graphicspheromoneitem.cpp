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

#include "graphicspheromoneitem.h"
#include "utils/antconfig.h"
#include <QPen>

/*--------------------------------------------------------------------------------------*/

GraphicsPheromoneItem::GraphicsPheromoneItem( const AntPosition& position, PheromoneType type, QGraphicsItem* parent )
: AntPheromone( position, type ),
  QGraphicsEllipseItem( parent ),
  m_opacity( 1.0 )
{
  setPen( QPen( Qt::darkGray ) );
  setRect( position.x() - AntConfig::PheromoneSize / 2, position.y() - AntConfig::PheromoneSize / 2, AntConfig::PheromoneSize, AntConfig::PheromoneSize );

  switch( type )
  {
    case Found:
      setBrush( Qt::darkGreen );
      break;
    case Hazard:
      setBrush( Qt::darkRed );
      break;
    case None:
    default:
      setBrush( Qt::white );
  }
}

/*--------------------------------------------------------------------------------------*/

void GraphicsPheromoneItem::updateGraphics()
{
  m_opacity = qreal( pheromoneStrength() );
  setOpacity( m_opacity );
}

/*--------------------------------------------------------------------------------------*/