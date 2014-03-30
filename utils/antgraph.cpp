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

#include "antgraph.h"
#include "antconfig.h"
#include <algorithm>

/*--------------------------------------------------------------------------------------*/

AntGraph::AntGraph( const AntPosition& start )
: m_maxNodesRemembered  ( 5 ),
  m_nodes               ( 1, start ),
  m_shortestPath        ( 1, start ),
  m_shortestPathReversed(),
  m_recentlyVisited     () {}

/*--------------------------------------------------------------------------------------*/

void AntGraph::addNode( const AntPosition& position )
{
  /* Keep track of two separate lists of nodes, the first is a list of ALL the nodes
   * ever visited and the second that of the nodes on the current shortest path.  The
   * reason for keeping the first list is that, even though ants may double back on
   * themselves, we'd still like to know if the nodes they travel along on the next
   * "iteration" of their travels has been previously visited in order to favour nodes
   * that have never been visited at all (e.g. go down a new fork, rather than simply
   * traversing the previous loop that ended in failure). */
  if( std::none_of( std::begin( m_nodes ), std::end( m_nodes ),
                    [ &position ]( AntPosition pos ) { return pos == position; } ) )
  {
    m_nodes.push_back( position );
  }
  else
  {
    /* If a node is already in the list, it means the ant has looped back on itself,
     * in other words, all the nodes encountered since we last visited this particular
     * node has been for naught and can be deleted.  This way, we ensure that the route
     * the ant travels between the starting point and the final destination remains the
     * shortest route for its particular graph. */
    auto it = std::find( std::begin( m_shortestPath ), std::end( m_shortestPath ), position );

    /* Remove the pointless nodes (the loop) from the vector. */
    m_shortestPath.resize( it - std::begin( m_shortestPath ) );
  }

  /* Regardless of what happens, add the node to the end of the shortest path (if the
   * node had been previously encountered, it would have been removed in the "else" part
   * above and will need to be re-added). */
  m_shortestPath.push_back( position );

  /* We don't want the same position to occur multiple times in the recently
   * visited list since this will skew the results when "recentlyVisited" is called. */
  if( !recentlyVisited( position ) )
  {
    if( m_recentlyVisited.size() < m_maxNodesRemembered )
    {
      m_recentlyVisited.push_back( position );
    }
    else
    {
      m_recentlyVisited.pop_front();
      m_recentlyVisited.push_back( position );
    }
  }
}

/*--------------------------------------------------------------------------------------*/

void AntGraph::setFoundTarget( const AntPosition& position )
{
  addNode( position );
  reverseShortestPath();
}

/*--------------------------------------------------------------------------------------*/

const std::vector< AntPosition >& AntGraph::shortestPath() const
{
  return m_shortestPathReversed;
}

/*--------------------------------------------------------------------------------------*/

bool AntGraph::recentlyVisited( const AntPosition& position ) const
{
  return std::any_of( std::begin( m_recentlyVisited ), std::end( m_recentlyVisited ),
                      [ &position ]( AntPosition pos ) { return pos == position; } );
}

/*--------------------------------------------------------------------------------------*/

void AntGraph::setMaxNodesRemembered( unsigned int maxNodesRemembered )
{
  m_maxNodesRemembered = maxNodesRemembered;
  m_recentlyVisited.resize( m_maxNodesRemembered );
}

/*--------------------------------------------------------------------------------------*/

void AntGraph::reverseShortestPath()
{
  m_shortestPathReversed.clear();
  m_shortestPathReversed.resize( m_shortestPath.size() );
  std::reverse_copy( std::begin( m_shortestPath ), std::end( m_shortestPath ), std::begin( m_shortestPathReversed ) );
}

/*--------------------------------------------------------------------------------------*/
