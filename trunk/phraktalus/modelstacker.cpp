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

#include "modelstacker.h"
#include "elevationmodel.h"

ModelStacker::ModelStacker(QObject *parent) :
        QObject(parent),
        stack(list<ElevationModel*>()),
        current(stack.begin())
{
    emitSignals();
}

void ModelStacker::push(ElevationModel* m)
{
    while (*current != stack.back()) {
        delete stack.back();
        stack.pop_back();
    }
    stack.push_back(m);
    current++;
    emitSignals();
}

void ModelStacker::previous()
{
    current--;
    emitSignals();
}

void ModelStacker::next()
{
    current++;
    emitSignals();
}

void ModelStacker::clear()
{
    for (list<ElevationModel*>::iterator it = stack.begin();
         it != stack.end();
         it++)
        delete (*it);
    stack.clear();
    current = stack.begin();
    emitSignals();
}

ElevationModel* ModelStacker::top()
{
    if (current == stack.end()) return NULL;
    return (*current);
}

void ModelStacker::emitSignals()
{
    emit topChanged(top());
    emit hasPrevious((stack.size() > 1) && ( current != stack.begin()));
    emit hasNext((stack.size() > 1) && (*current != stack.back()));
    emit isEmpty(stack.empty());
}
