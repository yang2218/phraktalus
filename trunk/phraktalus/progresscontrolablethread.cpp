/****************************************************************************************
 * Copyright (c) 2009 López, Gabriel Orlando <glopez@alumnos.exa.unicen.edu.ar>         *
 * Copyright (c) 2009 Rossi, Matías Javier <marossi@alumnos.exa.unicen.edu.ar>          *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 3 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#include "progresscontrolablethread.h"

ProgressControlableThread::ProgressControlableThread(QObject * parent) :
        QThread(parent),
        totalIterations(0),
        currentIteration(0)
{
}

void ProgressControlableThread::setTotalIterations(unsigned long t)
{
    totalIterations = t;
}

void ProgressControlableThread::iterationCompleted()
{
    emit progressUpdated((++currentIteration*100)/totalIterations);
}

unsigned long ProgressControlableThread::getTotalIterations() const
{
    return totalIterations;
}

unsigned long ProgressControlableThread::getCurrentIteration() const
{
    return currentIteration;
}

