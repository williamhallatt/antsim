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

#ifndef ANTGRAPH_H
#define ANTGRAPH_H

#include "antposition.h"
#include <vector>
#include <list>

/*! \brief This class represents an AntBot's internal search graph.
 *
 *  As ants proceed with their foraging activities, this graph is constantly
 *  updated in order to maintain an internal shortest path (by excluding loops
 *  created by doubling back and so forth from the ants' search paths).
 */

class AntGraph
{
public:
  /*! Constructor. The node represented by "start" will be the goal node (the node
   *  to return to) once an ant has reached/discovered its target. */
  explicit AntGraph( const AntPosition& start );

  /*! Adds a node corresponding to "position" to the existing graph (if the position
   *  is already known, the nodes forming the "loop" from the last time the position was
   *  encountered until now is removed from the node list). */
  void addNode( const AntPosition& position );

  /*! Sets the target (food) node. A call to this function triggers the reversal of
   *  the traversed shortest path.
   *
   *  \sa shortestPath */
  void setFoundTarget( const AntPosition& position );

  /*! Returns the shortest path between target and source (first node will correspond to the
   *  target and the last to the starting point). A target node must have been
   *  specified or else an empty vector is returned.
   *
   *  \sa setFoundTarget */
  const std::vector< AntPosition >& shortestPath() const;

  /*! Returns true if "position" was one of "x" number of last
   *  nodes visited (see antconfig.h for the MaxNodesRemembered value). */
  bool recentlyVisited( const AntPosition& position ) const;

  /*! Sets the maximum number of nodes that AntGraph should consider when determining
   *  whether a position has been recently visited (default 5).
   *
   *  \sa recentlyVisited
   */
  void setMaxNodesRemembered( unsigned int maxNodesRemembered );

private:
  /*! Reverses the shortest path for the ant's return journey.
   *
   *  \sa shortestPath */
  void reverseShortestPath();

private:
  unsigned int m_maxNodesRemembered;

  std::vector< AntPosition > m_nodes;
  std::vector< AntPosition > m_shortestPath;
  std::vector< AntPosition > m_shortestPathReversed;
  std::list< AntPosition > m_recentlyVisited;
};

#endif // ANTGRAPH_H
