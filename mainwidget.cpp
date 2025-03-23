#include "mainwidget.h"
#include "model/datacenter.h"
#include <QList>
#include "searchaddfriend.h"

#include "toast.h"


MainWidget::MainWidget(QWidget *parent)
    : FramelessWidget(parent)
{
    initUi();
}

// 使用静态方法获取单例，
MainWidget *MainWidget::getInstance()
{
    static MainWidget mainWidget;
    return &mainWidget;
}

MainWidget::~MainWidget() {}

//	用于初始化界面
void MainWidget::initUi()
{
    //this->setWindowTitle("我的聊天室");
    //this->setWindowIcon(QIcon(":/resource/images/xiaoju.jpg"));
    //去除标题栏   
    //this->setWindowFlags(Qt::FramelessWindowHint );




    initMainWidget();

    initMainWidgetLeft();	//头像，会话，好友，好友申请按钮
    initMainWidgetMid();	//会话、好友、好友申请列表
    initMainWidgetRight();	//会话信息

    initMainWidgetSignal();//绑定信号槽

    //使得开始页面是会话列表
    switchTabSession();

}

// 初始化主页面
void MainWidget::initMainWidget()
{
    QHBoxLayout*  mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);
    this->setMinimumHeight(600);
    this->setMinimumWidth(1000);

    mainWidgetLeft = new QWidget();
    mainWidgetMid = new QWidget();
    mainWidgetRight = new QWidget();

    mainWidgetLeft->setFixedWidth(60);
    mainWidgetMid->setFixedWidth(200);
    mainWidgetRight->setMinimumWidth(700);
    mainWidgetRight->setWindowFlags(Qt::WindowStaysOnTopHint);

    mainWidgetLeft->setStyleSheet("QWidget{background-color:rgb(45,45,45);} QPushButton{border-radius:5px;border:none;background-color:transparent;}");
    mainWidgetMid->setStyleSheet("background-color:rgb(247,247,247);");
    mainWidgetRight->setStyleSheet("background-color:rgb(245,245,245);");

    mainLayout->addWidget(mainWidgetLeft);
    mainLayout->addWidget(mainWidgetMid);
    mainLayout->addWidget(mainWidgetRight);
    



}

// 初始化主界面的左界面
void MainWidget::initMainWidgetLeft()
{
    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    vBoxLayout->setSpacing(10);
    vBoxLayout->setContentsMargins(0,30,0,0);
    mainWidgetLeft->setLayout(vBoxLayout);

    //用户头像初始化
    userAvatar = new QPushButton();
    userAvatar->setFixedSize(45,45);
    userAvatar->setIconSize(QSize(45,45));
    //userAvatar->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
    vBoxLayout->addWidget(userAvatar,1,Qt::AlignTop | Qt::AlignCenter);

    //会话列表按钮初始化
    sessionTabBtn = new QPushButton();
    sessionTabBtn->setFixedSize(45,45);
    sessionTabBtn->setIconSize(QSize(35,35));
    sessionTabBtn->setIcon(QIcon(":/resource/images/duihua_action.png"));
    vBoxLayout->addWidget(sessionTabBtn,1,Qt::AlignTop | Qt::AlignCenter);

    //好友列表按钮初始化
    friendTabBtn = new QPushButton();
    friendTabBtn->setFixedSize(45,45);
    friendTabBtn->setIconSize(QSize(35,35));
    friendTabBtn->setIcon(QIcon(":/resource/images/haoyou.png"));
    vBoxLayout->addWidget(friendTabBtn,1,Qt::AlignTop | Qt::AlignCenter);

    //好友申请列表初始化
    applyTabBtn = new QPushButton();
    applyTabBtn->setFixedSize(45,45);
    applyTabBtn->setIconSize(QSize(35,35));
    applyTabBtn->setIcon(QIcon(":/resource/images/tianjiahaoyou.png"));
    vBoxLayout->addWidget(applyTabBtn,1,Qt::AlignTop | Qt::AlignCenter);
    vBoxLayout->addStretch(20);//空白
}

