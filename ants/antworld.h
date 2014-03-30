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

#ifndef ANTWORLD_H
#define ANTWORLD_H

#include "antpheromone.h"
#include "antworldtile.h"
#include <vector>

/*--------------------------------------------------------------------------------------*/

class AntBot;
class AntPosition;

/*--------------------------------------------------------------------------------------*/

/*! \brief An abstract base class representing the sim world that is responsible for the creation and
 *  maintenance of AntBots, AntPheromones and AntWorldTiles as well as the synchronization of the
 *  interaction of all objects.
 *
 *  Since AntBots, AntPheromones and AntWorldTiles are independent of any specific graphics framework,
 *  users must implement \sa createAnt, \sa createPheromone and \sa createWorldTile in their derived
 *  class in order to create their graphical derived objects correctly.
 *
 *  Also, since timers are platform-dependent, users must ensure that the \sa tick function
 *  gets called at regular intervals to ensure that all sim objects remain synchronized.
 */

class AntWorld
{
public:
  /*! Destructor. */
  virtual ~AntWorld();

  /*! Synchronizes ant movement and pheromone evaporation. Call this function at regular
   *  intervals to ensure that everything (ant movement, pheromone evaporation, etc)
   *  remains coordinated (think in terms of cycles/clock ticks). */
  void tick();

  /*! Creates and registers an ant spawned at "position".  This function calls
   *  "createAnt" in order to populate the registry list and furthermore tells the
   *  ant whether or not it must react to smart pheromones (default is "true").
   *
   *  \sa createAnt
   *  \sa setSmartPheromonesEnabled
   */
  void registerAnt( const AntPosition& position );

  /*! Creates and registers a world tile at "position". This function calls
   *  "createWorldTile" in order to populate the registry list.
   *
   *  \sa createWorldTile
   */
  void registerWorldTile( const AntPosition& position, AntWorldTile::TileType type );

  /*! Deletes all ants currently in the registry, sets all the ant counters (dead, foraging, gathering)
   *  back to zero and the shortest path to INT_MAX. */
  void resetAntRegister();

  /*! Deletes all pheromones currently in the registry. */
  void resetPheromoneRegister();

  /*! Deletes all world tiles currently in the registry. */
  void resetWorldTileRegister();

  /*! Sets the desired pheromone evaporation rate. */
  void setEvaporationRate( double evaporationRate );

  /*! Returns the number of ants currently in the registry.
   *  \sa foragingAnts
   *  \sa gatheringAnts
   *  \sa deadAnts */
  int antCount() const;

  /*! Returns the number of ants currently foraging.
   *  \sa gatheringAnts
   *  \sa deadAnts
   *  \sa antCount */
  int foragingAnts() const;

  /*! Returns the number of ants currently gathering.
   *  \sa foragingAnts
   *  \sa deadAnts
   *  \sa antCount */
  int gatheringAnts() const;

  /*! Returns the number of ants that have died so far.
   *  \sa gatheringAnts
   *  \sa foragingAnts
   *  \sa antCount */
  int deadAnts() const;

  /*! Returns the current, known shortest path length (INT_MAX while no ant has found any target(s)). */
  int shortestPathLength() const;

  /*! Enables pheromones (default) or disables pheromones (ants will not react
   *  to pheromone in their environment). */
  void setPheromonesEnabled( bool enable );

  /*! Enables smart pheromones (default) or disables smart pheromones ("Hazard" and other
   *  smart pheromones will be disabled). */
  void setSmartPheromonesEnabled( bool enable );

  /*! Sets the maximum number of nodes that ants should "remember" at any given time. */
  void setMaxNodesRemembered( int maxNodesRemembered );

protected:
  /*! Constructor. */
  AntWorld();

  /*! Creates a new AntBot to add to the registry.
   *  \sa registerAnt */
  virtual AntBot* createAnt( const AntPosition& position ) = 0;

  /*! Creates a new AntPheromone to add to the registry.
   *  \sa registerPheromone */
  virtual AntPheromone* createPheromone( const AntPosition& position, AntPheromone::PheromoneType type ) = 0;

  /*! Creates a new AntWorldTile to add to the registry.
   *  \sa registerWorldTile */
  virtual AntWorldTile* createWorldTile( const AntPosition& position, AntWorldTile::TileType type ) = 0;

private:
  using SharedAntPtr = std::shared_ptr< AntBot >;
  using SharedPherPtr = std::shared_ptr< AntPheromone >;

  /*! AntWorld is not copyable. */
  AntWorld( const AntWorld& ) = delete;

  /*! AntWorld is not assignable. */
  AntWorld& operator=( const AntWorld& ) = delete;

  /*! Returns nullptr if no pheromone is found at "position". */
  AntPheromone* findPheromone( const AntPosition& position );

  /*! Returns nullptr if no tile is found at "position". */
  AntWorldTile* findTile( const AntPosition& position );

  /*! Called on each tick to update the ant registry. */
  void updateAnts();

  /*! Called on each tick to update the pheromone registry. */
  void updatePheromones();

  /*! Update details regarding gathering ants. */
  void doGatheringAntLogic( const SharedAntPtr &ant );

  /*! Update details regarding foraging ants. */
  void doForagingAntLogic();

  /*! Update details regarding foraging ants. */
  void doDeadAntLogic();

  /*! Creates and registers a pheromone dropped at "position" by "ant". This function calls
   *  "createPheromone" in order to populate the registry list.
   *
   *  \warning this function does nothing when pheromones are disabled.
   *
   *  \sa createPheromone
   *  \sa setPheromonesEnabled
   */
  void registerPheromone( const AntPosition& position, AntPheromone::PheromoneType type, SharedAntPtr& ant );

  /*! Deregisters "ant" from all registered pheromones. */
  void doAntPheromoneDeregistration( const SharedAntPtr &ant );

private:
  int m_foragingAnts;
  int m_gatheringAnts;
  int m_deadAnts;

  double m_evaporationRate;

  bool m_pheromoneEnabled;
  bool m_smartPheromoneEnabled;

  unsigned int m_maxNodesRemembered;

  std::vector< SharedAntPtr > m_ants;
  std::vector< SharedPherPtr > m_pheromones;
  std::vector< AntWorldTile* > m_worldTiles;
  std::vector< AntPosition > m_currentShortestPath;
};

#endif // ANTWORLD_H