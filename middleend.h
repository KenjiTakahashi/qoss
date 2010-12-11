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
#include <QThread>

#include "backend.h"

//temp
#include <QDebug>

static backend *backend_handler = new backend(); // yeah, yeah

/* half-stoned class, no responsibility taken */
class QOSSWatcher : public QThread {
    Q_OBJECT
public:
    explicit QOSSWatcher(
            QList<bool> switches, /* [0] == values
                                     [1] == mute
                                     [2] == modes */
            int i_,
            string dev_,
            QObject *parent = 0);
    void run();
private:
    bool mute;
    int muteValue;
    int i;
    string dev;
signals:
    void muteUpdated(int);
};

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
    explicit QOSSTreeWidgetItem(QTreeWidgetItem *parent, QString string);
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
    explicit QOSSConfig(QWidget *parent = 0);
private:
    QHBoxLayout *layout;
public slots:
    void showQOSSWidget(QTreeWidgetItem*, QTreeWidgetItem*);
};

#endif // MIDDLEEND_H
