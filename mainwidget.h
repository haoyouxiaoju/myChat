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
#include "groupsessiondetailwidget.h"
#include "framelesswidget.h"


class MainWidget : public FramelessWidget
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

public:
    //
    // 
    //tab按钮切换
    void switchTabSession();
    void switchToSession(const QString& user_id);//用于点击好友跳转到会话
    void switchTabFriend();
    void switchTabApply();
    void changeTabIconAction(MainWidget::ActiveTab& activeTab,MainWidget::ActiveTab tab);

    //tab按钮切换后 mainWidgetMid切换设置
    void loadSessionList();
    void loadFriendList();
    void loadApplyList();

    //
    // 点击会话列表item 触发加载会话消息 ---- 好友列表也使用此函数
    void loadRecentMessages(const QString& chat_session_id);

    void updataFriendList();
    void updataChatSessionList();
    void updataFriendApplyList();
    void updateRecentMessages(const QString& chat_session_d);
protected:
    void mousePressEvent(QMouseEvent* event) override;
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
    QWidget* topWidget;
	QLabel* sessionName;				//显示会话名称
    QPushButton* minimize_button;       //控制mainWidget最小化
    QPushButton* maximize_button;       //控制mainWidget最大化
    bool maximize_button_status;
    QPushButton* closeMainWidget_button;//控制mainWidget关闭
    QPushButton* more_Button;           //更多按钮
    SessionDetailsWidget* sessionDetail_widget;//弹窗 
    MessageShowArea* messageShowArea;   //会话消息展示
    MessageEditArea* messageEditArea;   //消息输入框
    //bool isOpen_moreButton;             //是否弹出弹窗
};
#endif // MAINWIDGET_H
