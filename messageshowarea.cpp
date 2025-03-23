#include "messageshowarea.h"
#include "debug.h"
#include <QTimer>
#include <QBuffer>
#include <QImageWriter>
#include <QMap>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMenu>
#include "model/datacenter.h"
#include "mainwidget.h"
#include "toast.h"
#include "soundrecorder.h"


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

		addItem(true, message);
		addItem(false, message);
	}

#endif
}

void MessageShowArea::scrollToEndLater()
{
    LOG() << "scrollToEndLater";
    QTimer* timer = new QTimer();
    LOG() << this->verticalScrollBar()->maximum();
    connect(timer, &QTimer::timeout, this, [=]() {
        // 执⾏滚动操作
    LOG() << this->verticalScrollBar()->maximum();
        int maxValue = this->verticalScrollBar()->maximum();
        this->verticalScrollBar()->setValue(maxValue);
    LOG() << this->verticalScrollBar()->maximum();
        // 销毁定时器
        timer->stop();
        timer->deleteLater();
        });
    timer->start(400);
}

void MessageShowArea::addItem(bool isLeft, const model::Message& message)
{
    MessageShowItem* item = MessageShowItem::MakeMessageItem(isLeft,message);
    this->container->layout()->addWidget(item);
    
}

void MessageShowArea::addFrontItem(bool isLeft, const model::Message& message)
{
    MessageShowItem* item = MessageShowItem::MakeMessageItem(isLeft, message);
    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(container->layout());
    layout->insertWidget(0, item);
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
    
	model::DataCenter* dataCenter = model::DataCenter::getInstance();
    MessageShowItem* item = new MessageShowItem(isLeft);
    //item->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setVerticalSpacing(10);
    gridLayout->setHorizontalSpacing(10);
    gridLayout->setContentsMargins(10,0,10,0);
    item->setLayout(gridLayout);
    item->setMinimumHeight(100);

    QPushButton*  avatar_Button = new QPushButton();

    //对于已有一样头像数据的icon则不再使用message中的icon,而是直接用__avatar中的icon
       
    avatar_Button->setIcon(dataCenter->getIcon(message.sender.avatar));


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
    senderName_And_Date->setAlignment(Qt::AlignBottom);
    if(isLeft){
        gridLayout->addWidget(senderName_And_Date,0,1);
    }else{
        gridLayout->addWidget(senderName_And_Date,0,0,Qt::AlignRight);
        //用户修改名称成功后调整名称
        connect(dataCenter, &model::DataCenter::changeNickNameDone,item , [senderName_And_Date,dataCenter,message]() {
            senderName_And_Date->setText(dataCenter->getMySelf()->nickname + "|" + message.time);
            });
        //用户修改头像成功后触发
        connect(dataCenter, &model::DataCenter::changeAvatarDone, item, [avatar_Button, dataCenter]() {
            avatar_Button->setIcon(dataCenter->getIcon(dataCenter->getMySelf()->avatar));
            });
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
	connect(avatar_Button, &QPushButton::clicked, item, [item, message]() {
        MainWidget* mainWidget = MainWidget::getInstance();
		OtherUsrInfoWidget userInfo(message.sender,mainWidget);
		QPoint globalPos = QCursor::pos();
		userInfo.move(globalPos.x(), globalPos.y());
		userInfo.exec();

		});



    return item;
}

QWidget *MessageShowItem::makeTextMessageItem(const QByteArray &content, bool isLeft)
{
    QWidget* res_Widget = new MessageContent(model::TEXT_TYPE,content,"",content,isLeft);

    return res_Widget;
}

QWidget *MessageShowItem::makeImageMessageItem(const QString &fileId, const QByteArray &content, bool isLeft)
{
    QWidget* res_Widget = new MessageImage(fileId,content,isLeft);
    return res_Widget;

}

QWidget *MessageShowItem::makeFileMessageItem(const QString &fileId, const QByteArray &content, bool isLeft, const QString &fileName)
{
    QWidget* res_Widget = new MessageContent(model::FILE_TYPE,"[文件]"+fileName,fileId,content,isLeft);
    return res_Widget;

}

QWidget *MessageShowItem::makeSoundMessgaeItem(const QString &fileId, const QByteArray &content, bool isLeft)
{
    QWidget* res_Widget = new MessageContent(model::SPEECH_TYPE,"[语音]"+fileId,fileId,content,isLeft);

    return res_Widget;

}

MessageContent::MessageContent(model::MessageType type, const QString& text, const QString& fileId, const QByteArray& content, bool isLeft)
	:type(type), fileId(fileId), content(content), isLeft(isLeft), status(false), fileName("")
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

	if (type == model::TEXT_TYPE) {
        return;
	}
	if (content.isEmpty()) {
		model::DataCenter* dataCenter = model::DataCenter::getInstance();
		connect(dataCenter, &model::DataCenter::getSingleFileDone, this, &MessageContent::updateUI, Qt::UniqueConnection);
		dataCenter->getSingleFileAsync(fileId);
	}

}

