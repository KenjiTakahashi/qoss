/**
 * This is a part of qoss @ http://github.com/KenjiTakahashi/qoss/
 * Karol "Kenji Takahashi" Wozniak (C) 2010
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MIDDLEEND_H
#define MIDDLEEND_H

#define MONO 1
#define STEREO 2
#define OTHER 0

#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QStringList>
#include <QIcon>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>
#include <QTreeWidget>
#include <QList>

#include "backend.h"

//temp
#include <iostream>
#include <QDebug>

class QOSSWidget : public QGroupBox {
    Q_OBJECT
public:
    explicit QOSSWidget(
            unsigned int type_, /* MONO || STEREO || OTHER */
            bool peak_,
            unsigned int min,
            unsigned int max,
            QString title = "",
            QStringList modes = QStringList(),
            QWidget *parent = 0);
private:
    bool locked;
    unsigned int type;
    bool peak;
private slots:
    void setLocked(bool);
public slots:
    void updateValue(int);
};

class QOSSStructure : public QTreeWidget {
    Q_OBJECT
public:
    explicit QOSSStructure();
private:
    backend *b;
};

#endif // MIDDLEEND_H
