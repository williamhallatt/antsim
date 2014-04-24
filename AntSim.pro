# Copyright (c) 2013 by William Hallatt.
#
# This file forms part of "AntSim".
#
# The official website for this project is <http://www.goblincoding.com> and,
# although not compulsory, it would be appreciated if all works of whatever
# nature using this source code (in whole or in part) include a reference to
# this site.
#
# Should you wish to contact me for whatever reason, please do so via:
#
#                 <http://www.goblincoding.com/contact>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program (GNUGPL.txt).  If not, see
#
#                    <http://www.gnu.org/licenses/>

#-------------------------------------------------
#
# Project created by QtCreator 2013-04-20T15:57:59
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AntSim
TEMPLATE = app

SOURCES += main.cpp\
        antsimmainwindow.cpp \
    world/graphicsworldtile.cpp \
    utils/antgraph.cpp \
    ants/antbot.cpp \
    world/graphicsantitem.cpp \
    ants/antpheromone.cpp \
    world/graphicspheromoneitem.cpp \
    ants/antworldtile.cpp \
    utils/antposition.cpp \
    ants/antworld.cpp \
    world/antworldscene.cpp

HEADERS  += antsimmainwindow.h \
    world/graphicsworldtile.h \
    utils/antgraph.h \
    ants/antbot.h \
    world/graphicsantitem.h \
    utils/antconfig.h \
    ants/antpheromone.h \
    world/graphicspheromoneitem.h \
    ants/antworldtile.h \
    utils/antposition.h \
    ants/antworld.h \
    world/antworldscene.h

FORMS    += antsimmainwindow.ui

RESOURCES += \
    resources/resources.qrc

QMAKE_CXXFLAGS += -std=c++11
