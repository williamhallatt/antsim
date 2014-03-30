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

#include "antbot.h"
#include "antworldtile.h"
#include "utils/antgraph.h"
#include "utils/antconfig.h"

#include <cstdlib>
#include <climits>
#include <numeric>
#include <algorithm>
#include <map>

/*--------------------------------------------------------------------------------------*/

AntBot::AntBot( const AntPosition& position )
: m_graph                    ( new AntGraph( position ) ),
  m_position                 ( position ),
  m_state                    ( Foraging ),
  m_pheromoneType            ( AntPheromone::None ),
  m_droppedPheromone         ( false ),
  m_doPheromoneDeregistration( false ),
  m_neighbourPheromoneSum    ( 0.0 ),
  m_pheromoneStrength        ( 0.0 ),
  m_returningToSource        ( false ),
  m_stepsFromTarget          ( 0 ),
  m_shortestPathLength       ( INT_MAX ),
  m_neighbours               (),
  m_pheromones               (),
  m_deRegisteredPheromones   () {}

/*--------------------------------------------------------------------------------------*/

AntBot::~AntBot()
{
  // m_graph is unique_ptr
}

/*--------------------------------------------------------------------------------------*/

void AntBot::advance()
{
  m_doPheromoneDeregistration = false;

  /* Ant states turn to "DroppingPheromone" as soon as they have found the food source for
   * the first time.  This enables them to continue dropping pheromone on the tiles leading
   * to the food source, but also to "restart" the foraging experience (so that they do not
   * end up continuously running between nest and food). */
  switch( m_state )
  {
    case Gathering:
      gather();
      break;
    case Foraging:            // deliberate fall-through
    case DroppingPheromone:
      forage();
      break;
    case Dead:
      break;
  }
}

/*--------------------------------------------------------------------------------------*/

const AntPosition& AntBot::position() const
{
  return m_position;
}

/*--------------------------------------------------------------------------------------*/

void AntBot::setAntState( AntBot::AntState state )
{
  m_state = state;

  switch( m_state )
  {
    case Gathering:
      showFoundGraphics();
      break;
    default:
      showForageGraphics();
  }
}

/*--------------------------------------------------------------------------------------*/

void AntBot::setMaxNodesRemembered( unsigned int maxNodesRemembered )
{
  m_graph->setMaxNodesRemembered( maxNodesRemembered );
}

/*--------------------------------------------------------------------------------------*/

bool AntBot::droppedPheromone() const
{
  bool temp = m_droppedPheromone;
  m_droppedPheromone = false;
  return temp;
}

/*--------------------------------------------------------------------------------------*/

AntPheromone::PheromoneType AntBot::droppedPheromoneType() const
{
  return m_pheromoneType;
}

/*--------------------------------------------------------------------------------------*/

double AntBot::droppedPheromoneStrength() const
{
  return ( m_state == DroppingPheromone || m_state == Gathering ) ? m_pheromoneStrength : 0.0;
}

/*--------------------------------------------------------------------------------------*/

void AntBot::registerPheromonePosition( const AntPosition& pheromonePosition )
{
  if( std::none_of( std::begin( m_pheromones ), std::end( m_pheromones ),
                    [ &pheromonePosition ]( AntPosition pos ){ return pos == pheromonePosition; } ) )
  {
    m_pheromones.push_back( pheromonePosition );
  }
}

/*--------------------------------------------------------------------------------------*/

const std::vector< AntPosition >& AntBot::deRegisteredPheromonePositions()
{
  if( !m_deRegisteredPheromones.empty() &&
      !m_doPheromoneDeregistration )
  {
    m_deRegisteredPheromones.clear();
  }

  return m_deRegisteredPheromones;
}

/*--------------------------------------------------------------------------------------*/

int AntBot::stepsFromTarget() const
{
  return ( m_state == DroppingPheromone || m_state == Gathering ) ? m_stepsFromTarget : INT_MAX;
}

/*--------------------------------------------------------------------------------------*/

const std::vector< AntPosition >& AntBot::shortestPath() const
{
  return m_graph->shortestPath();
}

/*--------------------------------------------------------------------------------------*/

