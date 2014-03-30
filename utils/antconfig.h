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

#ifndef ANTCONFIG_H
#define ANTCONFIG_H

/*--------------------------------------------------------------------------------------*/

/*! All configurable values are stored in this namespace.  This is probably not ideal in terms
 *  of access (obviously some values are available to classes that do not need them), but it makes
 *  it easier to change values quickly if everything is in one place. */

namespace AntConfig
{
  const double AntSize = 18.0;          /*!< pixels (ant square size) */
  const double TileSize = 20.0;         /*!< pixels (world tile side) */
  const double PheromoneSize = 10.0;    /*!< pixels (pheromone diameter) */

  /*! Instead of calculating probabilities in percentages, we assign an integer (chosen according
   *  to specific criteria) from within the probability range to the specific event (in this case
   *  the probability of an ant selecting any particular tile). */
  const unsigned int ProbabilityRange = 10000000;

  /*! The probability penalty for recently visited tiles (the probability range will be
   *  divided by this factor whenever a recently visited node is encountered). */
  const unsigned int RecentlyVisitedPenalty = 1000000;

  /*! The effective "0.0" against which a pheromone's current strength is measured in order
   *  to determine whether or not it has evaporated. This value is also assigned to tiles
   *  with no pheromone when AntBot calculates their probability of selection (this means that
   *  tiles WITH pheromone will always have a higher probability of selection than tiles WITHOUT
   *  since giving this value to a tile without pheromone will put it on par with a tile whose
   *  pheromone has evaporated).
   *
   *  \warning This value must NEVER be negative.
   */
  const double PheromoneMin = 0.01;
}

/*--------------------------------------------------------------------------------------*/

#endif // ANTCONFIG_H