#pragma once

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QDialog>


class OtherUsrInfoWidget : public QDialog
{
	Q_OBJECT

public:
	OtherUsrInfoWidget(QWidget *parent = nullptr);
	~OtherUsrInfoWidget();

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


};
