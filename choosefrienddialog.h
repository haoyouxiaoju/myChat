#pragma once

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollarea>
#include <QScrollBar>
#include <QCheckBox>
#include <QFont>


class ChooseFriendItem;

class ChooseFriendDialog : public QDialog
{
	Q_OBJECT

public:
	ChooseFriendDialog(const QString& usrId,QWidget *parent = nullptr);
	~ChooseFriendDialog();

	void addFriend(const QString& userId, const QIcon& avatar, const QString& name, bool checked);	//总好友列表添加item
	void addSelectedFriend(const QString& userId, const QIcon& avatar, const QString& name);		//右边选中列表添加item
	void deleteSelectedFriend(const QString& userId);												//删除选中列表item

	QList<QString> generateMemberList();	//生成会话成员列表

public slots:
	void OK_Slot();
	void Cannot_Slot();


private:
	void initLeftWidget();		//初始化左边界面
	void initRightWidget();		//初始化右边界面

private:
	QHBoxLayout* layout;		//主layout ， 存放左右俩界面
	QString usrId;				//弹出会话的好友id
	QWidget* left_widget_scroll;	//总好友列表
	QWidget* right_widget_scroll;	//选中的列表

	

};

class ChooseFriendItem :public QWidget{
	Q_OBJECT
public:
	ChooseFriendItem(ChooseFriendDialog* owner,const QString& userId, const QIcon& avatar, const QString& name,bool checked);
	~ChooseFriendItem();

protected:
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;	//选中checkBox
private:
	void toggleCheckBox(bool checked);						//checkBox选中的槽函数
public:
	QCheckBox* checkBox;
	ChooseFriendDialog* owner;		//父元素
	QString userId;
	QIcon avatar;
	QString name;
	bool isHover;					//是否悬浮于item上

};
