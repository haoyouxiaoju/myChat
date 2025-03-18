#include "usrinfowidget.h"
#include "debug.h"
#include "model/datacenter.h"
#include "model/data.h"

#include <QTimer>
#include <QFileDialog>
#include <QDir>


using namespace model;

usrInfoWidget::usrInfoWidget(QWidget *parent)
    : QDialog(parent)
{
    layout = new QGridLayout();
    layout->setSpacing(10);
    layout->setContentsMargins(25,25,25,25);
    layout->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    this->setLayout(layout);
    this->setMinimumSize(300, 200);
    //  将此界面设置为无标题栏的窗口 和 弹出式窗口
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setStyleSheet("QPushButton{ border-radius:5px;background-color:transparent; } \
                         QLabel{ font:bold 17px '宋体'; text-align:center; } \
                         QLineEdit { font: none 13px '宋体'; text-align:left;border:none;background-color:rgb(240, 240, 240) }");
    QIcon modifly(":/resource/images/bianji.png");
    QIcon submit(":/resource/images/queren.png");

    avatar = new QPushButton();
    avatar->setFixedSize(60,60);
    avatar->setIconSize(QSize(60, 60));
    layout->addWidget(avatar, 0, 0, 2, 2);

    id_label = new QLabel("序号");
    id_edit = new QLineEdit();
    layout->addWidget(id_label,0, 2);
    layout->addWidget(id_edit, 0, 3);

    userName_label = new QLabel("昵称");
    userName_edit = new QLineEdit();
    userName_ModifiyButton = new QPushButton();
    userName_SubmitButton = new QPushButton();
    userName_ModifiyButton->setIconSize(QSize(20, 20));
    userName_ModifiyButton->setIcon(modifly);
    userName_SubmitButton->setIconSize(QSize(20, 20));
    userName_SubmitButton->setIcon(submit);
    layout->addWidget(userName_label, 1, 2);
    layout->addWidget(userName_edit, 1, 3);
    layout->addWidget(userName_ModifiyButton, 1, 4);
    layout->addWidget(userName_SubmitButton, 1, 4);
    userName_SubmitButton->hide();


    desc_label = new QLabel("签名");
    desc_edit = new QLineEdit();
    desc_ModifiyButton = new QPushButton();
    desc_SubmitButton = new QPushButton();
    desc_ModifiyButton->setIconSize(QSize(20, 20));
    desc_ModifiyButton->setIcon(modifly);
    desc_SubmitButton->setIconSize(QSize(20, 20));
    desc_SubmitButton->setIcon(submit);
    layout->addWidget(desc_label, 2, 2);
    layout->addWidget(desc_edit, 2, 3);
    layout->addWidget(desc_ModifiyButton, 2, 4);
    layout->addWidget(desc_SubmitButton, 2, 4);
    desc_SubmitButton->hide();

    phone_label = new QLabel("电话");
    phone_edit = new QLineEdit();
    phone_ModifiyButton = new QPushButton();
    phone_SubmitButton = new QPushButton();
    phone_ModifiyButton->setIconSize(QSize(20, 20));
    phone_ModifiyButton->setIcon(modifly);
    phone_SubmitButton->setIconSize(QSize(20, 20));
    phone_SubmitButton->setIcon(submit);
    layout->addWidget(phone_label, 3, 2);
    layout->addWidget(phone_edit, 3, 3);
    layout->addWidget(phone_ModifiyButton, 3, 4);
    layout->addWidget(phone_SubmitButton, 3, 4);
    phone_SubmitButton->hide();

    phone_code_edit = new QLineEdit();
    sendCode_button = new QPushButton("获取");
    sendCode_button->setStyleSheet("QPushButton{border:1px solid;border-radius:0px;}");
    code_label = new QLabel("验证码");
    layout->addWidget(code_label, 4, 2);
    layout->addWidget(phone_code_edit, 4, 3);
    layout->addWidget(sendCode_button, 4, 4);

    phone_code_edit->hide();
    sendCode_button->hide();
    code_label->hide();



#if TEXT_UI
    //debug
    id_edit->setReadOnly(false);
    userName_edit->setReadOnly(false);
    desc_edit->setReadOnly(false);
    phone_edit->setReadOnly(false);

    avatar->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
    userName_edit->setText("haoyouxiaoju");
    desc_edit->setText("hello world");
    phone_edit->setText("10000000000");
    //debug_end
#endif

    model::DataCenter* dataCenter = model::DataCenter::getInstance();
    model::UserInfo* info = dataCenter->getMySelf();
    if (info != nullptr) {
        avatar->setIcon(dataCenter->getIcon(info->avatar));
        userName_edit->setText(info->nickname);
        desc_edit->setText(info->description);
        phone_edit->setText(info->phone);
    }


    //设置lineEdit 只读
    id_edit->setReadOnly(true);
    userName_edit->setReadOnly(true);
    desc_edit->setReadOnly(true);
    phone_edit->setReadOnly(true);


    initSlotsToSignals();

}

