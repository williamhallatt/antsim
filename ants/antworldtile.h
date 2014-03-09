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

#ifndef ANTWORLDTILE_H
#define ANTWORLDTILE_H

#include "utils/antposition.h"
#include "ants/antpheromone.h"

#include <memory>

/*--------------------------------------------------------------------------------------*/

/*! \brief This class represents the sim's 2D world squares (tiles) and is designed to be
 *  independent of any specific graphics framework.
 *
 *  As an abstract base class, users must therefore ensure that they inherit from it and
 *  implement \sa updateGraphics in order to apply graphical changes when the tile type is
 *  changed.
 */

class AntWorldTile
{
public:
  using SharedPheromonePtr = std::shared_ptr< const AntPheromone >;

  enum TileType
  {
    Path,   /*!< The tile forms part of an allowed path. */
    Wall,   /*!< The tile forms part of an obstructive wall. */
    Hazard, /*!< The tile forms part of an insurmountable and deadly hazard. */
    Food,   /*!< The tile is a search target. */
    Spawn,  /*!< The tile is an ant spawn point. */
    None
  };

  /*! Destructor. */
  virtual ~AntWorldTile();

  /*! Return the position at the centre of the tile. Returns the default coordinate
   *  ( 0.0, 0.0) if no centre position is set.
   *
   *  \sa setCentre */
  const AntPosition& centre() const;

  /*! Sets the tile's centre position to "centre".
   *
   *  \sa centre */
  void setCentre( const AntPosition& centre );

  /*! Sets the tile's relevant type.  This function will also call updateGraphics (unless
   *! tileType is NONE, in which case nothing is done, the member is also not updated).
   *
   *  \sa tileType
   *  \sa updateGraphics */
  void setTileType( TileType tileType );

  /*! Returns the tile's relevant type.
   *
   *  \sa setTileType */
  TileType tileType() const;

  /*! Sets the pheromone associated with this tile (i.e. the pheromone that's been "dropped"
   *  onto it).  Only one pheromone can be associated with each tile.
   *
   *  \sa deRegisterPheromone
   *  \sa tilePheromoneType
   *  \sa tilePheromoneStrength
   */
  void registerPheromone( SharedPheromonePtr pher );

  /*! Returns the pheromone type if the tile has a pheromone on it, "AntPheromone::None" if not.
   *
   *  \sa registerPheromone
   *  \sa deRegisterPheromone
   */
  AntPheromone::PheromoneType tilePheromoneType() const;

  /*! Returns the pheromone strength if the tile has a pheromone on it, "AntConfig::PheromoneMin" if not
   *  (the value returned will always be >= 0.0)
   *
   *  \sa registerPheromone
   *  \sa deRegisterPheromone
   */
  double tilePheromoneStrength() const;

protected:
  /*! Constructs an AntWorldTile with ( 0.0, 0.0 ) as centre coordinate and "Wall" as type. */
  explicit AntWorldTile();

  /*! Implement this function to update the tile's graphical representation when the tile type changes.
   *
   *  \sa setTileType
   */
  virtual void updateGraphics( TileType tileType ) = 0;

private:
  /*! AntWorldTiles are not copyable. */
  AntWorldTile( const AntWorldTile& ) = delete;

  /*! AntWorldTiles are not assignable. */
  AntWorldTile& operator=( const AntWorldTile& ) = delete;

  TileType m_type;
  AntPosition m_centre;
  std::weak_ptr< const AntPheromone > m_pheromone;
};

#endif // ANTWORLDTILE_H