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

#include "antpheromone.h"
#include "antbot.h"
#include "utils/antconfig.h"
#include <algorithm>

/*--------------------------------------------------------------------------------------*/

AntPheromone::AntPheromone( const AntPosition& position, PheromoneType type )
: m_type           ( type ),
  m_position       ( position ),
  m_strength       ( 1.0 ),
  m_evaporationRate( 0.0 ),
  m_ants           () {}

/*--------------------------------------------------------------------------------------*/

AntPheromone::~AntPheromone() {}

/*--------------------------------------------------------------------------------------*/

void AntPheromone::update()
{
  m_ants.erase( std::remove_if( std::begin( m_ants ), std::end( m_ants ),
                                []( WeakAntPtr& ant ){ return ant.expired(); } ),
                std::end( m_ants ) );

  updateGraphics();

  /* Hazards will remain active for the duration of the run and will not evaporate. */
  if( m_type == Hazard )
  {
    m_strength = 1.0;
  }
  else
  {
    auto antPheromoneSum( 0.0 );

    for( auto& ant : m_ants )
    {
      SharedAntPtr bot = ant.lock();
      if( bot ) antPheromoneSum += bot->droppedPheromoneStrength();
    }

    m_strength = ( 1.0 - m_evaporationRate ) * m_strength + antPheromoneSum;
  }
}

/*--------------------------------------------------------------------------------------*/

double AntPheromone::pheromoneStrength() const
{
  return evaporated() ? 0.0 : m_strength;
}

/*--------------------------------------------------------------------------------------*/

void AntPheromone::setEvaporationRate( double evaporationRate )
{
  m_evaporationRate = evaporationRate;
}

/*--------------------------------------------------------------------------------------*/

bool AntPheromone::evaporated() const
{
  return m_strength < AntConfig::PheromoneMin;
}

/*--------------------------------------------------------------------------------------*/

void AntPheromone::registerAnt( SharedAntPtr ant )
{
  for( auto& bot : m_ants ) if( bot.lock() == ant ) return;
  m_ants.push_back( ant );
}

/*--------------------------------------------------------------------------------------*/

void AntPheromone::deRegisterAnt( const SharedAntPtr &ant )
{
  auto count( -1 );

  for( auto& bot : m_ants )
  {
    count++;
    if( bot.lock() == ant ) break;
  }

  if( count >= 0 ) m_ants.erase( std::begin( m_ants ) + count );
}

/*--------------------------------------------------------------------------------------*/

const AntPosition& AntPheromone::position() const
{
  return m_position;
}

/*--------------------------------------------------------------------------------------*/

AntPheromone::PheromoneType AntPheromone::pheromoneType() const
{
  return m_type;
}

/*--------------------------------------------------------------------------------------*/

void AntPheromone::setPheromoneType( AntPheromone::PheromoneType type )
{
  m_type = type;
}

/*--------------------------------------------------------------------------------------*/