void usrInfoWidget::clickAvatar()
{
    QString filter = "Image files(*.png *jpeg *.jpg *.bmp)";

    QString imagePath = QFileDialog::getOpenFileName(this->parentWidget(), "选择头像", QDir::homePath(), filter);
    if (imagePath.isEmpty()) {
        LOG() << "未选中图片";
        return;
    }
    //读取图片内容
    QByteArray imageBytes = model::localFileToQByteArray(imagePath);
    //发送请求
    DataCenter* dataCenter = DataCenter::getInstance();
    connect(dataCenter, &DataCenter::changeAvatarDone, this, &usrInfoWidget::clickAvatar, Qt::UniqueConnection);
    dataCenter->changeAvatarAsync(imageBytes);
    


}

void usrInfoWidget::clickAvatarDone()
{
    DataCenter* dataCenter = DataCenter::getInstance();
    avatar->setIcon(dataCenter->getIcon(dataCenter->getMySelf()->avatar));
}

void usrInfoWidget::initSlotsToSignals()
{
    //修改用户昵称
    connect(userName_ModifiyButton, &QPushButton::clicked, this, &usrInfoWidget::clickNameModifyBtn);
    connect(userName_SubmitButton, &QPushButton::clicked, this, &usrInfoWidget::clickNameSubmitBtn);
    //修改用户签名
    connect(desc_ModifiyButton, &QPushButton::clicked, this, &usrInfoWidget::clickSignatureModifyBtn);
    connect(desc_SubmitButton, &QPushButton::clicked, this, &usrInfoWidget::clickSignatureSubmitBtn);

    //获取验证码 and 修改用户手机号
    connect(phone_ModifiyButton, &QPushButton::clicked, this, &usrInfoWidget::clickPhoneModifyBtn);
    connect(phone_SubmitButton, &QPushButton::clicked, this, &usrInfoWidget::clickPhoneSubmitBtn);
    connect(sendCode_button, &QPushButton::clicked, this, &usrInfoWidget::clickSendVerityCodeBtn);

    //修改头像
    connect(avatar, &QPushButton::clicked, this, &usrInfoWidget::clickAvatar);

}

void usrInfoWidget::clickNameModifyBtn()
{

    //设置lineEdit 可写
    userName_edit->setReadOnly(false);
    userName_ModifiyButton->hide();
    userName_SubmitButton->show();
}

void usrInfoWidget::clickNameSubmitBtn()
{
    //设置lineEdit 只读
    phone_edit->setReadOnly(true);
    phone_SubmitButton->hide();
    DataCenter* dataCenter = DataCenter::getInstance();

    const QString& newName = userName_edit->text();
    if (newName.isEmpty() ) {
        return;
    }
    if (newName == dataCenter->getMySelf()->nickname) {

        return this->clickNameSubmitBtnDone();
    }
    //
    connect(dataCenter, &DataCenter::changeNickNameDone, this, &usrInfoWidget::clickNameSubmitBtnDone, Qt::UniqueConnection);
    //  发送修改昵称请求
    dataCenter->changeNickNameAsync(newName);

    
}

