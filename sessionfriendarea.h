#ifndef SESSIONFRIENDAREA_H
#define SESSIONFRIENDAREA_H

#include <QWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QStyleOption>
#include <QPainter>

#include "model/data.h"

class SessionFriendItem;

enum ItemType{
    SESSIONITEM,
    FRIENDITEM,
    FRIENDAPPLYITEM,
    SEARCHADDFRIENDITEM,
    SESSIONFRIENDITEM,

};


/***** *****	会话好友申请的滑动widget		***** *****/
class SessionFriendArea : public QScrollArea
{
    Q_OBJECT

    friend class SessionFriendItem;
public:
    SessionFriendArea(QWidget* parent=nullptr);
    ~SessionFriendArea();

public:
    //清空所有元素
    void clear();

    //添加元素
    void addItem(ItemType type,const QString& id, const QIcon& avatar , const QString& name , const QString& lastMessage);

    void addItem(ItemType type,const model::UserInfo& info);
    void addItem(ItemType type, const model::ChatSessionInfo& info);

    //点击指定下标元素
    void clickItem(int index);



private:
    QWidget* container;						//存放item的widget
    SessionFriendItem* selected_item;		//存放container中目前哪个item已被选中

};



/***** *****	会话好友申请item	基类 	***** *****/
class SessionFriendItem : public QWidget{

    Q_OBJECT
    friend class SessionFriendArea;
public:
    SessionFriendItem(SessionFriendArea* owner , const QIcon& avatar , const QString& name , const QString& lastMessage);

private:
    void select();
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

    //子类各自点击后需要发生的动作
    virtual void avtion();

    QGridLayout* layout;			//item 的 layout
    QPushButton* __avatar;			//头像
    QLabel* nameLabel;				//昵称
    QLabel* MessageLabel;			//上一条消息

    SessionFriendArea* owner;		//item 所在的 container 的上司

    bool selected;					//是否被选择


};




/***** *****	会话item		***** *****/
class SessionItem : public SessionFriendItem{
    Q_OBJECT
public:
    SessionItem(SessionFriendArea* owner ,const QString& chatSessionId, const QIcon& avatar , const QString& name , const QString& lastMessage);


protected:
    void avtion() override;

private:
    void updateLastMessage(const QString& chatSessionId);

private:
    QString chatSessionId;

};




/***** *****	好友item		***** *****/
class FriendItem : public SessionFriendItem{
    Q_OBJECT
public:
    FriendItem(SessionFriendArea* owner ,const QString& userId, const QIcon& avatar , const QString& name , const QString& description);

protected:
    void avtion() override;

private:
    QString userId;

};




/***** *****	好友申请item		***** *****/
class FriendApplyItem : public SessionFriendItem{
    Q_OBJECT
public:
    FriendApplyItem(SessionFriendArea* owner ,const QString& userId, const QIcon& avatar , const QString& name );

private:
    //接受好友申请
    void acceptFriend();
	void acceptFriendDone(const QString& userId,const QString& reason);
	//拒绝好友申请
	void rejectFriend();
	void rejectFriendDone(const QString& userId, const QString& reason);


protected:
    void avtion() override;

private:
    QString userId;

};

class SearchAddFriendItem : public SessionFriendItem {
    Q_OBJECT
public:
	SearchAddFriendItem(SessionFriendArea* owner,const model::UserInfo& info);

protected:
    void avtion() override;

private:
	model::UserInfo info;


};
#endif // SESSIONFRIENDAREA_H
