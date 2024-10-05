#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
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
    this->setWindowTitle("我的聊天室");
    this->setWindowIcon(QIcon(":/resource/images/xiaoju.jpg"));


    initMainWidget();

    initMainWidgetLeft();	//头像，会话，好友，好友申请按钮
    initMainWidgetMid();	//会话、好友、好友申请列表
    initMainWidgetRight();	//会话信息

    initMainWidgetSignal();//绑定信号槽
}

// 初始化主页面
void MainWidget::initMainWidget()
{
    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);
    this->setMinimumHeight(600);

    mainWidgetLeft = new QWidget();
    mainWidgetMid = new QWidget();
    mainWidgetRight = new QWidget();

    mainWidgetLeft->setFixedWidth(60);
    mainWidgetMid->setFixedWidth(200);
    mainWidgetRight->setMinimumWidth(700);

    mainWidgetLeft->setStyleSheet("background-color:rgb(45,45,45);");
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
    userAvatar->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
    vBoxLayout->addWidget(userAvatar,1,Qt::AlignTop | Qt::AlignCenter);

    //会话列表按钮初始化
    sessionTabBtn = new QPushButton();
    sessionTabBtn->setFixedSize(45,45);
    sessionTabBtn->setIconSize(QSize(35,35));
    sessionTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
    vBoxLayout->addWidget(sessionTabBtn,1,Qt::AlignTop | Qt::AlignCenter);

    //好友列表按钮初始化
    friendTabBtn = new QPushButton();
    friendTabBtn->setFixedSize(45,45);
    friendTabBtn->setIconSize(QSize(35,35));
    friendTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
    vBoxLayout->addWidget(friendTabBtn,1,Qt::AlignTop | Qt::AlignCenter);

    //好友申请列表初始化
    applyTabBtn = new QPushButton();
    applyTabBtn->setFixedSize(45,45);
    applyTabBtn->setIconSize(QSize(35,35));
    applyTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
    vBoxLayout->addWidget(applyTabBtn,1,Qt::AlignTop | Qt::AlignCenter);

    applyTabBtn->setStyleSheet("QPushButton{background-color:transparent;}");
    vBoxLayout->addStretch(20);//空白
}

void MainWidget::initMainWidgetMid()
{
    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(0);
    gridLayout->setVerticalSpacing(5);
    gridLayout->setContentsMargins(0,5,0,0);
    mainWidgetMid->setLayout(gridLayout);

    //空白占位
    QWidget* widget1= new QWidget();
    widget1->setFixedWidth(10);
    QWidget* widget2= new QWidget();
    widget1->setFixedWidth(10);
    QWidget* widget3= new QWidget();
    widget1->setFixedWidth(10);

    // 搜索按钮和输入框
    search_input = new QTextEdit();
    search_input->setFixedHeight(30);
    search_input->setPlaceholderText("搜索");
    search_input->setStyleSheet("QTextEdit {border-radius:5px; padding-left:5px; background-color:rgb(226,226,226); } ");
    gridLayout->addWidget(widget1,0,0);
    gridLayout->addWidget(search_input,0,1);
    gridLayout->addWidget(widget2,0,2);

    search_submit = new QPushButton();
    search_submit->setFixedSize(30,30);
    search_submit->setIconSize(QSize(25,25));
    search_submit->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
    search_submit->setStyleSheet("QTextEdit {border-radius:5px;background-color:rgb(226,226,226)}"
                                 " QPushButton:pressed{background-color:rgb(240,240,240)} ");
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
    QWidget* topWidget = new QWidget();
    topWidget->setFixedHeight(40);
    topWidget->setStyleSheet("QWidget { border-bottom:1px solid rgb(231,231,231);border-left:1px solid rgb(231,231,231); }");

    //
    QHBoxLayout* topWidgetLayout = new QHBoxLayout();
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
    sessionName->setStyleSheet("QWidget {border-left: none }");
    topWidgetLayout->addWidget(sessionName,0,Qt::AlignLeft);

    // 更多按钮
    QPushButton* more_Button = new QPushButton();
    more_Button->resize(QSize(30,30));
    more_Button->setIconSize(QSize(30,30));
    more_Button->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    more_Button->setIcon(QIcon(":/resource/images/more.png"));
    more_Button->setStyleSheet("QPushButton { border: none ; background-color:transparent ;}");
    topWidgetLayout->addWidget(more_Button,0,Qt::AlignRight);

    vBoxLayout->addWidget(topWidget,0,Qt::AlignTop);

    //会话展示界面
    MessageShowArea* messageShowArea  = new MessageShowArea();

    vBoxLayout->addWidget(messageShowArea);

    //消息输入界面
    MessageEditArea* messageEditArea = new MessageEditArea();

    vBoxLayout->addWidget(messageEditArea,0,Qt::AlignBottom);





}

// 用于绑定信号与槽
void MainWidget::initMainWidgetSignal()
{

    //会话列表按钮信号绑定
    connect(sessionTabBtn,&QPushButton::clicked,this,&MainWidget::switchTabSession);
    //好友列表按钮信号绑定
    connect(friendTabBtn,&QPushButton::clicked,this,&MainWidget::switchTabFriend);
    //好友申请列表信号绑定
    connect(applyTabBtn,&QPushButton::clicked,this,&MainWidget::switchTabApply);

}

//			改变Tab按钮的Icon状态，activTab失去焦点Icon，Tab获取焦点Icon,并将activeTab设置为Tab；
void MainWidget::changeTabIconAction(MainWidget::ActiveTab& activeTab,MainWidget::ActiveTab tab){
    // 失去焦点设置
    switch(activeTab){
    case MainWidget::SESSION_LIST:
        sessionTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
        break;
    case MainWidget::FRIEND_LIST:
        friendTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
        break;
    case MainWidget::APPLY_LIST:
        applyTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
        break;
    default:
        sessionTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
        friendTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
        applyTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
    }

    //获取焦点设置
    switch(tab){
    case MainWidget::SESSION_LIST:
        sessionTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
        break;
    case MainWidget::FRIEND_LIST:
        friendTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
        break;
    case MainWidget::APPLY_LIST:
        applyTabBtn->setIcon(QIcon(":/resource/images/xiaoju.jpg"));
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

}

void MainWidget::loadFriendList()
{

}

void MainWidget::loadApplyList()
{

}
