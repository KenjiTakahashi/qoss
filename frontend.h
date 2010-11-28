#ifndef FRONTEND_H
#define FRONTEND_H

#include <QMainWindow>

namespace Ui {
    class frontend;
}

class frontend : public QMainWindow
{
    Q_OBJECT

public:
    explicit frontend(QWidget *parent = 0);
    ~frontend();

private:
    Ui::frontend *ui;
};

#endif // FRONTEND_H