void MainWidget::initMainWidgetMid()
{
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(0);
    gridLayout->setVerticalSpacing(15);
    gridLayout->setContentsMargins(0,15,0,0);
    gridLayout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    mainWidgetMid->setLayout(gridLayout);

    ////空白占位
    QWidget* widget1= new QWidget();
    widget1->setFixedWidth(10);
    QWidget* widget2= new QWidget();
    widget2->setFixedWidth(10);
    QWidget* widget3= new QWidget();
    widget3->setFixedWidth(10);

    // 搜索按钮和输入框
    search_input = new QTextEdit();
    search_input->setFixedHeight(30);
    search_input->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    search_input->setPlaceholderText("搜索");
    search_input->setStyleSheet("QTextEdit {border-radius:5px; padding-left:5px; background-color:rgb(226,226,226); } ");
    gridLayout->addWidget(widget1,0,0);
    gridLayout->addWidget(search_input,0,1);
    gridLayout->addWidget(widget2,0,2);

    search_submit = new QPushButton();
    search_submit->setFixedSize(25,25);
    search_submit->setIconSize(QSize(25,25));
    search_submit->setIcon(QIcon(":/resource/images/sousuo.png"));
    search_submit->setStyleSheet("QPushButton {border-radius:5px;background-color:rgb(226,226,226);border:none;}QPushButton:pressed{background-color:rgb(240,240,240); }");
    gridLayout->addWidget(search_submit,0,3);
    gridLayout->addWidget(widget3,0,4);

    // 列表
    session_friendArea = new SessionFriendArea();
    gridLayout->addWidget(session_friendArea,1,0,1,5);
}


// 会话界面
void MainWidget::initMainWidgetRight()
{

    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(0,0,0,0);
    vBoxLayout->setSpacing(0);
    mainWidgetRight->setLayout(vBoxLayout);

    //	存放会话名称和更多信息按钮
    topWidget = new QWidget();
    topWidget->setFixedHeight(75);
    topWidget->setStyleSheet("QWidget { border-bottom:1px solid rgb(231,231,231);border-left:1px solid rgb(231,231,231); }");

    //
    QGridLayout* topWidgetLayout = new QGridLayout();
    topWidgetLayout->setSpacing(0);
    topWidgetLayout->setContentsMargins(10,0,10,0);
    topWidget->setLayout(topWidgetLayout);

    //会话名称
    sessionName = new QLabel("测试");
    QFont big_font;
    big_font.setPointSize(16);
    big_font.setWeight(QFont::Bold);
    sessionName->setFont(big_font);
    sessionName->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    sessionName->setStyleSheet("QWidget {border: none }");
    topWidgetLayout->addWidget(sessionName,0,0,2,2);

    //最小化
    minimize_button = new QPushButton();
    minimize_button->setFixedSize(QSize(30,30));
    minimize_button->setIconSize(QSize(15,15));
    minimize_button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    minimize_button->setIcon(QIcon(":/resource/images/minimize.png"));
    minimize_button->setStyleSheet("QPushButton { border: none ; background-color:transparent ;}");
    topWidgetLayout->addWidget(minimize_button,0,13,1,1);
    
    //最大化
    maximize_button_status = false;
    maximize_button = new QPushButton();
    maximize_button->setFixedSize(QSize(30,30));
    maximize_button->setIconSize(QSize(15,15));
    maximize_button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    maximize_button->setIcon(QIcon(":/resource/images/maximize.png"));
    maximize_button->setStyleSheet("QPushButton { border: none ; background-color:transparent ;}");
    topWidgetLayout->addWidget(maximize_button,0,14,1,1);


    //关闭
    closeMainWidget_button = new QPushButton();
    closeMainWidget_button->setFixedSize(QSize(30,30));
    closeMainWidget_button->setIconSize(QSize(15,15));
    closeMainWidget_button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    closeMainWidget_button->setIcon(QIcon(":/resource/images/close.png"));
    closeMainWidget_button->setStyleSheet("QPushButton { border: none ; background-color:transparent ;}");
    topWidgetLayout->addWidget(closeMainWidget_button,0,15,1,1);
    
    
    // 更多按钮
    more_Button = new QPushButton();
    more_Button->setFixedSize(QSize(30,30));
    more_Button->setIconSize(QSize(20,20));
    more_Button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    more_Button->setIcon(QIcon(":/resource/images/more.png"));
    more_Button->setStyleSheet("QPushButton { border: none ; background-color:transparent ;}");
    topWidgetLayout->addWidget(more_Button,1,15,1,1);

    vBoxLayout->addWidget(topWidget,0,Qt::AlignTop);

    //会话展示界面
    messageShowArea  = new MessageShowArea();

    vBoxLayout->addWidget(messageShowArea);

    //消息输入界面 后续会使用到主窗口里获取消息展示区
    messageEditArea = new MessageEditArea(this);

    vBoxLayout->addWidget(messageEditArea,0,Qt::AlignBottom);





}

