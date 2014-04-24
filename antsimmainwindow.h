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

#ifndef ANTSIMMAINWINDOW_H
#define ANTSIMMAINWINDOW_H

#include <QDateTime>
#include <QMainWindow>

/*--------------------------------------------------------------------------------------*/

class GraphicsAntWorldScene;

/*--------------------------------------------------------------------------------------*/

namespace Ui
{
  class AntSimMainWindow;
}

/*--------------------------------------------------------------------------------------*/

class AntSimMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  /*! Constructor. */
  explicit AntSimMainWindow( QWidget* parent = 0 );

  /*! Destructor. */
  ~AntSimMainWindow();

private slots:
  /*! Initialises the world scene. */
  void initialise();

  /*! Opens an existing world.
   * \sa save
   * \sa saveAs */
  void open();

  /*! Saves the current world.
   * \sa open
   * \sa saveAs */
  void save();

  /*! Saves the current world under a different file name.
   *  \sa open
   *  \sa save */
  void saveAs();

  /*! Switches the state of the active tile type. */
  void buttonClicked( int button );

  /*! Starts the sim with the number of ants specified in the
   *  relevant spin box.
   *  \sa spawn */
  void startStopSim();

  /*! Spawns ants at the same rate as they advance, this is purely
   *  cosmetic in that it prevents new ants spawning on top of each
   *  other.
   *  \sa startStopSim */
  void spawn();

  /*! Advances all the ants to their next position. */
  void advance();

  /*! Resets the sim. */
  void reset();

  /*! Sets the pheromone evaporation rate. */
  void setEvaporationRate( double evaporationRate );

  /*! Enables pheromones (default) or disables pheromones (ants will not react
   *  to pheromone in their environment). */
  void togglePheromones( bool enable );

  /*! Enables smart pheromones (default) or disables smart pheromones ("Hazard" and other
   *  smart pheromones will be disabled). */
  void toggleSmartPheromones( bool enable );

  /*! Sets the maximum number of nodes that ants should "remember" at any given time. */
  void setMaxNodesRemembered( int maxNodesRemembered );

private:
  /*! Sets the info fields (gathering, dead, etc). */
  void setAntStats();

  /*! Loads the user selected world. */
  void loadWorld( QString fileName );

  Ui::AntSimMainWindow* ui;
  GraphicsAntWorldScene* m_scene;

  QTimer* m_synchTimer;
  QTime m_totalTimer;
  QTime m_elapsedTime;
  QString m_fileName;
  bool m_stopped;
};

#endif // ANTSIMMAINWINDOW_H