bool AntBot::isForaging() const
{
  return m_state == Foraging;
}

/*--------------------------------------------------------------------------------------*/

bool AntBot::isDead() const
{
  return m_state == Dead;
}

/*--------------------------------------------------------------------------------------*/

bool AntBot::isGathering() const
{
  return m_state == Gathering || m_state == DroppingPheromone;
}

/*--------------------------------------------------------------------------------------*/

void AntBot::updatePosition( const AntPosition& position )
{
  m_position = position;

  /* In case the query always returns neighbours in a specific order, e.g. N, E, S, W,
    add another touch of randomised variation to the entire procedure. */
  m_neighbours.clear();
  m_neighbours = queryTerrain( m_position );
  std::random_shuffle( std::begin( m_neighbours ), std::end( m_neighbours ) );

  updateGraphics( m_position );
}

/*--------------------------------------------------------------------------------------*/

const AntPosition& AntBot::determineNextPosition()
{
  /* If, for whatever reason, a tile has no neighbours (you never know how the world
   * might be constructed), return the current position. */
  if( m_neighbours.size() == 0 ) return m_position;

  std::vector< int > probabilities = calculateNeighbourProbabilities();

  /* Calculate the normalisation factor (sum of probabilities must ultimately add to 1.0). */
  double normalisationFactor = static_cast< double >( AntConfig::ProbabilityRange ) /
                               std::accumulate( probabilities.begin(), probabilities.end(), 0 );

  /* Now we calculate the normalised probabilities for each tile and stick these normalised values
   * into a map against their original, pre-sorted indices (otherwise we will not know which index
   * went with which tile initially). */
  std::multimap< int /*value*/, int /*index*/ > indices;

  for( std::vector< int >::size_type i = 0; i < probabilities.size(); ++i )
  {
    probabilities[ i ] = static_cast< int >( probabilities[ i ] * normalisationFactor );
    indices.insert( std::make_pair( probabilities[ i ], i ) );
  }

  /* Sort the values from lowest to highest, randomly select a value in the
   * probability range and see in which interval the selected probability falls. */
  std::sort( probabilities.begin(), probabilities.end() );
  int selectedProbability = rand() % AntConfig::ProbabilityRange + 1;   // don't want to select 0 prob (disallowed) tiles

  std::vector< int >::size_type index = 0;

  while( index < probabilities.size() - 1 )
  {
    if( selectedProbability > probabilities[ index ] )
    {
      ++index;
    }
    else
    {
      break;
    }
  }

  /* Use the convenience mapping to obtain the index of the tile associated with the
   * selected probability and use that index (remember the indices of "probabilities"
   * correspond to those of m_neighbours) to find the selected tile's centre position. */
  return m_neighbours[ ( indices.find( probabilities[ index ] ) )->second ]->centre();
}

/*--------------------------------------------------------------------------------------*/

int AntBot::calculateTileProbabilityMax( const AntWorldTile* tile ) const
{
  /* Skip all tiles where ants have gone...and died...shame...*/
  if( tile->tilePheromoneType() == AntPheromone::Hazard ) return 0;

  switch( tile->tileType() )
  {
    case AntWorldTile::Path:    // deliberate fall-through
    case AntWorldTile::Spawn:   // deliberate fall-through
    case AntWorldTile::Food:
      {
        /* Prefer nodes moving away from the last visited node by penalising recently visited nodes. */
        double finalRange = AntConfig::ProbabilityRange;
        if( m_graph->recentlyVisited( tile->centre() ) ) finalRange /= AntConfig::RecentlyVisitedPenalty;

        double probabilityMax = ( tile->tilePheromoneStrength() / m_neighbourPheromoneSum ) * finalRange;
        if( probabilityMax < 1.0 ) probabilityMax = rand() % static_cast< int >( finalRange ) + 1;  // +1 since we can't return 0 values for non-wall/non-hazard tiles

        return static_cast< int >( probabilityMax );
      }
    case AntWorldTile::Wall:    // deliberate fall-through
    case AntWorldTile::Hazard:  // deliberate fall-through
    case AntWorldTile::None:
      return 0;
  }

  return 0;
}

