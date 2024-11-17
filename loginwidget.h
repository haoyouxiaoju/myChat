#pragma once

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushbutton>
#include <QGridLayout>
#include "framelesswidget.h"

class LoginWidget : public	FramelessWidget 
{
	Q_OBJECT

public:
	LoginWidget(QWidget *parent=nullptr);
	~LoginWidget();

public:
	void setMode(bool status);

protected:
	void mousePressEvent(QMouseEvent* event) override;

private:
	//	切换 登录注册模式
	void switch_mode();
	//	切换 手机号登录注册
	void switch_phone_mode();
	

	void __slot_connect();
private:
	QLabel* title;						//标题
	QLineEdit* usr_edit;				//用户名输入
	QLineEdit* password_edit;			//密码输入
	QLineEdit* auth_edit;				//验证码输入
	QPushButton* auth_button;			//获取验证码

	QPushButton* OK_button;				//确认按钮->登录 or 注册
	QPushButton* switch_login_register;	//切换注册 登录
	QPushButton* switch_phone;			//切换到手机号登录
	
	bool tag_switch;					//true->登录 false->注册

	QGridLayout* layout;

};


class LoginPhoneWidget : public	FramelessWidget 
{
	Q_OBJECT

public:
	LoginPhoneWidget(QWidget *parent=nullptr);
	~LoginPhoneWidget();

public:
	void setMode(bool status);
protected:
	void mousePressEvent(QMouseEvent* event) override;

private:
	//	切换 登录注册模式
	void switch_mode();
	//	切换 手机号登录注册
	void switch_usr_mode();
	

	void __slot_connect();
private:
	QLabel* title;						//标题
	QLineEdit* phone_edit;				//用户名输入
	QLineEdit* auth_edit;				//验证码输入
	QPushButton* auth_button;			//获取验证码

	QPushButton* OK_button;				//确认按钮->登录 or 注册
	QPushButton* switch_login_register;	//切换注册 登录
	QPushButton* switch_usr;			//切换到用户登录
	
	bool tag_switch;					//true->登录 false->注册

	QGridLayout* layout;

};

