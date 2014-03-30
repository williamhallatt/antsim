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

#ifndef ANTBOT_H
#define ANTBOT_H

#include "antpheromone.h"
#include "utils/antposition.h"
#include <vector>
#include <memory>

/*--------------------------------------------------------------------------------------*/

class AntGraph;
class AntWorldTile;

/*--------------------------------------------------------------------------------------*/

/*! \brief An abstract base class responsible for all logic related to individual ant bots and
 *  designed to be independent of any specific graphics framework.
 *
 *  Since this class is independent of any graphical representation and since AntBots
 *  are necessarily dependent on their environment for all foraging and
 *  gathering activities, users of this class must therefore implement a number of
 *  functions that would be dependent on their graphics framework of choice:
 *
 *  \sa queryTerrain
 *  \sa updateGraphics
 *  \sa showFoundGraphics
 *  \sa showForageGraphics
 */

class AntBot
{
public:
  /*! Destructor. */
  virtual ~AntBot();

  /*! Advances the ant to the next best node. */
  void advance();

  /*! Returns the ant's position relative to its world. */
  const AntPosition& position() const;

  /*! Returns true if the ant just dropped some pheromone.  Calling this function
   *  resets the dropped pheromone flag (i.e. additional calls will return "false").
   *
   *  \sa droppedPheromoneType
   *  \sa droppedPheromoneStrength
   */
  bool droppedPheromone() const;

  /*! Returns the type of the last pheromone dropped.
   *
   *  \sa droppedPheromone
   *  \sa droppedPheromoneStrength
   */
  AntPheromone::PheromoneType droppedPheromoneType() const;

  /*! The returned value is dependent on the length of the ant's tour as per the ACO
   *  algorithm ( 1 / length ).  This function will return 0.0 if the ant is not dropping
   *  pheromone.
   *
   *  \sa droppedPheromone
   *  \sa droppedPheromoneType
   */
  double droppedPheromoneStrength() const;

  /*! Ants need to know which pheromones they are contributing to on each iteration (in other
   *  words, all pheromones registered with an ant will receive a pheromone update on each
   *  iteration).  When the ant's shortest path changes, the old pheromones are "forgotten" and
   *  a new list is started.
   *
   *  \sa deRegisteredPheromonePositions
   */
  void registerPheromonePosition( const AntPosition& pheromonePosition );

  /*! Returns the pheromones from which this particular ant must be deregistered (returns
   *  and empty vector if no pheromones are tagged for deregistration).
   *
   *  \sa registerPheromone
   */
  const std::vector< AntPosition >& deRegisteredPheromonePositions();

  /*! Returns the number of steps that the ant's current position is from the target
   *  (returns INT_MAX when the ant is foraging/dead). */
  int stepsFromTarget() const;

  /*! Returns the ant's internal shortest path or an empty vector if the ant is still foraging. */
  const std::vector< AntPosition >& shortestPath() const;

  /*! Returns "true' if the ant is foraging (hasn't found the target yet). */
  bool isForaging() const;

  /*! Returns "true" if ant is dead. */
  bool isDead() const;

  /*! Returns "true" if ant is gathering (returning to source from target or having found food). */
  bool isGathering() const;

  /*! Tells the ant's internal graph to keep track of of "x" nr of last nodes visited. */
  void setMaxNodesRemembered( unsigned int maxNodesRemembered);

protected:
  /*! Constructor. */
  explicit AntBot( const AntPosition& position );

  /*! Implement this function to return a list of AntWorldTiles neighbouring the
   *  ant's current "position".
   *
   *  \sa update
   */
  virtual std::vector< const AntWorldTile* > queryTerrain( const AntPosition& position ) = 0;

  /*! Implement this function to update the graphics with the new position (in other words,
   *  this function should move the ant's graphical representation to "position").
   *
   *  \sa update
   */
  virtual void updateGraphics( const AntPosition& position ) = 0;

  /*! Implement if you wish to change the graphical representation of the ant once it
   *  has discovered a food item/target.
   *
   *  \sa showForageGraphics
   */
  virtual void showFoundGraphics() = 0;

