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

#include "progresscontroller.h"
#include "ui_progresscontroller.h"
#include "progresscontrolablethread.h"


ProgressController::ProgressController(ProgressControlableThread *thread,
                                       QWidget *parent,
                                       QString title,
                                       QString text) :
    QDialog(parent),
    ui(new Ui::ProgressController),
    thread(thread)
{
    ui->setupUi(this);
    setWindowTitle(title);
    ui->textLabel->setText(text);
    connect(thread, SIGNAL(progressUpdated(int)), ui->progressBar, SLOT(setValue(int)));
    connect(thread, SIGNAL(finished()), this, SLOT(accept()));
}

ProgressController::~ProgressController()
{
    delete ui;
}

void ProgressController::on_cancelButton_clicked()
{
    thread->blockSignals(true);
    thread->wait();
    thread->terminate();
    reject();
}