/*--------------------------------------------------------------------------------------*/

std::vector< int > AntBot::calculateNeighbourProbabilities()
{
  m_neighbourPheromoneSum = std::accumulate( std::begin( m_neighbours ), std::end( m_neighbours ), 0.0,
                                             []( double sum, const AntWorldTile*& tile ) { return sum + tile->tilePheromoneStrength(); } );

  /* First we determine the selection probability maximum for each tile. The indices
   * of the probability maximums will coincide with the indices of their respective
   * tiles in m_neighbours. */
  std::vector< int > probabilities;
  for( auto& neighbour : m_neighbours ) probabilities.push_back( calculateTileProbabilityMax( neighbour ) );

  /* Introduce a "shake" factor to distinguish duplicate values from each other. The range is
   * a bit of an arbitrary calculation since the value must be relatively small compared to
   * the entire probability range, yet cannot be fixed in case the parameters change. */
  int shakeRange = AntConfig::ProbabilityRange / AntConfig::RecentlyVisitedPenalty;

  for( auto probability : probabilities )
  {
    /* We don't want to shake 0 values since these must remain 0 (Walls and Hazards should
     * definitely not be traversable). */
    if( probability > 0 )
    {
      /* Check if we have duplicates. */
      if( std::count( std::begin( probabilities ), std::end( probabilities ), probability ) > 1 )
      {
        /* Find the duplicates of this specific probability and apply the shake
          factor to all of them. */
        auto it = std::find( std::begin( probabilities ), std::end( probabilities ), probability );

        while( it != std::end( probabilities ) )
        {
          *it += rand() % shakeRange;
          it = std::find( it + 1, std::end( probabilities ), probability );
        }
      }
    }
  }

  return probabilities;
}

/*--------------------------------------------------------------------------------------*/

void AntBot::forage()
{
  /* If any of the neighbouring tiles is a food or hazard tile, we're done. */
  for( auto& tile : m_neighbours ) if( foundFood( tile ) || encounteredHazard( tile ) ) return;

  auto position = determineNextPosition();
  updatePosition( position );
  m_graph->addNode( position );  // "addNode" automatically checks for duplicates
}

/*--------------------------------------------------------------------------------------*/

void AntBot::gather()
{
  if( m_returningToSource )
  {
    if( m_stepsFromTarget < m_shortestPathLength )
    {
      auto position = m_graph->shortestPath()[ m_stepsFromTarget ];
      updatePosition( position );
      ++m_stepsFromTarget;
    }
    else
    {
      m_stepsFromTarget = 0;
      m_returningToSource = false;
    }

    m_droppedPheromone = true;
  }
  else
  {
    setAntState( DroppingPheromone );
  }
}

/*--------------------------------------------------------------------------------------*/

bool AntBot::foundFood( const AntWorldTile* tile )
{
  if( tile->tileType() == AntWorldTile::Food )
  {
    m_graph->setFoundTarget( tile->centre() );

    /* See if the new source/target path is shorter than the previous before we
     * change the state, etc to avoid switching to a worse solution. */
    int newShortestPathLength = static_cast< int >( m_graph->shortestPath().size() );

    if( m_shortestPathLength > newShortestPathLength )
    {
      setAntState( Gathering );
      m_shortestPathLength = newShortestPathLength;
      m_pheromoneStrength = ( 1.0 / static_cast< double >( m_shortestPathLength ) );
      m_pheromoneType = AntPheromone::Found;
      m_returningToSource = true;
      m_deRegisteredPheromones.clear();
      m_deRegisteredPheromones = m_pheromones;
      m_pheromones.clear();
      m_doPheromoneDeregistration = true;
      return true;
    }
  }

  return false;
}

/*--------------------------------------------------------------------------------------*/

bool AntBot::encounteredHazard( const AntWorldTile* tile )
{
  if( tile->tileType() == AntWorldTile::Hazard )
  {
    setAntState( Dead );
    m_droppedPheromone = true;
    m_pheromoneType = AntPheromone::Hazard;
    return true;
  }

  return false;
}

/*--------------------------------------------------------------------------------------*/