void usrInfoWidget::clickNameSubmitBtnDone()
{
    userName_edit->setReadOnly(true);
    userName_SubmitButton->hide();
    userName_ModifiyButton->show();
   
}

void usrInfoWidget::clickSignatureModifyBtn()
{
    desc_edit->setReadOnly(false);
    desc_ModifiyButton->hide();
    desc_SubmitButton->show();

}

void usrInfoWidget::clickSignatureSubmitBtn()
{
    DataCenter* dataCentet = DataCenter::getInstance();
    const QString& newDesc = desc_edit->text();
    if (newDesc.isEmpty() ) {
        return;
    }
    if (newDesc == dataCentet->getMySelf()->description) {
        return this->clickSignatureSubmitBtnDone();
    }

    connect(dataCentet, &DataCenter::changeDescriptionDone, this, &usrInfoWidget::clickSignatureSubmitBtnDone, Qt::UniqueConnection);

    //
    dataCentet->changeDescriptionAsync(newDesc);
}

void usrInfoWidget::clickSignatureSubmitBtnDone()
{
    desc_edit->setReadOnly(true);
    desc_SubmitButton->hide();
    desc_ModifiyButton->show();
}

void usrInfoWidget::clickPhoneModifyBtn()
{
    phone_edit->setReadOnly(false);
    phone_ModifiyButton->hide();
    phone_SubmitButton->show();
    //显示验证码输入
    code_label->show();
    phone_code_edit->show();
    sendCode_button->show();

}

void usrInfoWidget::clickPhoneSubmitBtn()
{
    DataCenter* dataCenter = DataCenter::getInstance();
    //
    const QString& verifyCodeId = dataCenter->getVerifyCodeId();
    if (verifyCodeId.isEmpty()) {
        LOG() << "验证码尚未获取成功! 稍后再试!";
        return;
    }
    const QString& verifyCode = phone_code_edit->text();
    if (verifyCode.isEmpty()) {
        return;
    }
    connect(dataCenter, &DataCenter::changeUserPhoneDone, this, &usrInfoWidget::clickPhoneSubmitBtnDone, Qt::UniqueConnection);
    dataCenter->changePhoneAsync(this->phoneToChange, verifyCode, verifyCode);
}

void usrInfoWidget::clickPhoneSubmitBtnDone()
{
    phone_edit->setReadOnly(true);
    phone_ModifiyButton->show();
    phone_SubmitButton->hide();

    //显示验证码输入
    code_label->hide();
    phone_code_edit->hide();
    sendCode_button->hide();
}

void usrInfoWidget::clickSendVerityCodeBtn()
{
 const QString& phone = phone_edit->text();
    if (phone.isEmpty()) {
        return;
    }
    DataCenter* dataCenter = DataCenter::getInstance();
    if (phone == dataCenter->getMySelf()->phone) {
        
    }
    dataCenter->getVerifyCodeAsync(phone);
    //记录新手机号
    phoneToChange = phone;
    sendCode_button->setEnabled(false);
    time = 30;
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this,timer]() {
        if (time < 1) {
            sendCode_button->setText("发送验证码");
            sendCode_button->setEnabled(true);
            timer->stop();
            timer->deleteLater();
            return;
        }
        --time;
        sendCode_button->setText(QString::number(time) + "s");
        });
    timer->start(1000);


}


void usrInfoWidget::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::WindowActivate) {
	
	}
    if (QEvent::ActivationChange == event->type()) {
        if(QApplication::activeWindow() != this){
            this->close();
        }
    }
	 QWidget::changeEvent(event);
}

