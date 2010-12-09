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
#include "middleend.h"

QOSSWidget::QOSSWidget(
        int type_,
        int peak_,
        QMap<QString, int> is,
        int min,
        int max,
        QString title,
        QStringList modes,
        QWidget *parent) : QGroupBox(parent) {
    // icons are not actually working :/
    this->setTitle(title);
    type = type_;
    peak = peak_;
    locked = false;
    QHBoxLayout *layout = new QHBoxLayout();
    if(peak != NONE) {
        QProgressBar *peak = new QProgressBar();
        peak->setOrientation(Qt::Vertical);
        layout->addWidget(peak);
    }
    QVBoxLayout *leftLayout = new QVBoxLayout();
    QPushButton *mute = new QPushButton(QIcon::fromTheme("audio-volume-muted"), ""); // what icon name here?:|
    mute->setCheckable(true);
    mute->setFixedWidth(24);
    leftLayout->addWidget(mute);
    QSlider *slider = new QSlider();
    slider->setRange(min, max);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(updateValue(int)));
    leftLayout->addWidget(slider);
    leftLayout->setAlignment(slider, Qt::AlignJustify);
    layout->addLayout(leftLayout);
    if(type == STEREO) {
        QVBoxLayout *rightLayout = new QVBoxLayout();
        QPushButton *lock = new QPushButton(QIcon::fromTheme("lock"), "");
        lock->setCheckable(true);
        lock->setFixedWidth(24);
        connect(lock, SIGNAL(toggled(bool)), this, SLOT(setLocked(bool)));
        rightLayout->addWidget(lock);
        QSlider *slider_right = new QSlider();
        slider_right->setRange(min, max);
        connect(slider_right, SIGNAL(valueChanged(int)), this, SLOT(updateValue(int)));
        rightLayout->addWidget(slider_right);
        rightLayout->setAlignment(slider_right, Qt::AlignJustify);
        layout->addLayout(rightLayout);
    }
    if(peak == STEREO) {
        QProgressBar *peak_right = new QProgressBar();
        peak_right->setOrientation(Qt::Vertical);
        layout->addWidget(peak_right);
    }
    layout->addStretch();
    if(!modes.empty()) {
        QComboBox *box = new QComboBox();
        box->addItems(modes);
        QVBoxLayout *boxLayout = new QVBoxLayout();
        layout->setContentsMargins(0, 0, 0, 0);
        QWidget *lay = new QWidget();
        lay->setLayout(layout);
        boxLayout->addWidget(lay);
        boxLayout->setAlignment(lay, Qt::AlignJustify);
        boxLayout->addWidget(box);
        QHBoxLayout *boxLayout2 = new QHBoxLayout();
        boxLayout2->addLayout(boxLayout);
        boxLayout2->addStretch(1);
        this->setLayout(boxLayout2);
    } else {
        this->setLayout(layout);
    }
}
void QOSSWidget::setLocked(bool state) {
    locked = state;
    if(locked) {
        QObjectList children = this->sender()->parent()->children();
        QSlider *left;
        QSlider *right;
        if(peak != NONE) {
            left = (QSlider*)children.at(3);
            right = (QSlider*)children.at(5);
        } else {
            left = (QSlider*)children.at(2);
            right = (QSlider*)children.at(4);
        }
        if(left->value() < right->value()) {
            left->setValue(right->value());
        } else {
            right->setValue(left->value());
        }
    }
}
void QOSSWidget::updateValue(int value) {
    if(type == STEREO && locked) {
        // hardcoded at values are bad, but they're good anyway.
        QObjectList children = this->sender()->parent()->children();
        if(peak != NONE) {
            if(this->sender() == children.at(3)) {
                ((QSlider*)children.at(5))->setValue(value);
            } else {
                ((QSlider*)children.at(3))->setValue(value);
            }
        } else {
            if(this->sender() == children.at(2)) {
                ((QSlider*)children.at(4))->setValue(value);
            } else {
                ((QSlider*)children.at(2))->setValue(value);
            }
        }
    }
    //remember to write actual value to driver!
}

