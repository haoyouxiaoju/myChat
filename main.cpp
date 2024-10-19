#include "mainwidget.h"

#include <QApplication>
#include "model/data.h"
#include "framelesswidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget* w = MainWidget::getInstance();
    //LOG()<<"hello";
    w->show();
    //FramelessWidget* w = new FramelessWidget();
    //w->show();

    return a.exec();
}
