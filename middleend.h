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

#include <QWidget>
#include <QSlider>
#include <QGridLayout>
#include <QComboBox>
#include <QStringList>
#include <QIcon>
#include <QPushButton>
#include <QProgressBar>

//temp
#include <iostream>
#include <QDebug>

class QOSSSlider : public QWidget {
    Q_OBJECT
public:
    explicit QOSSSlider(
            unsigned int type_, /* MONO || STEREO */
            unsigned int min,
            unsigned int max,
            QStringList modes,
            QWidget *parent = 0);
private:
    bool locked;
    unsigned int type;
private slots:
    void setLocked(bool);
public slots:
    void updateLeft(int);
    void updateRight(int);
};

class middleend
{
public:
    middleend();
};

#endif // MIDDLEEND_H
