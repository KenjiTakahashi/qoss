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
        unsigned int type_,
        bool peak_,
        unsigned int min,
        unsigned int max,
        QString title,
        QStringList modes,
        QWidget *parent) : QGroupBox(parent) {
    // icons are not actually working :/
    this->setTitle(title);
    type = type_;
    peak = peak_;
    locked = false;
    QHBoxLayout *layout = new QHBoxLayout();
    if(peak) {
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
        if(peak) {
            QProgressBar *peak_right = new QProgressBar();
            peak_right->setOrientation(Qt::Vertical);
            layout->addWidget(peak_right);
        }
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
        if(peak) {
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
        if(peak) {
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

QOSSStructure::QOSSStructure() {
    this->setColumnCount(1);
    this->setHeaderHidden(true);
    b = new backend();
    map<string, map<string, map<string, ossdata> > > results;
    b->get_local_dev_info(results);
    string error = b->get_error();
    if(!error.empty()) {
        cout << error; // do sth real
    } else {
        QList<QTreeWidgetItem*> items;
        for(map<string, map<string, map<string, ossdata> > >::iterator it = results.begin(); it != results.end(); ++it) {
            QTreeWidgetItem *parent = new QTreeWidgetItem(QStringList(QString::fromStdString(it->first)));
            map<string, map<string, ossdata> > results2 = it->second;
            for(map<string, map<string, ossdata> >::iterator it2 = results2.begin(); it2 != results2.end(); ++it2) {
                QTreeWidgetItem *subparent = new QTreeWidgetItem(parent, QStringList(QString::fromStdString(it2->first)));
                map<string, ossdata> results3 = it2->second;
                for(map<string, ossdata>::iterator it3 = results3.begin(); it3 != results3.end(); ++it3) {
                    if(!(it3->first).empty()) {
                        new QTreeWidgetItem(subparent, QStringList(QString::fromStdString(it3->first)));
                    }
                }
            }
            items.append(parent);
        }
        this->insertTopLevelItems(0, items);
    }
}
