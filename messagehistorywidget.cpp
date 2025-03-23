#include "messagehistorywidget.h"
#include "model/data.h"
#include "model/datacenter.h"
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include "debug.h"
#include "toast.h"

messageHistoryWidget* messageHistoryWidget::w = nullptr;

messageHistoryWidget* messageHistoryWidget::getMessageHistoryWidget()
{
    if (w == nullptr) {
        w = new messageHistoryWidget();
    }
    return w;
}

messageHistoryWidget::messageHistoryWidget(QWidget *parent)
	: FramelessWidget(parent)
{

    layout = new QGridLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    this->setLayout(layout);
    this->setFixedSize(550, 700);
    this->setObjectName("obj_widget");
    this->setStyleSheet(QString("QPushButton{border:none;}  \
                                  QWidget#obj_widget{ border:1px solid rgb(193, 193, 193)}  "));
    this->setAttribute(Qt::WA_DeleteOnClose);

    //*
    //// 设置阴影
    //this->setStyleSheet(".obj_widget{ border:1px solid rgb(255,255,255)}");
    //QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    //shadow->setOffset(0, 0);//设置阴影距离
    //shadow->setColor(QColor('#444'));
    //shadow->setBlurRadius(30);
    //this->setGraphicsEffect(shadow);
    //this->setContentsMargins(1, 1, 1, 1);

    //*
    //历史记录弹窗的标题
    title = new QLabel();
    QFont font;
    font.setFamily("宋体");
    font.setPixelSize(17);
    title->setFont(font);
    title->setFixedHeight(35);
    title->setAlignment(Qt::AlignCenter);
    //title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout->addWidget(title, 0, 0, 1, 8);
    QPushButton* close_button = new QPushButton();
    close_button->setFixedSize(35, 35);
    //close_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    close_button->setIcon(QIcon(":/resource/images/close.png"));
    QPushButton* minimized_button = new QPushButton();
    minimized_button->setFixedSize(35, 35);
    minimized_button->setIcon(QIcon(":/resource/images/minimize.png"));

    
    layout->addWidget(minimized_button, 0, 8, 1, 1);
    layout->addWidget(close_button, 0, 9, 1, 1);

    connect(close_button, &QPushButton::clicked, this, [this]() {
        this->close();
        //单例 所以不能简单直接关闭所以使用hide
        //this->hide();
        });
    connect(minimized_button, &QPushButton::clicked, this, [this]() {
        LOG() << "minimized";
        this->minimizeWidget();
        });


#ifdef TEXT_UI

    title->setText("haoyouxiaojuhaoyouxiaoju");
#endif // DEBUG
    

    //*
    // 搜索输入框
    QPushButton* searchIcon = new QPushButton();
    searchIcon->setIconSize(QSize(30,30));
    searchIcon->setFixedSize(30, 30);
    searchIcon->setIcon(QIcon(":/resource/images/sousuo.png"));
    searchIcon->setEnabled(true);
    search = new QLineEdit();
    search->setFont(font);
    search->setPlaceholderText("搜索");
    search->setFixedHeight(30);
    search->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    layout->addWidget(search, 1, 0, 1, 9);
    layout->addWidget(searchIcon,1, 9, 1, 1);

	connect(searchIcon, &QPushButton::clicked, this,&messageHistoryWidget::clickSearchBtn);

    //*
    // 文件   图片 ...
    QPushButton* file_button = new QPushButton("文件");
    file_button->setFixedHeight(50);
    file_button->setPalette(QPalette(QColor(115, 132, 166)));
    
    layout->addWidget(file_button, 2, 0, 1, 1);




    QWidget* message_list_widget = new QWidget();
    message_list_widget_layout = new QVBoxLayout();
    message_list_widget_layout->setSpacing(0);
    message_list_widget_layout->setContentsMargins(10, 0, 10, 0);
    message_list_widget->setLayout(message_list_widget_layout);

    QScrollArea* area = new QScrollArea();
    area->setWidget(message_list_widget);

    area->setWidgetResizable(true);
    area->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    area->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{ width:2px;background-color:rgb(240,240,240); } \
    QScrollBar::handle:vertical{background-color:rgb(46,46,46)}");
    area->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{height:0px}");
    area->setStyleSheet("QScrollArea{ border:1px solid rgb(193, 193, 193)} }");

    layout->addWidget(area, 3, 0, 7, 10);

#if TEXT_UI
	for (int i = 0; i < 5; ++i) {
		model::Message message;
		model::UserInfo user;
		user.userId = QString::number(i);
		user.nickname = "haoyouxiaoju";
		user.description = "";
		user.phone = "";
		user.avatar = QIcon(":/resource/images/xiaoju.jpg");
		message = model::Message::MakeMessage(model::TEXT_TYPE, "", user, "好好好好好好好好好好好友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊好好好好好好好好好友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊友小菊", "");

		addItem( message);
		addItem( message);
	}
#endif


}

messageHistoryWidget::~messageHistoryWidget()
{
    if (w != nullptr) {
        //delete w;
        w = nullptr;
    }
}

void messageHistoryWidget::clickSearchBtn()
{
	model::DataCenter* dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::searchMessageDone, this, &messageHistoryWidget::clickSearchBtnDone);
	if (search->text().isEmpty()) {
		Toast::showMessage("搜索内容不能为空");
		return;
	}
	dataCenter->searchMessageAsync(search->text());
}

void messageHistoryWidget::clickSearchBtnDone()
{
	model::DataCenter* dataCenter = model::DataCenter::getInstance();
	QList<model::Message>* messageList = dataCenter->getSearchMessageResult();
		LOG() << "messageList.size() = " << messageList->size();
	if (messageList == nullptr || messageList->size() == 0) {
		Toast::showMessage("搜索历史消息失败");
		return;
	}
    this->clear();
	for (int i = 0; i < messageList->size(); ++i) {
		addItem(messageList->at(i));
	}
}

void messageHistoryWidget::addItem(const model::Message& message)
{
	MessageShowItem* item = MessageShowItem::MakeMessageItem(true,message);
    message_list_widget_layout->addWidget(item);

}

void messageHistoryWidget::clear()
{
    //清空消息
	int count = message_list_widget_layout->count();
	for (int i = count - 1; i >= 0; --i) {
		QWidget* widget = message_list_widget_layout->itemAt(i)->widget();
        if (widget == nullptr) {
            continue;
        }
		message_list_widget_layout->removeWidget(widget);
		delete widget;
	}
}

void messageHistoryWidget::mousePressEvent(QMouseEvent* event)
{
    int y = title->height();
    if (event->y() < y) {
        this->setMoveStatus(true);
    }
    FramelessWidget::mousePressEvent(event);
    
}
