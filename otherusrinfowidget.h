#pragma once

#include <QWidget>
#include "model/data.h"
#include <QLineEdit>
#include <QLabel>
#include <QDialog>
#include <QPushButton>
#include <QGridLayout>
#include <QApplication>
#include <QEvent>

// 未设置Qt::WA_DeleteOnClose 只能在栈上创建，不能通过new的方式创建，否则会内存泄漏。
class OtherUsrInfoWidget : public QDialog
{
	Q_OBJECT

public:
	OtherUsrInfoWidget(const model::UserInfo& usrInfo,QWidget *parent = nullptr);
	~OtherUsrInfoWidget();

protected:

private: 
	void clickSendMessageBtn();
	void clickDeleteFriendBtn();
	void clickAddFriendBtn();

private:
	QPushButton* avatar;
	QLabel* id_tab;
	QLineEdit* id_label;
	QLabel* usrName_tab;
	QLineEdit* usrName_label;
	QLabel* phone_tab;
	QLineEdit* phone_label;

	QPushButton* applyFriend_button;
	QPushButton* sendSession_button;
	QPushButton* deleteFriend_button;

	QGridLayout* layout;

	model::UserInfo info;

};
