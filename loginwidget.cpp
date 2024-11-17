#include "loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent)
	:FramelessWidget(parent),tag_switch(true)
{
	this->setFixedSize(400, 420);
	
	layout = new QGridLayout();
	layout->setContentsMargins(40, 20, 40, 20);
	layout->setVerticalSpacing(10);
	layout->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	this->setLayout(layout);

	this->setStyleSheet("QLineEdit{border:none;background-color:rgb(235, 235, 235);  \
					border-radius:4px;font-family:'宋体';font-size:17px;text-}		\
					QPushButton{border:none}");
	
	QPushButton* close_button = new QPushButton(this);
	close_button->setIcon(QIcon(":/resource/images/close.png"));
	close_button->setFixedSize(40, 30);
	close_button->move(360, 0);
	connect(close_button, &QPushButton::clicked, this, [this] {
		this->close();
		});


	title = new QLabel("登录");
	title->setFont(QFont("宋体", 27, 900));
	layout->addWidget(title, 1, 0, 1, 10);

	usr_edit = new QLineEdit();
	usr_edit->setPlaceholderText("输入用户名");
	usr_edit->setTextMargins(10, 0, 0, 0);
	usr_edit->setFixedSize(320, 45);

	layout->addWidget(usr_edit, 2, 0, 1, 10);

	password_edit = new QLineEdit();
	password_edit->setPlaceholderText("输入密码");
	password_edit->setTextMargins(10, 0, 0, 0);
	password_edit->setFixedSize(320, 45);

	layout->addWidget(password_edit, 3, 0, 1, 10);

	auth_edit = new QLineEdit();
	auth_edit->setPlaceholderText("输入验证码");
	auth_edit->setTextMargins(10, 0, 0, 0);
	auth_edit->setFixedSize(220, 45);

	layout->addWidget(auth_edit, 4, 0, 1, 7);

	auth_button = new QPushButton("验证码");
	auth_button->setFont(QFont("宋体", 15));
	auth_button->setFixedSize(100, 45);

	layout->addWidget(auth_button, 4, 7, 1, 3);

	OK_button = new QPushButton("登录");
	OK_button->setFixedSize(320, 45);
	OK_button->setObjectName("greenButton");
	OK_button->setStyleSheet("QPushButton#greenButton{border:none;background-color:rgb(44, 182, 61); \
				color:rgb(255,255,255);font-size:17px;}");
	layout->addWidget(OK_button, 5, 0, 1, 10);

	switch_phone = new QPushButton("手机号登录");
	switch_phone->setFont(QFont("宋体", 13));
	switch_phone->setFixedHeight(45);
	layout->addWidget(switch_phone, 6, 0, 1, 3);

	switch_login_register = new QPushButton("注册");
	switch_login_register->setFont(QFont("宋体", 13));
	switch_login_register->setFixedHeight(45);
	layout->addWidget(switch_login_register, 6, 8, 1, 2);


	//绑定 槽函数
	__slot_connect();
}

LoginWidget::~LoginWidget()
{
}

void LoginWidget::setMode(bool status)
{
	tag_switch = status;
	switch_mode();
}

void LoginWidget::mousePressEvent(QMouseEvent* event)
{
	this->setMoveStatus(true);
	FramelessWidget::mousePressEvent(event);
}

void LoginWidget::switch_mode()
{
	if (tag_switch) {
		title->setText("登录");
		OK_button->setText("登录");
		switch_phone->setText("手机号登录");
		switch_login_register->setText("注册");
	}
	else {
		title->setText("注册");
		OK_button->setText("注册");
		switch_phone->setText("手机号注册");
		switch_login_register->setText("登录");
	}
}

void LoginWidget::switch_phone_mode()
{
	LoginPhoneWidget* w = new LoginPhoneWidget();
	w->setMode(this->tag_switch);
	this->close();
	w->show();
}

void LoginWidget::__slot_connect()
{
	//登录/注册
	connect(OK_button, &QPushButton::clicked, this, [this] {
		if (tag_switch) {
			
		}
		else {

		}

		});

	// 切换登录或注册模式
	connect(switch_login_register, &QPushButton::clicked, this, [this]() {
		this->tag_switch = !tag_switch;
		switch_mode();

		});

	//切换到手机号
	connect(switch_phone, &QPushButton::clicked, this, [this]() {
		
		switch_phone_mode();

		});

	//关闭按钮是局部变量所以在构造函数就绑定

}

