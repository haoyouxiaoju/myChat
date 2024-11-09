#include "mainwidget.h"

#include <QApplication>
#include "model/data.h"
 #include "framelesswidget.h"
#include "test.qpb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget* w = MainWidget::getInstance();
    //contacts::PeopleInfo peopleinfo;
    //peopleinfo.setAge(12);
    //LOG()<<peopleinfo.age();
    //LOG()<<"hello";
     //w->show();
    //FramelessWidget* w = new FramelessWidget();
    //w->resize(200, 200);
    w->show();

    return a.exec();
}