// 用于绑定信号与槽
void MainWidget::initMainWidgetSignal()
{
    //数据处理中心
    model::DataCenter* dataCenter = model::DataCenter::getInstance();

    //会话列表按钮信号绑定
    connect(sessionTabBtn,&QPushButton::clicked,this,&MainWidget::switchTabSession);
    //好友列表按钮信号绑定
    connect(friendTabBtn,&QPushButton::clicked,this,&MainWidget::switchTabFriend);
    //好友申请列表信号绑定
    connect(applyTabBtn,&QPushButton::clicked,this,&MainWidget::switchTabApply);
    //点击个人头像 显示个人信息
    connect(userAvatar, &QPushButton::clicked, this, [this]() {
		usrInfoWidget userInfo(this);
		QPoint globalPos = QCursor::pos();
		userInfo.move(globalPos.x(), globalPos.y());
		userInfo.exec();

     });


    //最小化
    connect(minimize_button, &QPushButton::clicked, this, [this]() {
            this->minimizeWidget();
    });

    
    //最大化
	connect(maximize_button, &QPushButton::clicked, this, [this]() {
		if (maximize_button_status) {
			this->restoreWidget();
			maximize_button_status = false;

		}
		else {
			this->maximizeWidget();
			maximize_button_status = true;

		}
		});


    //关闭
    connect(closeMainWidget_button, &QPushButton::clicked, this, [this]() {
        this->close();
        });


    //更多
    connect(more_Button, &QPushButton::clicked, this, [this,dataCenter]() {

		//获取当前会话信息
		model::ChatSessionInfo* sessionInfo = dataCenter->findChatSessionBySessionId(dataCenter->getCurrentChatSessionId());
        if (sessionInfo == nullptr) {
			LOG() << "当前会话不存在";
			return;
        }
		//判断是单聊还是群聊
        if (sessionInfo->userId != "") {
			//单聊
			model::UserInfo* userInfo = dataCenter->getFriendById(sessionInfo->userId);
            if (userInfo == nullptr) {
                LOG() << "当前会话对应的用户不存在";
                return;
            }
			SessionDetailsWidget* sessionDetail_widget = new SessionDetailsWidget(*sessionInfo, this);
			sessionDetail_widget->show_widget();
			sessionDetail_widget->move(QPoint(this->x() + this->width(), this->y()));
		}
        else {
			//群聊
			GroupSessionDetailWidget* sessionDetail_widget = new GroupSessionDetailWidget(*sessionInfo, this);
			sessionDetail_widget->show_widget();
			sessionDetail_widget->move(QPoint(this->x() + this->width(), this->y()));
        }


		/*sessionDetail_widget = new SessionDetailsWidget((model::ChatSessionInfo()), this);
		sessionDetail_widget->show_widget();
		sessionDetail_widget->move(QPoint(this->x() + this->width(), this->y()));*/

	/*	GroupSessionDetailWidget* sessionDetail_widget = new GroupSessionDetailWidget( this);
            sessionDetail_widget->show_widget();
			sessionDetail_widget->move(QPoint(this->x() + this->width(), this->y()));*/


     });

    //
    //获取用户信息 --头像
    connect(dataCenter, &model::DataCenter::getMySelfDone, this, [dataCenter,this]() {
        const auto* myself = dataCenter->getMySelf();
        this->userAvatar->setIcon(dataCenter->getIcon(myself->avatar));
        });
    dataCenter->getMySelfAsync();

    //用户修改头像成功触发
    connect(dataCenter, &model::DataCenter::changeAvatarDone, this, [this,dataCenter]() {
        this->userAvatar->setIcon(dataCenter->getIcon(dataCenter->getMySelf()->avatar));
        });

    //删除好友
    connect(dataCenter, &model::DataCenter::deleteFriendDone, this,[this]() {
        this->updataChatSessionList();
        this->updataFriendList();
        LOG() << "删除好友完成";
        });
    //清空当前会话的消息
    connect(dataCenter, &model::DataCenter::clearCurrentSession, this, [this]() {
        sessionName->setText("");
        messageShowArea->clear();
        LOG() << "删除好友为当前会话, 清空完成";
        });

    //发送好友申请
	connect(dataCenter, &model::DataCenter::addFriendApplyDone, this, [this]() {
        Toast::showMessage("好友申请发送成功");
		});

    //处理好友申请
	connect(dataCenter, &model::DataCenter::reveiveFriendApplyDone, this, [this]() {
		Toast::showMessage("收到好友申请");
		updataFriendApplyList();
		});
	//接受到好友申请被同意
	connect(dataCenter, &model::DataCenter::reveiveFriendProcessAccept, this, [this](const model::UserInfo& userInfo) {
		Toast::showMessage("好友申请被同意");
		updataFriendList();
		});
	//接受到好友申请被拒绝
	connect(dataCenter, &model::DataCenter::reveiveFriendProcessReject, this, [this](const model::UserInfo& userInfo) {
		Toast::showMessage("好友申请被拒绝");
		});

	//创建群聊请求发送结束
	connect(dataCenter, &model::DataCenter::createGroupChatSessionDone, this, []() {
		Toast::showMessage("创建群聊请求发送成功");
		});

	//接收到会话
	connect(dataCenter, &model::DataCenter::receiveChatSessionDone, this, [this]() {
		//更新会话列表
		this->updataChatSessionList();
		Toast::showMessage("接收到会话");
        });
    //搜索添加好友
	connect(search_submit, &QPushButton::clicked, this, [this, dataCenter]() {
	/*	QString search_content = search_input->toPlainText();
		if (search_content.isEmpty()) {
			Toast::showMessage("搜索内容不能为空");
			return;
		}
		dataCenter->searchAddFriendAsync(search_content);*/
		//搜索添加好友
		const QString& search_key = search_input->toPlainText();
		if (search_key.isEmpty()) {
			Toast::showMessage("搜索内容不能为空");
			return;
		}
		connect(dataCenter, &model::DataCenter::searchAddFriendDone, this, [this,dataCenter]() {
            QList<model::UserInfo>* user_list = dataCenter->getSearchUserResult();
			if (user_list == nullptr || user_list->size() == 0) {
				Toast::showMessage("搜索添加好友失败");
				return;
			}
            for (auto e : *user_list) {
                LOG() <<"userId:" << e.userId<<",phone:"<<e.phone;
            }
			searchAddFriend* search_widget = new searchAddFriend(user_list, this);
			search_widget->move(search_submit->x() + this->x(), search_submit->y() + this->y() + search_submit->height());
			Toast::showMessage("搜索添加好友成功");
			search_widget->show();
			});
		dataCenter->searchAddFriendAsync(search_key);
		});

    // 
    //
    //dataCenter->getFriendListAsync()

}

