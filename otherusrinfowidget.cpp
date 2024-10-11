#include "otherusrinfowidget.h"

OtherUsrInfoWidget::OtherUsrInfoWidget(QWidget *parent)
	: QDialog(parent)
{
	layout = new QGridLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(15, 0, 15, 0);
	layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	this->setLayout(layout);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);

    this->setMinimumSize(200, 100);
	//this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setStyleSheet("QPushButton{ border-radius:5px;background-color:transparent;border:1px solid rgb(164, 164, 164)}\
                         QLabel{ font:bold 17px '宋体'; text-align:center; font-weight:800;} \
                         QLineEdit { font: none 13px '宋体'; text-align:left;border:none;background-color:rgb(243, 243, 243) }");


	avatar = new QPushButton();
	avatar->setIconSize(QSize(60,60));
	avatar->setFixedSize(QSize(60, 60));
	avatar->setStyleSheet("QPushButton{border-radius:5px;background-color:transparent;border:none;}");
	layout->addWidget(avatar, 0, 0, 3, 2);

	id_tab = new QLabel("序号");
	id_label = new QLineEdit();
	layout->addWidget(id_tab, 0, 2,1,2);
	layout->addWidget(id_label, 0,4,1,2);

	usrName_tab = new QLabel("昵称");
	usrName_label = new QLineEdit();
	layout->addWidget(usrName_tab, 1, 2,1,2);
	layout->addWidget(usrName_label, 1, 4,1,2);

	phone_tab = new QLabel("电话");
	phone_label = new QLineEdit();
	layout->addWidget(phone_tab, 2, 2,1,2);
	layout->addWidget(phone_label, 2, 4,1,2);

	applyFriend_button = new QPushButton("申请好友");
	applyFriend_button->setFixedSize(QSize(60, 30));
	layout->addWidget(applyFriend_button, 3, 0,1,2);

	sendSession_button = new QPushButton("发送消息");
	sendSession_button->setFixedSize(QSize(60, 30));
	layout->addWidget(sendSession_button, 3, 2,1,2);

	deleteFriend_button = new QPushButton("删除好友");
	deleteFriend_button->setFixedSize(QSize(60, 30));
	layout->addWidget(deleteFriend_button, 3, 4,1,2);


	//debug
	avatar->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
	id_label->setText("123");
	usrName_label->setText("haoyouxiaoju");
	phone_label->setText("10000000000");

	id_label->setReadOnly(true);
	usrName_label->setReadOnly(true);
	phone_label->setReadOnly(true);
	//debug-end


}

OtherUsrInfoWidget::~OtherUsrInfoWidget()
{}
