#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTextEdit>
#include <QFont>
#include <QPropertyAnimation>
#include "sessionfriendarea.h"
#include "messageshowarea.h"
#include "messageeditarea.h"
#include "usrinfowidget.h"
#include "otherusrinfowidget.h"
#include "sessiondetailswidget.h"


class MainWidget : public QWidget
{
    Q_OBJECT
private:
    MainWidget(QWidget *parent = nullptr);

public:
    static MainWidget* getInstance();
    ~MainWidget();
    enum ActiveTab{
        SESSION_LIST,
        FRIEND_LIST,
        APPLY_LIST,
        UN_INIT,
    };

private:
    //界面初始化
    void initUi();
    void initMainWidget();
    void initMainWidgetLeft();
    void initMainWidgetMid();
    void initMainWidgetRight();

    // 主界面信号绑定
    void initMainWidgetSignal();

    //tab按钮切换
    void switchTabSession();
    void switchTabFriend();
    void switchTabApply();
    void changeTabIconAction(MainWidget::ActiveTab& activeTab,MainWidget::ActiveTab tab);

    //tab按钮切换后 mainWidgetMid切换设置
    void loadSessionList();
    void loadFriendList();
    void loadApplyList();
protected:

private:
    QWidget* mainWidgetLeft;
    QWidget* mainWidgetMid;
    QWidget* mainWidgetRight;

    ActiveTab activeTab = UN_INIT;
    
    //mainWidgetLeft _
    QPushButton* userAvatar;			//用户头像
    QPushButton* sessionTabBtn;			//会话
    QPushButton* friendTabBtn;			//好友
    QPushButton* applyTabBtn;			//好友申请

    //mainWidgetMid
    QTextEdit* search_input;			//搜索框
    QPushButton* search_submit;			//搜索按钮
    SessionFriendArea* session_friendArea;

	//mainWidgetRight
	QLabel* sessionName;				//显示会话名称
    QPushButton* more_Button;           //更多按钮
    SessionDetailsWidget* sessionDetail_widget;//弹窗
    //bool isOpen_moreButton;             //是否弹出弹窗
};
#endif // MAINWIDGET_H
