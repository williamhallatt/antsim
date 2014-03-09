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

#ifndef ANTPHEROMONE_H
#define ANTPHEROMONE_H

#include "utils/antposition.h"
#include <vector>
#include <memory>

/*--------------------------------------------------------------------------------------*/

class AntBot;

/*--------------------------------------------------------------------------------------*/

/*! \brief This class represents the ant pheromones AntBots use to communicate with each other
 *  via the process of stigmergy and is designed to be independent of any specific graphics framework.
 *
 *  As an abstract base class, users must therefore ensure that they inherit from it and
 *  implement \sa updateGraphics in order to apply graphical effects associated with pheromone state
 *  and type.
 */

class AntPheromone
{
public:
  using SharedAntPtr = std::shared_ptr< const AntBot >;

  enum PheromoneType
  {
    Found,    /*!< Pheromone is on a path leading to the target. */
    Hazard,   /*!< Pheromone is adjacent to a hazard. */
    None
  };

  /*! Destructor. */
  virtual ~AntPheromone();

  /*! Updates the pheromone state (evaporation counters, graphics, etc).*/
  void update();

  /*! Returns the pheromone's type.
   *
   *  \sa setPheromoneType */
  PheromoneType pheromoneType() const;

  /*! Sets the pheromone's type.
   *
   *  \sa pheromoneType */
  void setPheromoneType( PheromoneType type );

  /*! Returns the density/strength of the pheromone (this function will never return negative numbers).
   *
   *  \sa setEvaporationRate
   *  \sa evaporated */
  double pheromoneStrength() const;

  /*! Sets the desired pheromone evaporation rate.
   *
   *  \sa evaporated
   *  \sa pheromoneStrength */
  void setEvaporationRate( double evaporationRate );

  /*! Returns true when the pheromone has "evaporated".
   *
   *  \sa setEvaporationRate
   *  \sa pheromoneStrength */
  bool evaporated() const;

  /*! Adds an ant reference for use when updating this pheromone's strength.  According to
   *  the ACO algorithm, pheromone evaporation and addition is dependent on the number of
   *  ants with a specific pheromone on their path and also the path length of the individual
   *  ants in question.  Since the pheromone strength is dependent on the ants' internal shortest
   *  paths, pheromones need to know which ants are dropping pheromone on them (hence the necessity
   *  of maintaining a registry). This function won't add the ant if it is already known.
   *
   *  \sa deRegisterAnt */
  void registerAnt( SharedAntPtr ant );

  /*! When an ant is no longer contributing to a specific pheromone (e.g. when they have found
   *  a shorter path), it needs to be removed from the pheromone's ant registry (to ensure that
   *  they no longer influence the pheromone strength calculation).
   *
   *  \sa registerAnt */
  void deRegisterAnt( const SharedAntPtr& ant );

  /*! Returns the pheromone's position. */
  const AntPosition& position() const;

protected:
  /*! Constructs a pheromone at "position" of "type". */
  explicit AntPheromone( const AntPosition& position, PheromoneType type );

  /*! Re-implement for unique graphics associated with pheromone state and type. */
  virtual void updateGraphics() = 0;

private:
  /*! AntPheromones are not copyable. */
  AntPheromone( const AntPheromone& ) = delete;

  /*! AntPheromones are not assignable. */
  AntPheromone& operator=( const AntPheromone& ) = delete;

private:
  PheromoneType m_type;
  AntPosition m_position;
  double m_strength;
  double m_evaporationRate;

  using WeakAntPtr = std::weak_ptr< const AntBot >;
  std::vector< WeakAntPtr > m_ants;   // keep track of ants with this pheromone on their path
};

#endif // ANTPHEROMONE_H