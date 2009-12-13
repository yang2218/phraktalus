# ****************************************************************************************
# * Copyright (c) 2009 López, Gabriel Orlando <glopez@alumnos.exa.unicen.edu.ar>         *
# * Copyright (c) 2009 Rossi, Matías Javier <marossi@alumnos.exa.unicen.edu.ar>          *
# *                                                                                      *
# * This program is free software; you can redistribute it and/or modify it under        *
# * the terms of the GNU General Public License as published by the Free Software        *
# * Foundation; either version 3 of the License, or (at your option) any later           *
# * version.                                                                             *
# *                                                                                      *
# * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
# * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
# * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
# *                                                                                      *
# * You should have received a copy of the GNU General Public License along with         *
# * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
# ****************************************************************************************

QT += opengl
TARGET = Phraktalus
TEMPLATE = app
RC_FILE = phraktalus.rc
SOURCES += main.cpp \
    phraktalus.cpp \
    glviewport.cpp \
    modelstacker.cpp \
    progresscontroller.cpp \
    progresscontrolablethread.cpp \
    vertex.cpp \
    randompositionaldeviation.cpp \
    midpointmodelsampler.cpp \
    elevationmodel.cpp \
    bintreetriangulation.cpp \
    imageparameters.cpp
HEADERS += phraktalus.h \
    glviewport.h \
    modelstacker.h \
    progresscontroller.h \
    progresscontrolablethread.h \
    vertex.h \
    triangle.h \
    randompositionaldeviation.h \
    positionaldeviation.h \
    midpointmodelsampler.h \
    matrix.h \
    elevationmodel.h \
    bintreetriangulation.h \
    operationcancelledexception.h \
    fileformatexception.h \
    elevationmodelfactory.h \
    imageparameters.h \
    resource.h
FORMS += phraktalus.ui \
    progresscontroller.ui \
    imageparameters.ui
RESOURCES += icons.qrc
