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

#include "antworldtile.h"
#include "utils/antconfig.h"
#include <algorithm>

/*--------------------------------------------------------------------------------------*/

AntWorldTile::AntWorldTile()
: m_type  ( Wall ),
  m_centre( 0.0, 0.0 ),
  m_pheromone() {}

/*--------------------------------------------------------------------------------------*/

AntWorldTile::~AntWorldTile() {}

/*--------------------------------------------------------------------------------------*/

const AntPosition& AntWorldTile::centre() const
{
  return m_centre;
}

/*--------------------------------------------------------------------------------------*/

void AntWorldTile::setCentre( const AntPosition& centre )
{
  m_centre = centre;
}

/*--------------------------------------------------------------------------------------*/
AntWorldTile::TileType AntWorldTile::tileType() const
{
  return m_type;
}

/*--------------------------------------------------------------------------------------*/

void AntWorldTile::registerPheromone( SharedPheromonePtr pher )
{
  m_pheromone = pher;
}

/*--------------------------------------------------------------------------------------*/

AntPheromone::PheromoneType AntWorldTile::tilePheromoneType() const
{
  SharedPheromonePtr pher = m_pheromone.lock();
  return ( pher ) ? pher->pheromoneType() : AntPheromone::None;
}

/*--------------------------------------------------------------------------------------*/

double AntWorldTile::tilePheromoneStrength() const
{
  /* Possible that the pheromone has evaporated, but hasn't been deleted yet. */
  SharedPheromonePtr pher = m_pheromone.lock();
  return ( pher && !pher->evaporated() ) ? pher->pheromoneStrength() : AntConfig::PheromoneMin;
}

/*--------------------------------------------------------------------------------------*/

void AntWorldTile::setTileType( TileType type )
{
  if( type != None )
  {
    m_type = type;
    updateGraphics( m_type );
  }
}

/*--------------------------------------------------------------------------------------*/
