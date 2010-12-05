#include "middleend.h"

QOSSSlider::QOSSSlider(
        unsigned int type_,
        unsigned int min,
        unsigned int max,
        QStringList modes,
        QWidget *parent) : QWidget(parent) {
    // icons are not actually working :/
    type = type_;
    QGridLayout *layout = new QGridLayout();
    QProgressBar *peak = new QProgressBar();
    peak->setOrientation(Qt::Vertical);
    layout->addWidget(peak, 0, 0, 2, 1);
    QSlider *slider = new QSlider();
    slider->setRange(min, max);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(updateLeft(int)));
    layout->addWidget(slider, 1, 1);
    layout->setAlignment(slider, Qt::AlignJustify);
    QPushButton *mute = new QPushButton(QIcon::fromTheme("audio-volume-muted"), ""); // what icon name here?:|
    mute->setCheckable(true);
    mute->setFixedWidth(24);
    layout->addWidget(mute, 0, 1);
    if(type == STEREO) {
        QPushButton *lock = new QPushButton(QIcon::fromTheme("lock"), "");
        lock->setCheckable(true);
        lock->setFixedWidth(24);
        connect(lock, SIGNAL(toggled(bool)), this, SLOT(setLocked(bool)));
        layout->addWidget(lock, 0, 2);
        QSlider *slider_right = new QSlider();
        slider_right->setRange(min, max);
        connect(slider_right, SIGNAL(valueChanged(int)), this, SLOT(updateRight(int)));
        layout->addWidget(slider_right, 1, 2);
        layout->setAlignment(slider_right, Qt::AlignJustify);
        QProgressBar *peak_right = new QProgressBar();
        peak_right->setOrientation(Qt::Vertical);
        layout->addWidget(peak_right, 0, 3, 2, 4);
    }
    if(!modes.empty()) {
        QComboBox *box = new QComboBox();
        box->addItems(modes);
        box->setFixedWidth(110);
        layout->addWidget(box, 2, 0, 3, 4);
    }
    this->setLayout(layout);
}
void QOSSSlider::setLocked(bool state) {
    locked = state;
}
void QOSSSlider::updateLeft(int value) {
    if(type == STEREO && locked) {
        // hardcoded at value, because itemAtPosition returned 0x0 no matter what
        ((QSlider*)((QGridLayout*)this->sender()->parent())->children().at(5))->setValue(value);
    }
    //remember to write actual value to driver!
}
void QOSSSlider::updateRight(int value) {
    if(type == STEREO && locked) {
        ((QSlider*)((QGridLayout*)this->sender()->parent())->children().at(2))->setValue(value);
    }
}

middleend::middleend()
{
}
