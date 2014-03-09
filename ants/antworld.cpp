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

#include "antworld.h"
#include "antbot.h"
#include "antpheromone.h"
#include "antworldtile.h"

#include <algorithm>
#include <time.h>
#include <climits>

/*--------------------------------------------------------------------------------------*/

template< typename T >
void deletePointers( std::vector< T* >& vec )
{
  std::for_each( std::begin( vec ), std::end( vec ),
                 []( T*& p ){ if( p ){ delete p; p = nullptr; }; } );
}

template< typename T >
void removeNullPointers( std::vector< T* >& vec )
{
  vec.erase( std::remove_if( std::begin( vec ), std::end( vec ),
                             []( T* p ){ return !p; } ),
             std::end( vec ) );
}

/*--------------------------------------------------------------------------------------*/

AntWorld::AntWorld()
:
  m_foragingAnts         ( 0 ),
  m_gatheringAnts        ( 0 ),
  m_deadAnts             ( 0 ),
  m_evaporationRate      ( 0.0 ),
  m_pheromoneEnabled     ( true ),
  m_smartPheromoneEnabled( true ),
  m_maxNodesRemembered   ( 5 ),
  m_ants                 (),
  m_pheromones           (),
  m_currentShortestPath  ()
{
  /* Seed rand for the duration of the sim's run. */
  srand( time( 0 ) );
}

/*--------------------------------------------------------------------------------------*/