//			改变Tab按钮的Icon状态，activTab失去焦点Icon，Tab获取焦点Icon,并将activeTab设置为Tab；
void MainWidget::changeTabIconAction(MainWidget::ActiveTab& activeTab,MainWidget::ActiveTab tab){
    // 失去焦点设置
    LOG() << activeTab;
    switch(activeTab){
    case MainWidget::SESSION_LIST:
        sessionTabBtn->setIcon(QIcon(":/resource/images/duihua.png"));
        break;
    case MainWidget::FRIEND_LIST:
        friendTabBtn->setIcon(QIcon(":/resource/images/haoyou.png"));
        break;
    case MainWidget::APPLY_LIST:
        applyTabBtn->setIcon(QIcon(":/resource/images/tianjiahaoyou.png"));
        break;
    default:
        sessionTabBtn->setIcon(QIcon(":/resource/images/duihua.png"));
        friendTabBtn->setIcon(QIcon(":/resource/images/haoyou.png"));
        applyTabBtn->setIcon(QIcon(":/resource/images/tianjiahaoyou.png"));
    }

    //获取焦点设置
    switch(tab){
    case MainWidget::SESSION_LIST:
        sessionTabBtn->setIcon(QIcon(":/resource/images/duihua_action.png"));
        break;
    case MainWidget::FRIEND_LIST:
        friendTabBtn->setIcon(QIcon(":/resource/images/haoyou_action.png"));
        break;
    case MainWidget::APPLY_LIST:
        applyTabBtn->setIcon(QIcon(":/resource/images/tianjiahaoyou_action.png"));
        break;

    default:
        break;
    }



    activeTab = tab;  //设置mainWidgetMid状态为新状态

}

