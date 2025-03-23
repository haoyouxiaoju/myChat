#include "loginwidget.h"
#include "toast.h"
#include "model/datacenter.h"
#include "mainwidget.h"

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

	auth_button = new VerifyCodeWidget();
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

void LoginWidget::userLoginDone(bool ok, const QString& reason)
{
	if (ok) {
		//登录成功
		Toast::showMessage("登录成功");
	}
	else {
		//登录失败
		Toast::showMessage("登录失败");
		return;
	}
	MainWidget* w = MainWidget::getInstance();
	w->show();
	this->deleteLater();
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
		LoginPhoneWidget* w = new LoginPhoneWidget();
		w->setMode(this->tag_switch);
		this->deleteLater();
		w->show();
	}
}

void LoginWidget::switch_phone_mode()
{
	LoginPhoneWidget* w = new LoginPhoneWidget();
	w->setMode(this->tag_switch);
	this->deleteLater();
	w->show();
}

void LoginWidget::__slot_connect()
{
	//登录/注册
	connect(OK_button, &QPushButton::clicked, this, [this] {
		if (tag_switch) {
			//登录
			const QString& username = usr_edit->text();
			const QString& password = password_edit->text();
			const QString& authcode = auth_edit->text();
			if (username.isEmpty() || password.isEmpty() || authcode.isEmpty()) {
				//提示输入不完整
				Toast::showMessage("输入不完整");
				return;
			}
			if (authcode.toLower() != auth_button->getVerifyCode().toLower()) {
				//验证码错误
				Toast::showMessage("验证码错误");
				return;
			}
			//发起登录请求
			model::DataCenter* dataCenter = model::DataCenter::getInstance();
			connect(dataCenter, &model::DataCenter::userLoginDone, this,&LoginWidget::userLoginDone,Qt::UniqueConnection);
			dataCenter->loginAsync(username, password);
		}
		else {
			//取消掉用户名注册
			return;
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

	auth_button = new QPushButton("获取验证码");
	auth_button->setFont(QFont("宋体", 15));
	auth_button->setFixedSize(100, 45);

	layout->addWidget(auth_button, 4,7, 1, 3);

	OK_button = new QPushButton("登录");
	OK_button->setFixedSize(320, 45);
	OK_button->setObjectName("greenButton");
	OK_button->setStyleSheet("QPushButton#greenButton{border:none;background-color:rgb(44, 182, 61); \
				color:rgb(255,255,255);font-size:17px;}");
	layout->addWidget(OK_button, 5, 0, 1, 10);

	switch_usr = new QPushButton("用户名登录");
	switch_usr->setFont(QFont("宋体", 13));
	switch_usr->setFixedHeight(45);
	layout->addWidget(switch_usr, 6, 0, 1, 3);

	switch_login_register = new QPushButton("注册");
	switch_login_register->setFont(QFont("宋体", 13));
	switch_login_register->setFixedHeight(45);
	layout->addWidget(switch_login_register, 6, 8, 1, 2);

	timer = new QTimer(this);


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

void LoginPhoneWidget::sendVerifyCodeDone()
{
	Toast::showMessage("验证码已发送");
}

void LoginPhoneWidget::switch_mode()
{
	if (tag_switch) {
		title->setText("登录");
		OK_button->setText("登录");
		switch_login_register->setText("注册");
		password_edit->hide();
	}
	else {
		title->setText("注册");
		OK_button->setText("注册");
		switch_login_register->setText("登录");
		password_edit->show();
	}
}

void LoginPhoneWidget::switch_usr_mode()
{
	LoginWidget* w = new LoginWidget();
	w->setMode(true);
	this->deleteLater();
	w->show();
}

void LoginPhoneWidget::phoneLoginDone(bool ok, const QString& reason)
{
	if (!ok) {
		Toast::showMessage("登录失败");
		return;
	}
	Toast::showMessage("登录成功");
	MainWidget* w = MainWidget::getInstance();
	w->show();
	this->deleteLater();
}

void LoginPhoneWidget::phoneRegisterDone(bool ok, const QString& reason)
{
	if (!ok) {
		Toast::showMessage("注册失败");
		return;
	}
	Toast::showMessage("注册成功");
	switch_usr_mode();
	phone_edit->clear();
	auth_edit->clear();
	auth_button->setText("获取验证码");
	auth_button->setEnabled(true);
}

void LoginPhoneWidget::__slot_connect()
{
	//登录/注册
	connect(OK_button, &QPushButton::clicked, this, [this] {
		const QString& phone = phone_edit->text();
		const QString& authcode = auth_edit->text();
		if (phone.isEmpty() || authcode.isEmpty() || phone.size() != 11) {
			Toast::showMessage("输入不完整");
			return;
		}
		model::DataCenter* dataCenter = model::DataCenter::getInstance();
		if (tag_switch) {
			//登录
			connect(dataCenter, &model::DataCenter::phoneLoginDone, this, &LoginPhoneWidget::phoneLoginDone, Qt::UniqueConnection);
			dataCenter->phoneLoginAsync(phone, authcode);
		}
		else {
			//注册
			const QString& password = password_edit->text();
			connect(dataCenter, &model::DataCenter::phoneRegisterDone, this, &LoginPhoneWidget::phoneRegisterDone, Qt::UniqueConnection);
			dataCenter->phoneRegisterAsync(phone, password,authcode);
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

	//验证码定时
	connect(timer, &QTimer::timeout, this, [this]() {
		if (time < 1) {
			auth_button->setText("发送验证码");
			auth_button->setEnabled(true);
			timer->stop();
			timer->deleteLater();
			return;
		}
		--(this->time);
		auth_button->setText(QString::number(time) + "s");
		});

	//获取验证码
	connect(auth_button, &QPushButton::clicked, this, [this]() {
		//获取验证码
		const QString& phone = phone_edit->text();
		if (phone.isEmpty() || phone.size() != 11) {
			Toast::showMessage("请输入正确的手机号");
			return;
		}
		auth_button->setEnabled(false);
		timer->start(1000);
		time = 30;
		model::DataCenter* dataCenter = model::DataCenter::getInstance();
		connect(dataCenter, &model::DataCenter::getVerifyCodeDone, this, &LoginPhoneWidget::sendVerifyCodeDone, Qt::UniqueConnection);
		dataCenter->getVerifyCodeAsync(phone);
		});


	//关闭按钮是局部变量所以在构造函数就绑定

}