  /*! Implement if you wish to change the visual representation of the ant to its
   *  standard foraging state.
   *
   *  \sa showFoundGraphics
   */
  virtual void showForageGraphics() = 0;

  /*! Updates the ant's position and determines the new position's neighbours
   *  before advancing the ant to the new position.
   *
   *  \sa queryTerrain
   *  \sa updateGraphics
   */
  void updatePosition( const AntPosition& position );

private:
  /*! Returns the next position chosen.
   *
   *  \sa calculateTileProbabilityMax
   *  \sa calculateNeighbourProbabilities
   */
  const AntPosition& determineNextPosition();

  /*! Returns a vector of the all the neighbouring tiles' probability of selection.
   *
   *  \sa determineNextPosition
   */
  std::vector< int > calculateNeighbourProbabilities();

  /*! Calculates and returns the maximum probability that "tile" will be selected based
   *  on the Ant System probability equation and AntSim's probability heuristics.
   *
   *  See:
   *
   *  Blum, C., 2005. Ant colony optimization: Introduction and recent trends.
   *              Physics of Life reviews, 2(4), pp.353–373.
   *
   *  Chandra Mohan, B. & Baskaran, R., 2012. A survey: Ant Colony Optimization based recent research
   *              and implementation on several engineering domain. Expert Systems with Applications, 39(4), pp.4618–4627.
   *
   *  Dorigo, M., Caro, G.D. & Gambardella, L.M., 1999. Ant algorithms for discrete optimization.
   *              Artificial life, 5(2), pp.137–172.
   *
   *  Dorigo, M. & Socha, K., 2006. An introduction to ant colony optimization. Handbook of approximation
   *              algorithms and metaheuristics, pp.26–1.
   *
   *  /sa determineNextPosition
  */
  int calculateTileProbabilityMax( const AntWorldTile* tile ) const;

  /*! Keep looking for the target.
   *
   *  \sa foundFood
   *  \sa encounteredHazard
   */
  void forage();

  /*! Returns "true" if "tile" is a Food source.
   *
   *  \sa forage
   *  \sa encounteredHazard
   */
  bool foundFood( const AntWorldTile* tile );

  /*! Returns "true" if the ant encountered a Hazard tile. When the ant comes
   *  across a Hazard, it will (unfortunately) die...but it's OK, they
   *  get reincarnated and feel no pain.
   *
   *  \sa forage
   *  \sa foundFood
   */
  bool encounteredHazard( const AntWorldTile* tile );

  /*! Gather food (moving from target to source initialising tiles with pheromone on the way). */
  void gather();

  enum AntState
  {
    Foraging,         /*!< Initial state where ant is searching for the target. */
    Gathering,        /*!< Ant returning to nest after finding the target (initialising pheromone on the way). */
    Dead,             /*!< Ant encountered a hazard and died. */
    DroppingPheromone /*!< Ant has found the food target (at least once) but is once again searching for additional routes. */
  };

  /*! Sets the ant's state and calls showFoundGraphics and showForageGraphics
   *  (in order to enforce user-defined graphics preferences). */
  void setAntState( AntState state );

private:
  /*! AntBots are not copyable. */
  AntBot( const AntBot& ) = delete;

  /*! AntBots are not assignable. */
  AntBot& operator=( const AntBot& ) = delete;

  std::unique_ptr< AntGraph > m_graph;
  AntPosition m_position;
  AntState m_state;

  AntPheromone::PheromoneType m_pheromoneType;
  mutable bool m_droppedPheromone;
  bool m_doPheromoneDeregistration;

  double m_neighbourPheromoneSum;
  double m_pheromoneStrength;

  bool m_returningToSource;
  int m_stepsFromTarget;
  int m_shortestPathLength;

  std::vector< const AntWorldTile* > m_neighbours;
  std::vector< AntPosition > m_pheromones;               // keep track of registered pheromones
  std::vector< AntPosition > m_deRegisteredPheromones;   // keep track of deregistered pheromones
};

#endif // ANTBOT_H