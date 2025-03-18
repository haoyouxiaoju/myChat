#include "messageeditarea.h"
#include "messagehistorywidget.h"
#include "mainwidget.h"

#include "toast.h"

using namespace model;

MessageEditArea::MessageEditArea(QWidget *parent)
    : QWidget{parent},owner(parent)
{
    
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(mainLayout);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->setSpacing(5);
    topLayout->setContentsMargins(10, 0, 0, 0);
    //输入框上方的功能按钮 
    QWidget* topWidget = new QWidget();
    topWidget->setFixedHeight(27);
    topWidget->setObjectName("edit-topWidget");
    topWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    topWidget->setLayout(topLayout);
    topWidget->setStyleSheet("QPushButton{  border:none;background-color:transparent;border-radius:3px;} #edit-topWidget{border-top:1px solid rgb(236, 236, 236)}");
    //发送图片
    sendImageBtn = new QPushButton();
    sendImageBtn->setIconSize(QSize(21, 21));
    sendImageBtn->setFixedSize(25, 25);
    sendImageBtn->setIcon(QIcon(":/resource/images/tupian.png"));
    topLayout->addWidget(sendImageBtn,0, Qt::AlignLeft|Qt::AlignVCenter);

    //发送文件
    sendFileBtn = new QPushButton();
    sendFileBtn->setIconSize(QSize(21, 21));
    sendFileBtn->setFixedSize(25, 25);
    sendFileBtn->setIcon(QIcon(":/resource/images/wenjianjia.png"));
    topLayout->addWidget(sendFileBtn,0, Qt::AlignLeft|Qt::AlignVCenter);
    
    //发送语言
    sendSoundBtn = new QPushButton();
    sendSoundBtn->setIconSize(QSize(21, 21));
    sendSoundBtn->setFixedSize(25, 25);
    sendSoundBtn->setIcon(QIcon(":/resource/images/huatong.png"));
    topLayout->addWidget(sendSoundBtn,0,  Qt::AlignLeft|Qt::AlignVCenter);

    //查看历史记录
    showHitstoryBtn = new QPushButton();
    showHitstoryBtn->setIconSize(QSize(25, 25));
    showHitstoryBtn->setFixedSize(25, 25);
    showHitstoryBtn->setIcon(QIcon(":/resource/images/lishijilu.png"));
    topLayout->addWidget(showHitstoryBtn,0, Qt::AlignLeft|Qt::AlignVCenter);

    mainLayout->addWidget(topWidget,Qt::AlignTop|Qt::AlignLeft);

    //输入框
    edit = new QPlainTextEdit();
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit->setStyleSheet("QPlainTextEdit{border:none;background-color:transparent;font-size:14px;padding:10px}");
    edit->verticalScrollBar()->setStyleSheet("QScroolBar::vertical{width:2px;background-color:rgb(46,46,45);}");
    mainLayout->addWidget(edit, 0);


    //发送按钮
    QWidget* bottomWidget = new QWidget();
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(0);
    bottomLayout->setContentsMargins(0, 0, 25, 15);
    bottomWidget->setLayout(bottomLayout);

    sendBtn = new QPushButton();
    sendBtn->setFixedSize(100, 30);
    sendBtn->setText("发送");
    sendBtn->setStyleSheet("QPushButton{border:none;background-color:rgb(233, 233, 233);border-radius:3px;color:rgb(7, 193, 96);font-size:17px}");
    bottomLayout->addWidget(sendBtn,0, Qt::AlignRight);
    mainLayout->addWidget(bottomWidget, 0);
        

    tiplabel = new QLabel();
    tiplabel->hide();


    connect(showHitstoryBtn, &QPushButton::clicked, this, [this]() {
        messageHistoryWidget* w  = messageHistoryWidget::getMessageHistoryWidget();
        w->show();

        });
    this->initSignalSlot();

}

void MessageEditArea::initSignalSlot()
{
    model::DataCenter* dataCenter = model::DataCenter::getInstance();

    //处理按钮点击
    connect(sendBtn, &QPushButton::clicked, this, &MessageEditArea::sendTextMessage);
    //
    connect(dataCenter, &DataCenter::sendMessageDone, this, &MessageEditArea::addSelfMessage);
    //发送消息
    connect(dataCenter, &DataCenter::sendMessageFailed, this, [](const QString& reason) {
        Toast::showMessage("发送消息失败:" + reason);
        });
    //处理服务端转发来的会话消息
    connect(dataCenter, &DataCenter::receiveMessageDone, this, &MessageEditArea::addOtherMessage);
}

void MessageEditArea::sendTextMessage()
{
    model::DataCenter* dataCenter = model::DataCenter::getInstance();
    
    if (dataCenter->getCurrentChatSessionId().isEmpty()) {
        LOG() << "当前未选中聊天会话,不发送任何消息";
        Toast::showMessage("当前未选中聊天会话,不发送任何消息");
        return;
    }

    QString content = edit->toPlainText().trimmed();
    if (content.isEmpty()) {
        LOG() << "输入框未空,不发送消息";
        return;
    }

    edit->setPlainText("");
    dataCenter->sendTextMessageAsync(dataCenter->getCurrentChatSessionId(), content);
}

void MessageEditArea::addSelfMessage(model::MessageType type, const QByteArray& body, const QString& extraInfo)
{
    DataCenter* dataCenter = DataCenter::getInstance();

    const Message& message = dataCenter->addMessage(Message::MakeMessage(type, dataCenter->getCurrentChatSessionId(), *dataCenter->getMySelf(), body, extraInfo));

    MainWidget* mainWidget = dynamic_cast<MainWidget*>(owner);
    MessageShowArea* messageShowArea = mainWidget->getMessageShowArea();
    messageShowArea->addItem(false, message);

    messageShowArea->scrollToEndLater();

    emit dataCenter->updateLastMessage(dataCenter->getCurrentChatSessionId());

}

void MessageEditArea::addOtherMessage(const model::Message& message)
{
    //把这个消息显示到界面
    MainWidget* mainWidget = dynamic_cast<MainWidget*>(owner);
    MessageShowArea* messageShowArea = mainWidget->getMessageShowArea();

    messageShowArea->addItem(true, message);

    messageShowArea->scrollToEndLater();
    Toast::showMessage("收到一条新消息");

    
}