LoginPhoneWidget::LoginPhoneWidget(QWidget* parent)
	:FramelessWidget(parent),tag_switch(true)
{
	this->setFixedSize(400, 420);
	
	layout = new QGridLayout();
	layout->setContentsMargins(40, 20, 40, 20);
	layout->setVerticalSpacing(10);
	layout->setAlignment(Qt::AlignCenter | Qt::AlignHCenter);
	this->setLayout(layout);

	this->setStyleSheet("QLineEdit{border:none;background-color:rgb(235, 235, 235);  \
					border-radius:4px;font-family:'宋体';font-size:17px;text-}		\
					QPushButton{border:none}");
	
	QPushButton* close_button = new QPushButton(this);
	close_button->setIcon(QIcon(":/resource/images/close.png"));
	close_button->setFixedSize(40, 30);
	close_button->move(360, 0);
	connect(close_button, &QPushButton::clicked, this, [this] {
		this->close();
		});


	title = new QLabel("登录");
	title->setFont(QFont("宋体", 27, 900));
	layout->addWidget(title, 1, 0, 1, 10);

	phone_edit = new QLineEdit();
	phone_edit->setPlaceholderText("输入电话号码");
	phone_edit->setTextMargins(10, 0, 0, 0);
	phone_edit->setFixedSize(320, 45);

	layout->addWidget(phone_edit, 2, 0, 1, 10);


	auth_edit = new QLineEdit();
	auth_edit->setPlaceholderText("输入验证码");
	auth_edit->setTextMargins(10, 0, 0, 0);
	auth_edit->setFixedSize(220, 45);

	layout->addWidget(auth_edit, 3, 0, 1, 7);

	auth_button = new QPushButton("验证码");
	auth_button->setFont(QFont("宋体", 15));
	auth_button->setFixedSize(100, 45);

	layout->addWidget(auth_button, 3,7, 1, 3);

	OK_button = new QPushButton("登录");
	OK_button->setFixedSize(320, 45);
	OK_button->setObjectName("greenButton");
	OK_button->setStyleSheet("QPushButton#greenButton{border:none;background-color:rgb(44, 182, 61); \
				color:rgb(255,255,255);font-size:17px;}");
	layout->addWidget(OK_button, 4, 0, 1, 10);

	switch_usr = new QPushButton("用户名登录");
	switch_usr->setFont(QFont("宋体", 13));
	switch_usr->setFixedHeight(45);
	layout->addWidget(switch_usr, 5, 0, 1, 3);

	switch_login_register = new QPushButton("注册");
	switch_login_register->setFont(QFont("宋体", 13));
	switch_login_register->setFixedHeight(45);
	layout->addWidget(switch_login_register, 5, 8, 1, 2);


	switch_mode();

	//绑定 槽函数
	__slot_connect();
}

LoginPhoneWidget::~LoginPhoneWidget()
{
}

void LoginPhoneWidget::setMode(bool status)
{
	tag_switch = status;
	switch_mode();
}

void LoginPhoneWidget::mousePressEvent(QMouseEvent* event)
{
	this->setMoveStatus(true);
	FramelessWidget::mousePressEvent(event);
}

void LoginPhoneWidget::switch_mode()
{
	if (tag_switch) {
		title->setText("登录");
		OK_button->setText("登录");
		switch_login_register->setText("注册");
	}
	else {
		title->setText("注册");
		OK_button->setText("注册");
		switch_login_register->setText("登录");
	}
}

void LoginPhoneWidget::switch_usr_mode()
{
	LoginWidget* w = new LoginWidget();
	this->close();
	w->show();
}

void LoginPhoneWidget::__slot_connect()
{
	//登录/注册
	connect(OK_button, &QPushButton::clicked, this, [this] {
		if (tag_switch) {
			
		}
		else {

		}

		});

	// 切换登录或注册模式
	connect(switch_login_register, &QPushButton::clicked, this, [this]() {
		this->tag_switch = !tag_switch;
		switch_mode();

		});

	//切换到手机号
	connect(switch_usr, &QPushButton::clicked, this, [this]() {
		switch_usr_mode();
		});

	//关闭按钮是局部变量所以在构造函数就绑定

}
