#pragma once

#include <QDialog>
#include <QWidget>

#include "sessiondetailswidget.h"

class GroupSessionDetailWidget  : public QDialog
{
	Q_OBJECT

public:
	GroupSessionDetailWidget(const model::ChatSessionInfo& sessionInfo, QWidget *parent);
	~GroupSessionDetailWidget();

	void addAvatarItem(const QIcon& icon, const QString& name);
	void addAvatarItem(AvatarItem* item);
	void show_widget(); 
private:
	QVBoxLayout* layout;				//主layout
	QGridLayout* avatarTabel_Layout;	//存放avataritem的layout
	
	model::ChatSessionInfo sessionInfo;
	int cur_Row = 0;
	int cur_Col = 0;
	const int Max_Col = 4;

};
