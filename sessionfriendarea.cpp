#include "sessionfriendarea.h"
#include "mainwidget.h"


#include "debug.h"

SessionFriendArea::SessionFriendArea(QWidget* parent)
    :QScrollArea(parent),selected_item(nullptr)
{
    this->setWidgetResizable(true);
    this->verticalScrollBar()->setStyleSheet("      \
      QScrollBar{width:6px;background-color:rgb(231,231,231)}  \
      QScrollBar:handle:vertical{border-radius:3px;}  \
      QScrollBar::handle:vertical:normal {width:6px;background-color:transparent}    \
      QScrollBar::handle:vertical:hover{width:6px;background-color:rgb(143,143,143)}  \
      QScrollBar::handle:vertical:pressed{width:6px;background-color:rgb(144,143,143)} \
      QScrollBar::add-page:vertical,QScrollBar::sub-page:vertical{background-color:transparent} \
      QScrollBar::add-line:vertical,QScrollBar::sub-line:vertical{width:0px}");
     
    this->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {width:0px;}");

    container = new QWidget(this);
    container->setFixedWidth(210);
    this->setWidget(container);


    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->setAlignment(Qt::AlignTop);
    container->setLayout(layout);

#if TEXT_UI
    /*****   debug *****/
    QIcon xiaoju(":/resource/images/xiaoju.jpg");
    for(int i=0;i<25;++i){
        addItem(FRIENDAPPLYITEM,QString::number(i),this,xiaoju,"xiaoju"+QString::number(i),"hello xiaoju");
    }
    /*****   debug *****/
#endif
}


SessionFriendArea::~SessionFriendArea()
{

}

void SessionFriendArea::clear()
{
    QLayout* layout = container->layout();
    //
    // 在切换tab的时候会进行item清空，如果此时不将上次tab栏的所选中的item取消掉
    // 在切换tab后的下次选中select() 会在selected_item 不为空的时候设置样式 改变item样式
    //  但此时的selected_item 已被delete 无法进行设置 所以会导致段错误
    if (this->selected_item) {
		this->selected_item->selected = false;
    }
    this->selected_item = nullptr;
    for(int i=layout->count()-1;i>=0;--i){
        QLayoutItem* item = layout->takeAt(i);
        if(item->widget()){
            delete item->widget();
        }
        delete item;

    }
}

void SessionFriendArea::addItem(ItemType type,const QString& id, const QIcon &avatar, const QString &name, const QString &lastMessage)
{
	SessionFriendItem* item = nullptr;
	switch (type) {
	case SESSIONITEM: {

		item = new SessionItem(this, id, avatar, name, lastMessage);
		break;
	}
	case FRIENDITEM: {
		item = new FriendItem(this, id, avatar, name, lastMessage);
		break;
	}
	case FRIENDAPPLYITEM: {
		item = new FriendApplyItem(this, id, avatar, name);
		break;
	}
	case SESSIONFRIENDITEM: {
		item = new SessionFriendItem(this, avatar, name, lastMessage);
		break;
	}
    defalut: {

	}

	}
	container->layout()->addWidget(item);


}

void SessionFriendArea::addItem(ItemType type,const model::UserInfo& info)
{
    this->addItem(type, info.userId, info.avatar, info.nickname,"");
}

void SessionFriendArea::addItem(ItemType type, const model::ChatSessionInfo& info) {
    switch (info.lastMessage.messageType)
    {
    case model::MessageType::TEXT_TYPE: {
        this->addItem(type, info.chatSessionId, info.avatar, info.chatSessionName, QString(info.lastMessage.content));
        break;
    }
    case model::MessageType::FILE_TYPE: {
		this->addItem(type, info.chatSessionId, info.avatar, info.chatSessionName,"[文件]");
        break;
    }
    case model::MessageType::IMAGE_TYPE: {
		this->addItem(type, info.chatSessionId, info.avatar, info.chatSessionName,"[图片]");
        break;
    }
    case model::MessageType::SPEECH_TYPE: {
		this->addItem(type, info.chatSessionId, info.avatar, info.chatSessionName,"[语音]");
        break;
    }
    default:
        qCritical() <<POSITION <<"messageType 错误！messageType=" << info.lastMessage.messageType;
        break;
    }
}

void SessionFriendArea::clickItem(int index)
{
    if(index >= container->layout()->count()){
        LOG()<<"所选index超出范围,index="<<index;
        return;
    }
    QLayoutItem* item = container->layout()->itemAt(index);
    if(item->widget() == nullptr){
        LOG()<<"指定元素不存在，index="<<index;
        return;
    }
    SessionFriendItem* SFitem = dynamic_cast<SessionFriendItem*> (item->widget());
    SFitem->select();

}

