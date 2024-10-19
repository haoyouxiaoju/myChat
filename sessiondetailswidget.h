#pragma once

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QCursor>
#include <QFont>
#include <QTimer>
#include <QVariantAnimation>
#include "model/data.h"
#include "choosefrienddialog.h"

class AvatarItem;

class SessionDetailsWidget : public QDialog
{
	Q_OBJECT
public:
	SessionDetailsWidget(const model::ChatSessionInfo& sessionInfo,QWidget *parent = nullptr);
	~SessionDetailsWidget();

public:
	//static SessionDetailsWidget* getWidget(const model::ChatSessionInfo& sessionInfo,QWidget *parent);
	void addAvatarItem(const QIcon& icon, const QString& name);
	void addAvatarItem(AvatarItem* item);
	void show_widget(); 
	void hide_widget();

protected:
	void paintEvent(QPaintEvent* event) override;
	void closeEvent(QCloseEvent* event) override;

private:
	QVBoxLayout* layout;				//主layout
	QGridLayout* avatarTabel_Layout;	//存放avataritem的layout
	
	model::ChatSessionInfo sessionInfo;
	int cur_Row = 0;
	int cur_Col = 0;
	const int Max_Col = 4;
	

};
class AvatarItem :public QWidget{
	Q_OBJECT

public:
	AvatarItem(const QIcon& icon ,const QString& name ,QWidget *parent = nullptr);
	~AvatarItem();



public:
	QPushButton* avatar_button;
	QLabel* name_label;
};