void MessageContent::updateUI(const QString& fileId, const QByteArray& content)
{
	if (fileId != this->fileId) {
		return;
	}
	this->content = content;
	this->update();
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
        width = totalWidth + 30;
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

void MessageContent::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (type == model::FILE_TYPE) {
			if (content.isEmpty()) {
				Toast::showMessage("文件加载中,请稍后再试");
			    return;
			}
            //未保存,则保存
            if (status == false) {
                saveFile(content);
            }
            else {
                //调用系统命令打开文件
                openFileWithSystemDefaultApp(fileName);
            }

        }
        else if (type == model::SPEECH_TYPE) {
            if (content.isEmpty()) {
                Toast::showMessage("数据加载中");
                return;
            }
            SoundRecorder* recorder = SoundRecorder::getInstance();
            connect(recorder, &SoundRecorder::soundPlayDone, this, [this]() {
                label->setText("[语音]");
                });
            label->setText("播放中...");
            recorder->startPlay(content);
        }
        else {
            LOG() << "未知类型,不支持点击";
        }
	}
	else if (event->button() == Qt::RightButton) {
		LOG() << "右键点击";
    }
    else {
        LOG() << "未知点击";
    }

}

void MessageContent::contextMenuEvent(QContextMenuEvent* event)
{
    if (type != model::SPEECH_TYPE) {
        LOG() << "非语音消息,暂时不支持菜单";
        return;
    }
    QMenu* menu = new QMenu(this);
    QAction* action = menu->addAction("语音转文字");
    connect(action, &QAction::triggered, this, [this]() {
        speechRecognition();
        });
    menu->exec(event->globalPos());
    delete menu;

}

// 通过系统默认程序打开文件
void  MessageContent::openFileWithSystemDefaultApp(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        qDebug() << "File does not exist:" << filePath;
        return;
    }
    // 使用 QDesktopServices 打开文件
    bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    if (!success) {
        qDebug() << "Failed to open file with default application.";
    }
}

void MessageContent::saveFile(const QByteArray& content)
{
	QString filePath = QFileDialog::getSaveFileName(this, "另存为", QDir::homePath(),"*"); 
	if (filePath.isEmpty()) {
		LOG() << "未选择文件保存路径";
        return;
	}
	//保存文件
	bool isOk = model::writeQByteArrayToFile(filePath, content);
	if (isOk) {
		status = true;
		fileName = filePath;
	}
}

void MessageContent::speechRecognition()
{
    model::DataCenter* dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::speechRecognitionDone, this, &MessageContent::speechRecognitionDone, Qt::UniqueConnection);
    dataCenter->speechRecognitionAsync(fileId, content);
}

void MessageContent::speechRecognitionDone(const QString& fileId, bool ok, const QString& errmsg, const QString& text)
{
    if (fileId != this->fileId) {
        return;
    }
    if (!ok) {
        Toast::showMessage("语音转文字失败:" + errmsg);
        return;
    }
    label->setText("语音转文字]" + text);
    this->update();
}

MessageImage::MessageImage(const QString& fileId, const QByteArray& content, bool isLeft)
	:fileId(fileId), content(content), isLeft(isLeft)
{
	this->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
	this->setStyleSheet("QPushButton{border:none;}");
	//this->setFixedSize(200, 200);
    image = new QPushButton(this);
    if (content.isEmpty()) {
		model::DataCenter* dataCenter = model::DataCenter::getInstance();
		connect(dataCenter, &model::DataCenter::getSingleFileDone, this,&MessageImage::updateUI,Qt::UniqueConnection);
		dataCenter->getSingleFileAsync(fileId);
    }


}

void MessageImage::updateUI(const QString& fileId, const QByteArray& content)
{
	if (fileId != this->fileId) {
		return;
	}
	this->content = content;
	this->update();

}

void MessageImage::paintEvent(QPaintEvent* event)
{
	QObject* obj = this->parent();
	if (!obj->isWidgetType()) {
		return;
	}
	QWidget* parent = dynamic_cast<QWidget*> (obj);
	int width = parent->width() * 0.55;
	QImage display_image;
    if (content.isEmpty()) {
        display_image.load(":/resource/images/xiaoju.jpg");
    }
    else {
		display_image.loadFromData(content);
    }
	int height = display_image.height() * width / display_image.width();
    QPixmap pixmap = QPixmap::fromImage(display_image);
	image->setIconSize(QSize(width, height));
	image->setIcon(QIcon(pixmap));
    //设置父元素高度
	parent->setFixedHeight(height + 50);
    //设置图片位置
	if (isLeft) {
		image->setGeometry(10, 0, width, height);
	}
	else {
		image->setGeometry(parent->width() - width - 10, 0, width, height);
	}
	
}