SessionFriendItem::SessionFriendItem(SessionFriendArea* owner, const QIcon &avatar, const QString &name, const QString &lastMessage)
    :owner(owner),selected(false)
{
    this->setFixedHeight(60);
    this->setStyleSheet("QWidget{background-color:rgb(231,231,231)}");
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    layout = new QGridLayout();
    layout->setHorizontalSpacing(10);
    layout->setVerticalSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout);

    __avatar = new QPushButton();
    __avatar->setIconSize(QSize(50,50));
    __avatar->setFixedSize(50,50);
    __avatar->setIcon(avatar);
    __avatar->setStyleSheet("QPushButton{background-color:rgb(231,231,231);}");

    nameLabel = new QLabel(name);
    nameLabel->setFixedHeight(30);
    nameLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: 600; }");
    nameLabel->setAlignment(Qt::AlignBottom);


    MessageLabel = new QLabel(lastMessage);
    MessageLabel->setFixedHeight(30);
    MessageLabel->setStyleSheet("QLabel { font-size: 10px; font-weight: 300; }");

    layout->addWidget(__avatar,0,0,2,2);
    layout->addWidget(nameLabel,0,2,1,8);
    layout->addWidget(MessageLabel,1,2,1,8);
}

void SessionFriendItem::paintEvent(QPaintEvent *event)
{
    (void)event;
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SessionFriendItem::mousePressEvent(QMouseEvent *event)
{
    //QWidget::mousePressEvent(event);
    (void)event;
    this->select();
}

void SessionFriendItem::enterEvent(QEnterEvent *event)
{
    QWidget::enterEvent(event);

    if(this->selected)
        return;

    this->setStyleSheet("QWidget{background-color:rgb(215,215,215)}");
}

void SessionFriendItem::leaveEvent(QEvent *event)
{
    //QWidget::leaveEvent(event);

    (void)event;
    if(this->selected)
        return;

    this->setStyleSheet("QWidget{background-color:rgb(231,231,231)}");

}

void SessionFriendItem::avtion()
{
    LOG()<<"";
}

void SessionFriendItem::select(){

    SessionFriendItem* selected_item = owner->selected_item;
    if(selected_item!=nullptr){
        selected_item->setStyleSheet("QWidget{background-color:rgb(231,231,231)}");
        selected_item->selected = false;
    }

    this->setStyleSheet("QWidget{background-color:rgb(200,200,200)}");
    this->selected = true;
    owner->selected_item = this;

    //调用自己被点击后需要进行的活动
    this->avtion();
}

/***** *****	会话item		***** *****/
SessionItem::SessionItem(SessionFriendArea *owner, const QString &chatSessionId, const QIcon &avatar, const QString &name, const QString &lastMessage)
    :SessionFriendItem(owner,avatar,name,lastMessage),
    chatSessionId(chatSessionId)
{

}

void SessionItem::avtion()
{
    LOG()<<"Session Item,chatSessionId="<<chatSessionId;

    MainWidget* m_widget = MainWidget::getInstance();
    m_widget->loadRecentMessages(chatSessionId);
    
}

/***** *****	会话item		***** *****/
/***** *****	 END		***** *****/




/***** *****	好友item		***** *****/
FriendItem::FriendItem(SessionFriendArea *owner, const QString &userId, const QIcon &avatar, const QString &name, const QString &description)
    :SessionFriendItem(owner,avatar,name,description),
    userId(userId)
{

}

void FriendItem::avtion()
{
    LOG()<<"FriendItem";

    
    MainWidget* m_widget = MainWidget::getInstance();
    m_widget->switchToSession(userId);

}
/***** *****	好友item		***** *****/
/***** *****	 END		***** *****/




/***** *****	好友申请item		***** *****/
FriendApplyItem::FriendApplyItem(SessionFriendArea *owner, const QString &userId, const QIcon &avatar, const QString &name)
    :SessionFriendItem(owner,avatar,name,""),
    userId(userId)
{
    layout->removeWidget(MessageLabel);
    delete MessageLabel;

    QPushButton* agree = new QPushButton("同意");
    layout->addWidget(agree,1,2,1,3);
    QPushButton* disagree = new QPushButton("拒绝");
    layout->addWidget(disagree,1,5,1,3);



}

void FriendApplyItem::avtion()
{
    LOG()<<"FriendApplyItem";
}

/***** *****	好友申请item		***** *****/
/***** *****	 END		***** *****/