// 切换为会话列表界面
void MainWidget::switchTabSession()
{
    if(activeTab!=SESSION_LIST){
        changeTabIconAction(activeTab,SESSION_LIST);
        this->loadSessionList();
    }

}

void MainWidget::switchToSession(const QString& user_id)
{
	model::DataCenter* dataCenter = model::DataCenter::getInstance();
	const model::ChatSessionInfo* info = dataCenter->findChatSessionByUserId(user_id);
	if (info == nullptr) {
		qCritical() << POSITION << "⽤⼾对应的会话不存在! userId=" << user_id;
		return;
	}
    LOG() << user_id;
    LOG() << info->userId;

	dataCenter->topChatSessionInfo(*info);

	switchTabSession();
	this->updataChatSessionList();
    this->session_friendArea->clickItem(0);

}

// 切换为好友列表界面
void MainWidget::switchTabFriend()
{
    if(activeTab != FRIEND_LIST){
        changeTabIconAction(activeTab,FRIEND_LIST);
        this->loadFriendList();
    }

}

// 切换为好友申请列表界面
void MainWidget::switchTabApply()
{

    if(activeTab != APPLY_LIST){
        changeTabIconAction(activeTab,APPLY_LIST);
        this->loadApplyList();
    }

}
void MainWidget::loadSessionList()
{
    model::DataCenter* dataCenter = model::DataCenter::getInstance();
    if (dataCenter->getChatSessionList()!=nullptr) {
        updataChatSessionList();
    }
    else {
        connect(dataCenter, &model::DataCenter::getChatSessionListDone, this, &MainWidget::updataChatSessionList, Qt::UniqueConnection);
        dataCenter->getChatSessionListAsync();
    }

}

void MainWidget::loadFriendList()
{

    //数据处理中心
    model::DataCenter* dataCenter = model::DataCenter::getInstance();
    if (dataCenter->getFriendList() != nullptr) {
        updataFriendList();
    }
    else {
		//获取好友列表 --
        connect(dataCenter, &model::DataCenter::getFriendListDone, this,&MainWidget::updataFriendList,Qt::UniqueConnection);
        dataCenter->getFriendListAsync();
	}


}


