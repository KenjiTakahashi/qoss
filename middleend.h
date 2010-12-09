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

#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QIcon>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>
#include <QTreeWidget>
#include <QList>

#include "backend.h"

//temp
#include <QDebug>

class QOSSWidget : public QGroupBox {
    Q_OBJECT
public:
    explicit QOSSWidget(
            int type_, /* MONO || STEREO || OTHER */
            int peak_, /* MONO || STEREO || NONE */
            QMap<QString, int> is, /* ["values", "mute", "modes"] nodes */
            int min = 0,
            int max = 0,
            QString title = "",
            QStringList modes = QStringList(),
            QWidget *parent = 0);
private:
    bool locked;
    int type;
    int peak;
private slots:
    void setLocked(bool);
public slots:
    void updateValue(int);
};

class QOSSTreeWidgetItem : public QTreeWidgetItem {
public:
    explicit QOSSTreeWidgetItem(
            QTreeWidgetItem *parent,
            QString string,
            int i_ = 0,
            int mutei_ = 0,
            int modei_ = 0,
            int minvalue_ = 0,
            int maxvalue_ = 0,
            int type_ = OTHER, /* STEREO || MONO || OTHER */
            int peak_ = NONE, /* STEREO || MONO || NONE */
            QStringList modes_ = QStringList());
    void seti(int i_);
    int geti();
    void setmutei(int mutei_);
    int getmutei();
    void setmodei(int modei_);
    int getmodei();
    void setminvalue(int minvalue_);
    int getminvalue();
    void setmaxvalue(int maxvalue_);
    int getmaxvalue();
    void settype(int type_);
    int gettype();
    void setpeak(int peak_);
    int getpeak();
    void setmodes(QStringList modes_);
    QStringList getmodes();
private:
    int i;
    int mutei;
    int modei;
    int minvalue;
    int maxvalue;
    int type;
    int peak;
    QStringList modes;
};

class QOSSConfig : public QWidget {
    Q_OBJECT
public:
    explicit QOSSConfig();
private:
    backend *b;
    QHBoxLayout *layout;
public slots:
    void showQOSSWidget(QTreeWidgetItem*, QTreeWidgetItem*);
};

#endif // MIDDLEEND_H
