#include "mainwidget.h"

#include <QApplication>
#include "model/data.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget* w = MainWidget::getInstance();
    LOG()<<"hello";

    w->show();
    return a.exec();
}
