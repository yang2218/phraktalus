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

#ifndef MODELSTACKER_H
#define MODELSTACKER_H

#include <QObject>
#include <list>
using namespace std;

class ElevationModel;

class ModelStacker : public QObject
{

    Q_OBJECT

    signals:
        void topChanged(ElevationModel*);
        void hasPrevious(bool);
        void hasNext(bool);
        void isEmpty(bool);

    public:
        ModelStacker(QObject *parent = 0);
    public slots:
        void push(ElevationModel*);
        void previous();
        void next();
        void clear();
        ElevationModel* top();
        void emitSignals();
    private:
        list<ElevationModel*> stack;
        list<ElevationModel*>::iterator current;
};

#endif // MODELSTACKER_H
