#include "mainwidget.h"

#include <QApplication>
#include "model/data.h"
#include "model/datacenter.h"
 #include "framelesswidget.h"
#include "test.qpb.h"
#include "loginwidget.h"
#include "network/netclient.h"

#define NETCLIENT_TEXT 1

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWidget* w = MainWidget::getInstance();
    //contacts::PeopleInfo peopleinfo;
    //peopleinfo.setAge(12);
    //LOG()<<peopleinfo.age();
    //LOG()<<"hello";
     //w->show();
    //FramelessWidget* w = new FramelessWidget();
    //w->setBorderColor(Qt::red);
    //w->resize(200, 200);

    LoginWidget* w = new LoginWidget();
    //LoginPhoneWidget* w = new LoginPhoneWidget();

    //model::DataCenter* datacebter = model::DataCenter::getInstance();

#ifdef NETCLIENT_TEXT
    NetClient netclient(nullptr);
    netclient.ping();
#endif
    w->show();

    return a.exec();
}

