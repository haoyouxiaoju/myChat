#include "messageshowarea.h"


MessageShowArea::MessageShowArea(QWidget * parent):
    QScrollArea(parent)
{
    container = new QWidget();


    QVBoxLayout* vBoxLayout = new QVBoxLayout();
    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(15,0,15,0);
    //vBoxLayout->setAlignment(Qt::AlignBottom);
    container->setLayout(vBoxLayout);

    this->setWidgetResizable(true);
    this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    this->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{ width:2px;background-color:rgb(240,240,240); } \
    QScrollBar::handle:vertical{background-color:rgb(46,46,46)}");
    this->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{height:0px}");
    this->setStyleSheet("QScrollArea{border:none}");
    this->setWidget(container);

    //debug
    model::Message message;
    model::UserInfo user;
    user.userId = "1";
    user.nickname = "haoyouxiaoju";
    user.description = "";
    user.phone = "";
    user.avatar = QIcon(":/resource/images/xiaoju.jpg");
    message = model::Message::MakeMessage(model::TEXT_TYPE, "", user, "好好好好好好好好好好好友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊好好好好好好好好好友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊", "");


    for (int i = 0; i < 20; ++i) {
        addItem(true, message);
        addItem(false, message);
    }

}

void MessageShowArea::addItem(bool isLeft, const model::Message& message)
{
    MessageShowItem* item = MessageShowItem::MakeMessageItem(isLeft,message);
    this->container->layout()->addWidget(item);
    
}

void MessageShowArea::clear()
{
    QLayout* layout = this->container->layout();
    for (int i = layout->count() - 1; i >= 0; --i) {
        QLayoutItem* item = layout->takeAt(i);
        if ( item->widget() != nullptr) {
            delete item->widget();
        }
        if (item != nullptr) {
            delete item;
        }
    }
}

MessageShowItem::MessageShowItem(bool isLeft)
    :QWidget(),isLeft(isLeft)
{

}

MessageShowItem *MessageShowItem::MakeMessageItem(bool isLeft, const model::Message &message)
{
    MessageShowItem* item = new MessageShowItem(isLeft);
    item->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(5);
    gridLayout->setHorizontalSpacing(15);
    gridLayout->setContentsMargins(0,0,0,0);
    item->setLayout(gridLayout);

    QPushButton*  avatar_Button = new QPushButton();
    avatar_Button->setIcon(message.sender.avatar);
    avatar_Button->setFixedSize(35,35);
    avatar_Button->setIconSize(QSize(35, 35));
    avatar_Button->setStyleSheet("QPushButton{ border:none;background-color:transparent }");
    if(isLeft){
        gridLayout->addWidget(avatar_Button,0,0,2,1,Qt::AlignLeft|Qt::AlignTop);
    }else{
        gridLayout->addWidget(avatar_Button,0,1,2,1,Qt::AlignLeft|Qt::AlignTop);
    }

    QLabel* senderName_And_Date = new QLabel();
    senderName_And_Date->setText(message.sender.nickname+"|"+message.time);
    QFont little_Font;
    little_Font.setPointSize(12);
    senderName_And_Date->setFont(little_Font);
    senderName_And_Date->setPalette(QPalette(QColor(178,178,178)));
    senderName_And_Date->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
    if(isLeft){
        gridLayout->addWidget(senderName_And_Date,0,1);
    }else{
        gridLayout->addWidget(senderName_And_Date,0,0,Qt::AlignRight);
    }

    QWidget* messageStruct = nullptr;
    switch(message.messageType){
    case model::TEXT_TYPE:
        messageStruct = makeTextMessageItem(message.content,isLeft);
        break;
    case model::FILE_TYPE:
        messageStruct = makeFileMessageItem(message.fileId,message.content,isLeft,message.fileName);
        break;
    case model::IMAGE_TYPE:
        messageStruct = makeImageMessageItem(message.fileId,message.content,isLeft);
        break;
    case model::SPEECH_TYPE:
        messageStruct = makeSoundMessgaeItem(message.fileId,message.content,isLeft);
        break;
    default:
        LOG()<<"error to messageType ,value:"<<message.messageType;
    }

    if(isLeft){
        gridLayout->addWidget(messageStruct,1,1);
    }else{
        gridLayout->addWidget(messageStruct,1,0);
    }


    return item;
}

QWidget *MessageShowItem::makeTextMessageItem(const QByteArray &content, bool isLeft)
{
    QWidget* res_Widget = new MessageContent(model::TEXT_TYPE,content,"",content,isLeft);

    return res_Widget;
}

QWidget *MessageShowItem::makeImageMessageItem(const QString &fileId, const QByteArray &content, bool isLeft)
{
    QWidget* res_Widget = new QWidget();

    return res_Widget;

}

QWidget *MessageShowItem::makeFileMessageItem(const QString &fileId, const QByteArray &content, bool isLeft, const QString &fileName)
{
    QWidget* res_Widget = new QWidget();

    return res_Widget;

}

QWidget *MessageShowItem::makeSoundMessgaeItem(const QString &fileId, const QByteArray &content, bool isLeft)
{
    QWidget* res_Widget = new QWidget();

    return res_Widget;

}

MessageContent::MessageContent(model::MessageType type, const QString& text, const QString& fileId, const QByteArray& content, bool isLeft)
    :type(type),fileId(fileId),content(content),isLeft(isLeft)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QFont font;
    font.setPixelSize(16);
    font.setFamily("微软雅黑");
  

    this->label = new QLabel(this);
    this->label->setText(text);
    this->label->setFont(font);
    this->label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    this->label->setWordWrap(true);
    this->label->setStyleSheet("QLabel{padding:0 10px;line-height:1.2;background-color:transparent;}");

}

void MessageContent::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    
    QObject* obj = this->parent();
    if (!obj->isWidgetType()) {
        return;
    }
    QWidget* parent = dynamic_cast<QWidget*> (obj);

    int width = parent->width() * 0.55;

    QFontMetrics metrics(this->label->font());
    int totalWidth = metrics.horizontalAdvance(label->text());
    
    int line_nums = (totalWidth / (width - 30)) + 1;
    if (line_nums == 1) {
        width = totalWidth + 10;
    }
    int height = line_nums * (this->label->font().pixelSize() * 1.2) + 20;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    
	if (isLeft) {
		painter.setPen(QPen(QColor(255, 255, 255)));
		painter.setBrush(QColor(255, 255, 255));


        path.addRoundedRect(QRect(10, 0, width, height), 10, 10);
        path.moveTo(10, 15);
        path.lineTo(0,20);
        path.lineTo(10, 25);
        path.closeSubpath();

        painter.drawPath(path);

        this->label->setGeometry(5, 0,width,height);
    }
    else {
        painter.setPen(QPen(QColor(65, 205, 82)));
        painter.setBrush(QColor(65, 205, 82));

        int leftPos = this->width() - width - 10;
        int rightPos = this->width() - 10;

        path.addRoundedRect(QRect(leftPos, 0, width, height), 10, 10);
        path.moveTo(rightPos, 15);
        path.lineTo(rightPos + 10, 20);
        path.lineTo(rightPos, 25);
        path.closeSubpath();

        painter.drawPath(path);

        this->label->setGeometry(leftPos , 0 , width,height);
    }

    parent->setFixedHeight(height + 50);




}


