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
