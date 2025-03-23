#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QDialog>
#include "model/data.h"
#include "framelesswidget.h"
#include "sessionfriendarea.h"

class searchAddFriend : public QDialog
{
	Q_OBJECT


public:
	searchAddFriend(QList<model::UserInfo>* user_list, QWidget *parent = nullptr);
	~searchAddFriend();

private:
	QLabel* searchLabel;
	QVBoxLayout* layout;
	SessionFriendArea* friendArea;

};