QOSSTreeWidgetItem::QOSSTreeWidgetItem(
        QTreeWidgetItem *parent,
        QString string,
        int i_,
        int mutei_,
        int modei_,
        int minvalue_,
        int maxvalue_,
        int type_,
        int peak_,
        QStringList modes_) : QTreeWidgetItem(parent, QStringList(string)) {
    i = i_;
    mutei = mutei_;
    modei = modei_;
    minvalue = minvalue_;
    maxvalue = maxvalue_;
    type = type_;
    peak = peak_;
    modes = modes_;
}
/* Let the horror begin! */
void QOSSTreeWidgetItem::seti(int i_) {
    i = i_;
}
int QOSSTreeWidgetItem::geti() {
    return i;
}
void QOSSTreeWidgetItem::setmutei(int mutei_) {
    mutei = mutei_;
}
int QOSSTreeWidgetItem::getmutei() {
    return mutei;
}
void QOSSTreeWidgetItem::setmodei(int modei_) {
    modei = modei_;
}
int QOSSTreeWidgetItem::getmodei() {
    return modei;
}
void QOSSTreeWidgetItem::setminvalue(int minvalue_) {
    minvalue = minvalue_;
}
int QOSSTreeWidgetItem::getminvalue() {
    return minvalue;
}
void QOSSTreeWidgetItem::setmaxvalue(int maxvalue_) {
    maxvalue = maxvalue_;
}
int QOSSTreeWidgetItem::getmaxvalue() {
    return maxvalue;
}
void QOSSTreeWidgetItem::settype(int type_) {
    type = type_;
}
int QOSSTreeWidgetItem::gettype() {
    return type;
}
void QOSSTreeWidgetItem::setpeak(int peak_) {
    peak = peak_;
}
int QOSSTreeWidgetItem::getpeak() {
    return peak;
}
void QOSSTreeWidgetItem::setmodes(QStringList modes_) {
    modes = modes_;
}
QStringList QOSSTreeWidgetItem::getmodes() {
    return modes;
}
/* and now let it end */

QOSSConfig::QOSSConfig() {
    b = new backend();
    map<string, map<string, map<string, ossdata> > > results;
    b->get_local_dev_info(results);
    string error = b->get_error();
    if(!error.empty()) {
        cout << error; // do sth real
    } else {
        QTreeWidget *tree = new QTreeWidget();
        tree->setColumnCount(1);
        tree->setHeaderHidden(true);
        connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
                this, SLOT(showQOSSWidget(QTreeWidgetItem*, QTreeWidgetItem*)));
        QList<QTreeWidgetItem*> items;
        for(map<string, map<string, map<string, ossdata> > >::iterator it = results.begin(); it != results.end(); ++it) {
            QTreeWidgetItem *parent = new QTreeWidgetItem(QStringList(QString::fromStdString(it->first)));
            map<string, map<string, ossdata> > results2 = it->second;
            for(map<string, map<string, ossdata> >::iterator it2 = results2.begin(); it2 != results2.end(); ++it2) {
                QOSSTreeWidgetItem *subparent = new QOSSTreeWidgetItem(parent, QString::fromStdString(it2->first));
                map<string, ossdata> results3 = it2->second;
                for(map<string, ossdata>::iterator it3 = results3.begin(); it3 != results3.end(); ++it3) {
                    QStringList modes;
                    for(unsigned int i = 0; i < it3->second.modes.size(); ++i) {
                        modes.append(QString::fromStdString(it3->second.modes[i]));
                    }
                    if(!(it3->first).empty()) {
                        new QOSSTreeWidgetItem(
                                subparent,
                                QString::fromStdString(it3->first),
                                it3->second.i,
                                it3->second.mute_i,
                                it3->second.mode_i,
                                it3->second.minvalue,
                                it3->second.maxvalue,
                                it3->second.type,
                                it3->second.peak,
                                modes);
                    } else {
                        subparent->seti(it3->second.i);
                        subparent->setmutei(it3->second.mute_i);
                        subparent->setmodei(it3->second.mode_i);
                        subparent->setminvalue(it3->second.minvalue);
                        subparent->setmaxvalue(it3->second.maxvalue);
                        subparent->settype(it3->second.type);
                        subparent->setpeak(it3->second.peak);
                        subparent->setmodes(modes);
                    }
                }
            }
            items.append(parent);
        }
        tree->insertTopLevelItems(0, items);
        layout = new QHBoxLayout();
        QGroupBox *previewBox = new QGroupBox("Preview");
        previewBox->setFixedWidth(200);
        previewBox->setLayout(layout);
        QHBoxLayout *layout2 = new QHBoxLayout();
        layout2->addWidget(tree);
        layout2->addWidget(previewBox);
        this->setLayout(layout2);
    }
}
void QOSSConfig::showQOSSWidget(QTreeWidgetItem *item, QTreeWidgetItem*) {
    if(item->childCount() == 0) {
        QOSSTreeWidgetItem* temp = (QOSSTreeWidgetItem*)item;
        QMap<QString, int> is;
        is["values"] = temp->geti();
        is["mute"] = temp->getmutei();
        is["modes"] = temp->getmodei();
        QOSSWidget *widget = new QOSSWidget(temp->gettype(),
                temp->getpeak(),
                is,
                temp->getminvalue(),
                temp->getmaxvalue(),
                temp->text(0),
                temp->getmodes());
        QLayoutItem *itemm;
        if((itemm = layout->takeAt(0)) != 0) {
            delete itemm->widget();
            delete itemm;
        }
        layout->addWidget(widget);
    }
}