AntWorld::~AntWorld()
{
  resetAntRegister();
  resetPheromoneRegister();
  resetWorldTileRegister();
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::tick()
{
  updateAnts();
  updatePheromones();
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::registerAnt( const AntPosition& position )
{
  /* Since ants are constantly moving, we are not concerned with ants spawning in the
   * same position (unlike pheromones and tiles which stay put and shouldn't be duplicated). */
  AntBot* ant = createAnt( position );
  ant->setMaxNodesRemembered( m_maxNodesRemembered );
  m_ants.push_back( SharedAntPtr( ant ) );
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::registerPheromone( const AntPosition& position, AntPheromone::PheromoneType type, SharedAntPtr& ant )
{
  if( m_pheromoneEnabled ||
      m_smartPheromoneEnabled )
  {
    AntPheromone* pheromone = findPheromone( position );

    if( pheromone )
    {
      /* Don't change the type if the pheromone is a hazard pheromone. */
      if( pheromone->pheromoneType() != AntPheromone::Hazard )
      {
        pheromone->setPheromoneType( type );
      }
    }
    else
    {
      /* For now this is sufficient, if other types of smart pheromone is going to be
         introduced, this section will have to be refactored. */
      if ( ( type == AntPheromone::Hazard && m_smartPheromoneEnabled ) ||
           ( type != AntPheromone::Hazard && m_pheromoneEnabled ) )
      {
        pheromone = createPheromone( position, type );
        pheromone->setEvaporationRate( m_evaporationRate );
        m_pheromones.push_back( SharedPherPtr( pheromone ) );

        /* Register the pheromone with the tile it was dropped on. */
        AntWorldTile* tile = findTile( position );
        if( tile ) tile->registerPheromone( m_pheromones.back() );
      }
    }

    if ( pheromone )
    {
      pheromone->registerAnt( ant );
      ant->registerPheromonePosition( pheromone->position() );
    }
  }
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::registerWorldTile( const AntPosition& position, AntWorldTile::TileType type )
{
  if( !findTile( position ) ) m_worldTiles.push_back( createWorldTile( position, type ) );
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::resetAntRegister()
{
  m_ants.clear();
  m_foragingAnts = 0;
  m_gatheringAnts = 0;
  m_deadAnts = 0;
  m_currentShortestPath.clear();
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::resetPheromoneRegister()
{
  m_pheromones.clear();
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::resetWorldTileRegister()
{
  deletePointers( m_worldTiles );
  removeNullPointers( m_worldTiles );
}

/*--------------------------------------------------------------------------------------*/

AntPheromone* AntWorld::findPheromone( const AntPosition& position )
{
  auto pher = std::find_if( std::begin( m_pheromones ), std::end( m_pheromones ),
                            [ &position ]( SharedPherPtr& p ){ return p && p->position() == position; } );

  return ( pher != std::end( m_pheromones ) ) ? ( *pher ).get() : nullptr;
}

/*--------------------------------------------------------------------------------------*/

AntWorldTile* AntWorld::findTile( const AntPosition& position )
{
  auto tile = std::find_if( std::begin( m_worldTiles ), std::end( m_worldTiles ),
                            [ &position ]( AntWorldTile*& t ){ return t && t->centre() == position; } );

  return ( tile != std::end( m_worldTiles ) ) ? *tile : nullptr;
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::updateAnts()
{
  m_foragingAnts = 0;
  m_gatheringAnts = 0;

  for( auto& ant : m_ants )
  {
    /* Take the next step first (we know where we are, but don't know where we are going). */
    ant->advance();

    if( ant->droppedPheromone() ) registerPheromone( ant->position(), ant->droppedPheromoneType(), ant );

    doGatheringAntLogic( ant );
    doAntPheromoneDeregistration( ant );
  }

  doDeadAntLogic();
  doForagingAntLogic();
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::updatePheromones()
{
  m_pheromones.erase( std::remove_if( std::begin( m_pheromones ), std::end( m_pheromones ),
                                      []( SharedPherPtr& p ){ return p->evaporated(); } ),
                      std::end( m_pheromones ) );

  for( auto& pher : m_pheromones ) pher->update();
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::doGatheringAntLogic( const SharedAntPtr& ant )
{
  if( ant->isGathering() )
  {
    m_gatheringAnts++;

    if( ant->shortestPath().size() < m_currentShortestPath.size() ||
        m_currentShortestPath.empty() )
    {
      m_currentShortestPath = ant->shortestPath();
    }
  }
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::doForagingAntLogic()
{
  m_foragingAnts = std::count_if( std::begin( m_ants ), std::end( m_ants ),
                                  []( SharedAntPtr& a ){ return a->isForaging(); } );
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::doDeadAntLogic()
{
  m_deadAnts += std::count_if( std::begin( m_ants ), std::end( m_ants ),
                               []( SharedAntPtr& a ){ return a->isDead(); } );

  m_ants.erase( std::remove_if( std::begin( m_ants ), std::end( m_ants ),
                                []( SharedAntPtr& a ){ return a->isDead(); } ),
                std::end( m_ants ) );
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::doAntPheromoneDeregistration( const SharedAntPtr& ant )
{
  for( auto& pos : ant->deRegisteredPheromonePositions() )
  {
    AntPheromone* pheromone = findPheromone( pos );
    if( pheromone ) pheromone->deRegisterAnt( ant );
  }
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::setEvaporationRate( double evaporationRate )
{
  m_evaporationRate = evaporationRate;
  for( auto& pher : m_pheromones ) pher->setEvaporationRate( m_evaporationRate );
}

/*--------------------------------------------------------------------------------------*/

int AntWorld::antCount() const
{
  return m_ants.size();
}

/*--------------------------------------------------------------------------------------*/

int AntWorld::foragingAnts() const
{
  return m_foragingAnts;
}

/*--------------------------------------------------------------------------------------*/

int AntWorld::gatheringAnts() const
{
  return m_gatheringAnts;
}

/*--------------------------------------------------------------------------------------*/

int AntWorld::deadAnts() const
{
  return m_deadAnts;
}

/*--------------------------------------------------------------------------------------*/

int AntWorld::shortestPathLength() const
{
  return m_currentShortestPath.empty() ? INT_MAX : m_currentShortestPath.size();
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::setPheromonesEnabled( bool enable )
{
  m_pheromoneEnabled = enable;
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::setSmartPheromonesEnabled( bool enable )
{
  m_smartPheromoneEnabled = enable;
}

/*--------------------------------------------------------------------------------------*/

void AntWorld::setMaxNodesRemembered( int maxNodesRemembered )
{
  m_maxNodesRemembered = maxNodesRemembered;
  for( auto& ant : m_ants ) ant->setMaxNodesRemembered( m_maxNodesRemembered );
}

/*--------------------------------------------------------------------------------------*/
