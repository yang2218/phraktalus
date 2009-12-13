/****************************************************************************************
 * Copyright (c) 2009 L�pez, Gabriel Orlando <glopez@alumnos.exa.unicen.edu.ar>         *
 * Copyright (c) 2009 Rossi, Mat�as Javier <marossi@alumnos.exa.unicen.edu.ar>          *
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

#ifndef PROGRESSCONTROLABLETHREAD_H
#define PROGRESSCONTROLABLETHREAD_H

#include <QThread>

class ProgressControlableThread : public QThread
{
    Q_OBJECT

    signals:
        void progressUpdated(int);
    private:
        unsigned long totalIterations;
        unsigned long currentIteration;
    protected:
        void setTotalIterations(unsigned long);
        void iterationCompleted();
    public:
        ProgressControlableThread(QObject *parent = 0);
        unsigned long getTotalIterations() const;
        unsigned long getCurrentIteration() const;
};

#endif // PROGRESSCONTROLABLETHREAD_H
