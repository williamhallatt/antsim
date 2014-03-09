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

#include "antposition.h"

/*--------------------------------------------------------------------------------------*/

AntPosition::AntPosition( double x, double y )
: m_x( x ),
  m_y( y ) {}

/*--------------------------------------------------------------------------------------*/

void AntPosition::setX( double x )
{
  m_x = x;
}

/*--------------------------------------------------------------------------------------*/

double AntPosition::x() const
{
  return m_x;
}

/*--------------------------------------------------------------------------------------*/

void AntPosition::setY( double y )
{
  m_y = y;
}

/*--------------------------------------------------------------------------------------*/

double AntPosition::y() const
{
  return m_y;
}

/*--------------------------------------------------------------------------------------*/

bool AntPosition::operator==( const AntPosition& other ) const
{
  return m_x == other.x() &&
         m_y == other.y();
}

/*--------------------------------------------------------------------------------------*/