void MainWidget::loadApplyList()
{
    model::DataCenter* dataCenter = model::DataCenter::getInstance();
    if(dataCenter->getApplyList() != nullptr){
        updataFriendApplyList();
    }
    else {
        //获取好友申请列表
        connect(dataCenter, &model::DataCenter::getApplyListDone, this, &MainWidget::updataFriendApplyList, Qt::UniqueConnection);
        dataCenter->getApplyListAsync();
    }
}


void MainWidget::loadRecentMessages(const QString& chat_session_id)
{
    model::DataCenter* dataCenter = model::DataCenter::getInstance();
    if (dataCenter->getRecentMessageList(chat_session_id) != nullptr) {
        updateRecentMessages(chat_session_id);
    }
    else {
        //获取会话最近消息列表
        connect(dataCenter, &model::DataCenter::getRecentMessageDone, this, &MainWidget::updateRecentMessages, Qt::UniqueConnection);
        dataCenter->getRecentMessageAsync(chat_session_id,true);
    }
}

void MainWidget::updataFriendList()
{
	model::DataCenter* dataCenter = model::DataCenter::getInstance();
	if (this->activeTab != ActiveTab::FRIEND_LIST) {
        return;
	}
    this->session_friendArea->clear();
	const auto* friend_list = dataCenter->getFriendList();
	for (const auto& e : *friend_list) {
		this->session_friendArea->addItem(ItemType::FRIENDITEM, e);
	}

}

void MainWidget::updataChatSessionList()
{
    model::DataCenter* dataCenter = model::DataCenter::getInstance();
    if (this->activeTab != ActiveTab::SESSION_LIST) {
        return;
    }
    this->session_friendArea->clear();
    const  auto* session_list = dataCenter->getChatSessionList();
    for (const auto& e : *session_list) {
        this->session_friendArea->addItem(ItemType::SESSIONITEM, e);
    }
}

void MainWidget::updataFriendApplyList()
{
    model::DataCenter* dataCenter = model::DataCenter::getInstance();
    if (this->activeTab != ActiveTab::APPLY_LIST) {
        return;
    }
    this->session_friendArea->clear();
    const  auto* apply_list  = dataCenter->getApplyList();
    for (const auto& e : *apply_list) {
        this->session_friendArea->addItem(ItemType::FRIENDAPPLYITEM, e);
    }

}

void MainWidget::updateRecentMessages(const QString& chat_session_id)
{
    model::DataCenter* dataCenter = model::DataCenter::getInstance();

    QList<model::Message>* recentList = dataCenter->getRecentMessageList(chat_session_id);


    messageShowArea->clear();
    int list_size = recentList->size();
    for (int i = 0; i < list_size ; ++i) {
        const model::Message& message = recentList->at(i);
        //  不等于代表不是本人发送 所以在left
        bool isLeft = (message.sender.userId != dataCenter->getMySelf()->userId);
        messageShowArea->addItem(isLeft, message);
    }

    //将消息展示框移动到最新一条信息
    messageShowArea->scrollToEndLater();

    model::ChatSessionInfo* info = dataCenter->getChatSessionInfo(chat_session_id);
    if (info != nullptr) {
        this->sessionName->setText(info->chatSessionName);
    }
    else {
        qCritical() << POSITION << "chatSessionId not found,id=" << chat_session_id;

    }


    dataCenter->setCurrentChatSessionId(chat_session_id);

}

MessageShowArea* MainWidget::getMessageShowArea()
{
    return this->messageShowArea;
}

void MainWidget::mousePressEvent(QMouseEvent* event)
{
    int topWidgetHeight = this->topWidget->height();
    int leftWidgetWidth = this->mainWidgetLeft->width();

    int x = event->x();
    int y = event->y();
    //  以上获取的数据是分别是符合条件的区域和鼠标的相对坐标
    // 判断是否符合窗口移动条件
    if (x <= leftWidgetWidth || y <= topWidgetHeight) {
        this->setMoveStatus(true);
    }

    FramelessWidget::mousePressEvent(event);
}

