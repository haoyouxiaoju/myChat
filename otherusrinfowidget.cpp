#include "otherusrinfowidget.h"
#include <QMap>
#include <QMessageBox>
#include "model/datacenter.h"
#include "mainwidget.h"

OtherUsrInfoWidget::OtherUsrInfoWidget(const model::UserInfo& usrInfo,QWidget *parent)
	: QDialog(parent),info(usrInfo)
{
	layout = new QGridLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(15, 15, 15, 15);
	layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	this->setLayout(layout);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
	// 设置Qt::WA_DeleteOnClose 的话OtherUsrInfoWidget只能通过new的方式创建，在栈上创建会发生释放块错误。
	//this->setAttribute(Qt::WA_DeleteOnClose);
    this->setMinimumSize(200, 100);
	//this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setStyleSheet("QPushButton{ border-radius:5px;background-color:transparent;border:1px solid rgb(164, 164, 164)}\
                         QLabel{ font:bold 17px '宋体'; text-align:center; font-weight:800;} \
                         QLineEdit { font: none 13px '宋体'; text-align:left;border:none;background-color:rgb(240, 240, 240) }");

	model::DataCenter* dataCenter = model::DataCenter::getInstance();
	

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
	layout->addWidget(applyFriend_button, 3, 4,1,2);

	sendSession_button = new QPushButton("发送消息");
	sendSession_button->setFixedSize(QSize(60, 30));
	layout->addWidget(sendSession_button, 3, 2,1,2);

	deleteFriend_button = new QPushButton("删除好友");
	deleteFriend_button->setFixedSize(QSize(60, 30));
	layout->addWidget(deleteFriend_button, 3, 4,1,2);

	model::UserInfo* info =  dataCenter->getFriendById(usrInfo.userId);
	if (info == nullptr) {
		deleteFriend_button->hide();
	}
	else {
		applyFriend_button->hide();
	}

	// init Text
	avatar->setIcon( dataCenter->getIcon(usrInfo.avatar));
	id_label->setText(usrInfo.userId);
	usrName_label->setText(usrInfo.nickname);
	phone_label->setText(usrInfo.phone);

	id_label->setReadOnly(true);
	usrName_label->setReadOnly(true);
	phone_label->setReadOnly(true);
	//init_end

	connect(sendSession_button, &QPushButton::clicked, this, &OtherUsrInfoWidget::clickSendMessageBtn);
	connect(deleteFriend_button, &QPushButton::clicked, this, &OtherUsrInfoWidget::clickDeleteFriendBtn);
	connect(applyFriend_button, &QPushButton::clicked, this, &OtherUsrInfoWidget::clickAddFriendBtn);

}

OtherUsrInfoWidget::~OtherUsrInfoWidget()
{
	LOG() << "close";
}

void OtherUsrInfoWidget::clickSendMessageBtn()
{
	MainWidget* mainWidget = MainWidget::getInstance();
	//mainWidget->switchTabSession();
	LOG() << "跳转到" << info.userId;
	mainWidget->switchToSession(info.userId);

	this->close();
	
}

void OtherUsrInfoWidget::clickDeleteFriendBtn()
{

	MainWidget* mainWidget = MainWidget::getInstance();
	auto result = QMessageBox::warning(mainWidget, "确认删除", "确认删除该好友?", QMessageBox::Ok | QMessageBox::Cancel);
	if (result != QMessageBox::Ok) {
		LOG() << "取消删除好友";
		return;
	}
	model::DataCenter* dataCenter = model::DataCenter::getInstance();
	dataCenter->deleteFriendAsync(info.userId);

	this->close();
}

void OtherUsrInfoWidget::clickAddFriendBtn()
{
	MainWidget* mainWidget = MainWidget::getInstance();
	auto result = QMessageBox::warning(mainWidget, "确认添加", "确认添加该好友?", QMessageBox::Ok | QMessageBox::Cancel);
	if (result != QMessageBox::Ok) {
		LOG() << "取消添加好友";
		return;
	}
	model::DataCenter* dataCenter = model::DataCenter::getInstance();
	dataCenter->addFriendApplyAsync(info.userId);
	this->close();
}

