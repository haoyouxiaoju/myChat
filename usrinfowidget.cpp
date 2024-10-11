#include "usrinfowidget.h"

usrInfoWidget::usrInfoWidget(QWidget *parent)
    : QDialog(parent)
{
    layout = new QGridLayout();
    layout->setSpacing(10);
    layout->setContentsMargins(25,25,25,25);
    layout->setAlignment(Qt::AlignTop|Qt::AlignLeft);
    this->setLayout(layout);
    this->setMinimumSize(280, 188);
    //  将此界面设置为无标题栏的窗口 和 弹出式窗口
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setStyleSheet("QPushButton{ border-radius:5px;background-color:transparent; } \
                         QLabel{ font:bold 17px '宋体'; text-align:center; } \
                         QLineEdit { font: none 13px '宋体'; text-align:left;border:none;background-color:rgb(243, 243, 243) }");
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
    layout->addWidget(userName_SubmitButton, 1, 5);
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
    layout->addWidget(desc_SubmitButton, 2, 5);
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
    layout->addWidget(phone_SubmitButton, 3, 5);
    phone_SubmitButton->hide();

    phone_code_edit = new QLineEdit();
    sendCode_button = new QPushButton();



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



    //设置lineEdit 只读
    id_edit->setReadOnly(true);
    userName_edit->setReadOnly(true);
    desc_edit->setReadOnly(true);
    phone_edit->setReadOnly(true);




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

