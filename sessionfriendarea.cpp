#include "sessionfriendarea.h"



SessionFriendArea::SessionFriendArea(QWidget* parent)
    :QScrollArea(parent),selected_item(nullptr)
{
    this->setWidgetResizable(true);
    this->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {width:2px;background:rgb(46,46,46)}");
    this->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal {width:0px;}");

    container = new QWidget(this);
    container->setFixedWidth(210);
    this->setWidget(container);


    QVBoxLayout* layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->setAlignment(Qt::AlignTop);
    container->setLayout(layout);

    /*****   debug *****/
    QIcon xiaoju(":/resource/images/xiaoju.jpg");
    for(int i=0;i<25;++i){
        addItem(FRIENDAPPLYITEM,QString::number(i),this,xiaoju,"xiaoju"+QString::number(i),"hello xiaoju");
    }
    /*****   debug *****/

}


SessionFriendArea::~SessionFriendArea()
{

}

void SessionFriendArea::clear()
{
    QLayout* layout = container->layout();
    for(int i=layout->count()-1;i>=0;++i){
        QLayoutItem* item = layout->takeAt(i);
        if(item->widget()){
            delete item->widget();
        }
        delete item;

    }
}

void SessionFriendArea::addItem(ItemType type,const QString& id,SessionFriendArea* owner, const QIcon &avatar, const QString &name, const QString &lastMessage)
{
    switch(type){
    case SESSIONITEM:{

        SessionItem* item = new SessionItem(owner,id,avatar,name,lastMessage);
        container->layout()->addWidget(item);
        break;
    }
    case FRIENDITEM:{
        FriendItem* item = new FriendItem(owner,id,avatar,name,lastMessage);
        container->layout()->addWidget(item);
        break;
    }
    case FRIENDAPPLYITEM:{
        FriendApplyItem* item = new FriendApplyItem(owner,id,avatar,name);
        container->layout()->addWidget(item);
        break;
    }
    case SESSIONFRIENDITEM:{
        SessionFriendItem* item = new SessionFriendItem(owner,avatar,name,lastMessage);
        container->layout()->addWidget(item);
        break;
    }
    defalut:{

    }

    }


}

void SessionFriendArea::clickItem(int index)
{
    if(index > container->layout()->count() || index < 0){
        LOG()<<"所选index超出范围,index="<<index;
        return;
    }
    QLayoutItem* item = container->layout()->itemAt(index);
    if(item->widget() == nullptr || item == nullptr){
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

void SessionFriendItem::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void SessionFriendItem::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
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
    QWidget::leaveEvent(event);

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
}

/***** *****	会话item		***** *****/
SessionItem::SessionItem(SessionFriendArea *owner, const QString &chatSessionId, const QIcon &avatar, const QString &name, const QString &lastMessage)
    :SessionFriendItem(owner,avatar,name,lastMessage),
    chatSessionId(chatSessionId)
{

}

void SessionItem::avtion()
{
    LOG()<<"Session Item";
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
