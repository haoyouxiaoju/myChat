#include "choosefrienddialog.h"
#include <QPainter>
#include "model/data.h"
#include "model/datacenter.h"
#include "toast.h"


using namespace model;

ChooseFriendDialog::ChooseFriendDialog(const QString& usrId,QWidget *parent)
	: QDialog(parent),usrId(usrId)
{
	//左右俩个
	layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);

	//this->setFixedSize(600, 440);
	this->setWindowFlags(Qt::WindowCloseButtonHint );
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setLayout(layout);

	//选中好友
	initLeftWidget();
	//已选中好友
	initRightWidget();
	model::DataCenter* dataCenter = model::DataCenter::getInstance();
	//总好友列表添加
	QList<model::UserInfo>* friendList = dataCenter->getFriendList();
	if (friendList == nullptr) {
		return;
	}
	for (auto& friendInfo : *friendList) {
		if (friendInfo.userId == usrId) {
			addFriend(friendInfo.userId, dataCenter->getIcon(friendInfo.avatar), friendInfo.nickname, true);
		}
		addFriend(friendInfo.userId, dataCenter->getIcon(friendInfo.avatar), friendInfo.nickname, false);
	}
	
	//debug
#if TEXT_UI
	addFriend("hayouxiaoju", QIcon(":/resource/images/xiaoju.jpg"),"haoyouxiaojuhohohohohohohoh",false);
	addFriend("hayouxiaoju", QIcon(":/resource/images/xiaoju.jpg"),"haoyouxiaojuhohohohohohohoh",false);
	addFriend("hayouxiaoju", QIcon(":/resource/images/xiaoju.jpg"),"haoyouxiaojuhohohohohohohoh",false);
	addFriend("hayouxiaoju", QIcon(":/resource/images/xiaoju.jpg"),"haoyouxiaojuhohohohohohohoh",false);
	addFriend("hayouxiaoju", QIcon(":/resource/images/xiaoju.jpg"),"haoyouxiaojuhohohohohohohoh",false);
#endif
	//debug-end


}

ChooseFriendDialog::~ChooseFriendDialog()
{}
// 用于左边总好友列表添加
void ChooseFriendDialog::addFriend(const QString& userId, const QIcon& avatar, const QString& name, bool checked)
{
	QVBoxLayout* left_layout = dynamic_cast<QVBoxLayout*>(left_widget_scroll->layout());
	ChooseFriendItem* item = new ChooseFriendItem(this,userId, avatar, name, checked);
	left_layout->addWidget(item);
}

// 选中item 右边添加item
void ChooseFriendDialog::addSelectedFriend(const QString& userId, const QIcon& avatar, const QString& name)
{
	QVBoxLayout* right_layout = dynamic_cast<QVBoxLayout*>(right_widget_scroll->layout());
	ChooseFriendItem* item = new ChooseFriendItem(this,userId, avatar, name, true);
	right_layout->addWidget(item);
}

// 取消选中 左右都取消
void ChooseFriendDialog::deleteSelectedFriend(const QString& userId)
{
	// 删去右边 符合item
	QVBoxLayout* right_layout = dynamic_cast<QVBoxLayout*>(right_widget_scroll->layout());
	int count = right_layout->count();
	for (int i = count - 1; i >= 0;--i) {
		QLayoutItem* layoutItem = right_layout->itemAt(i);
		if (layoutItem == nullptr || layoutItem->widget() == nullptr) {
			continue;
		}
		ChooseFriendItem* item = dynamic_cast<ChooseFriendItem*>(layoutItem->widget());
		if (item->userId == userId) {
			right_layout->removeWidget(item);
			item->deleteLater();
			break;

		}
	}

	//删去左边 符合item
	QVBoxLayout* left_layout = dynamic_cast<QVBoxLayout*>(left_widget_scroll->layout());
	int count_left_layout = left_layout->count();
	for (int i = count_left_layout-1; i >= 0; --i) {
		QLayoutItem* layoutItem = left_layout->itemAt(i);
		if (layoutItem == nullptr || layoutItem->widget() == nullptr) {
			continue;
		}
		ChooseFriendItem* item = dynamic_cast<ChooseFriendItem*>(layoutItem->widget());
		if (item->userId == userId) {
			item->checkBox->setChecked(false);
			break;
		}
	}

	
}

QList<QString> ChooseFriendDialog::generateMemberList()
{
	QList<QString> userIdList;
	DataCenter* dataCenter = DataCenter::getInstance();
	UserInfo* userInfo = dataCenter->getMySelf();
	if (userInfo == nullptr) {
		LOG() << "获取自己信息失败";
		return userIdList;
	}
	//添加自己
	userIdList.push_back(userInfo->userId);
	//添加选中的好友
	QVBoxLayout* right_layout = dynamic_cast<QVBoxLayout*>(right_widget_scroll->layout());
	int count = right_layout->count();
	for (int i = 0; i < count; i++) {
		QLayoutItem* item =  right_layout->itemAt(i);
		if (item == nullptr || item->widget() == nullptr) {
			continue;
		}
		userIdList.push_back(dynamic_cast<ChooseFriendItem*>(item->widget())->userId);
	}

	return userIdList;
}

// 确认键 slot
void ChooseFriendDialog::OK_Slot() {

	//生成会话成员列表
	QList<QString> userIdList = generateMemberList();
	if (userIdList.size() <= 2) {
		LOG() << "选中成员数⽬不⾜ 2 个, 不能创建群聊!";
		Toast::showMessage("至少选中俩名成员");
		return;
	}
	DataCenter* dataCenter = DataCenter::getInstance();
	dataCenter->createGroupChatSessionAsync(userIdList);
	this->accept();
}
// 取消键 slot
void ChooseFriendDialog::Cannot_Slot()
{
	this->reject();
}

// 初始化左边
void ChooseFriendDialog::initLeftWidget()
{
	//创建一个放入layout[0] 采用VBoxLayout
	//QScrollWidget(chooseBox+avatar+label)
	left_widget_scroll= new QWidget();
	left_widget_scroll->setObjectName("LeftWidget");
	left_widget_scroll->setStyleSheet("#LeftWidget{background-color:rgb(255,255,255)}");
	//left_widget_scroll->setFixedSize(280, 440);

	QVBoxLayout* left_layout = new QVBoxLayout();
	left_layout->setSpacing(0);
	left_layout->setContentsMargins(5, 10, 0, 0);
	left_layout->setAlignment(Qt::AlignTop | Qt::AlignCenter);
	left_widget_scroll->setLayout(left_layout);

	QScrollArea* area = new QScrollArea();
	area->setWidget(left_widget_scroll);
	area->setWidgetResizable(true);
	area->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{height:0px;border:none}");
	area->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{width:2px;background-color:rgb(255,255,255)} \
												QScrollBar::handle:vertical{background:rgb(199, 198, 198);border-radius:3px;}");
	layout->addWidget(area);
	


}

//初始化右边
void ChooseFriendDialog::initRightWidget()
{
	//创建一个widget放入groudlayout[1]  采用QGridLayout
	// label+QCrollwidget(...) + two button_s
	QGridLayout* right_widget_layout = new QGridLayout();
	right_widget_layout->setSpacing(0);
	right_widget_layout->setContentsMargins(5, 0, 0, 10);
	right_widget_layout->setAlignment(Qt::AlignTop);

	//右界面
	QWidget* right_widget = new QWidget();
	right_widget->setFixedSize(320, 440);
	right_widget->setObjectName("RightWidget");
	right_widget->setStyleSheet("#RightWidget{background-color:rgb(255,255,255)} \
								QWidget{font-size:15px;}");
	right_widget->setLayout(right_widget_layout);
	layout->addWidget(right_widget);

	//右边标题
	QLabel* title_label = new QLabel("选择联系人");
	title_label->setFixedHeight(40);
	title_label->setAlignment(Qt::AlignVCenter);
	right_widget_layout->addWidget(title_label, 0, 0, 1, 10);

	//选中好友widget
	right_widget_scroll = new QWidget();
	right_widget_scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	right_widget_scroll->setStyleSheet("QWidget{background-color:rgb(255,255,255)}");

	//选中好友weidget---layout
	QVBoxLayout* right_widget_scroll_layout = new QVBoxLayout();
	right_widget_scroll_layout->setSpacing(0);
	right_widget_scroll_layout->setContentsMargins(0, 0, 0, 0);
	right_widget_scroll_layout->setAlignment(Qt::AlignTop | Qt::AlignCenter);
	right_widget_scroll->setLayout(right_widget_scroll_layout);

	//选中itemscrollarea----右
	QScrollArea* area = new QScrollArea();
	area->setWidget(right_widget_scroll);
	area->setWidgetResizable(true);
	area->horizontalScrollBar()->setStyleSheet("QScrollBar:horizontal{height:0px;border:none}");
	area->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{width:2px;background-color:rgb(199, 198, 198)}");
	right_widget_layout->addWidget(area, 1, 0, 1, 10);

	//确认按钮----右
	QPushButton* OK_button = new QPushButton("完成");
	OK_button->setFixedSize(110, 30);
	OK_button->setStyleSheet("QPushButton{background-color:rgb(235, 235, 235);color:rgb(161, 161, 161);border:none;border-radius:5px;}");
	right_widget_layout->addWidget(OK_button, 2, 1, 1, 3);
	connect(OK_button, &QPushButton::clicked, this, &ChooseFriendDialog::OK_Slot);

	//取消按钮----右
	QPushButton* cannot_button = new QPushButton("取消");
	cannot_button->setFixedSize(110, 30);
	cannot_button->setStyleSheet("QPushButton{background-color:rgb(243, 243, 243);color:rgb(15, 194, 101);border:none;border-radius:5px;}");
	right_widget_layout->addWidget(cannot_button, 2, 5, 1, 3);
	connect(cannot_button, &QPushButton::clicked, this, &ChooseFriendDialog::Cannot_Slot);


 
}

/********ChooseFriendDialog****END****ChooseFriendDialog*****END******ChooseFriendDialog************/


/********ChooseFriendItem************ChooseFriendItem*************ChooseFriendItem*******************/

// item
ChooseFriendItem::ChooseFriendItem(ChooseFriendDialog* owner,const QString& userId, const QIcon& avatar, const QString& name,bool checked)
	:owner(owner),userId(userId),avatar(avatar),name(name),isHover(false)
{
	this->setFixedSize(248, 45);
	QHBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignLeft | Qt::AlignHCenter);
	this->setLayout(layout);

	checkBox = new QCheckBox();
	checkBox->setFixedSize(25,25);
	checkBox->setChecked(checked);
	checkBox->setStyleSheet("QCheckBox::indicator{width:25px;height:25px;image:url(:/resource/images/unchecked.png)} \
							QCheckBox::indicator:checked{width:25px;height:25px;image:url(:/resource/images/checked.png)}");
	connect(checkBox, &QCheckBox::toggled, this,&ChooseFriendItem::toggleCheckBox);
	layout->addWidget(checkBox);

	QPushButton* avatar_button = new QPushButton();
	avatar_button->setFixedSize(40,45);
	avatar_button->setIconSize(QSize(40,40));
	avatar_button->setIcon(avatar);
	avatar_button->setStyleSheet("border:none;border-radius:5px");
	layout->addWidget(avatar_button);

	QLabel* name_label = new QLabel();
	QFont font("宋体");
	font.setPixelSize(17);
	QFontMetrics metrice(font);
	name_label->setFont(font);
	name_label->setText(metrice.elidedText(name, Qt::ElideRight, 135));
	layout->addWidget(name_label);


}

ChooseFriendItem::~ChooseFriendItem()
{
}

void ChooseFriendItem::enterEvent(QEnterEvent* event)
{
	QWidget::enterEvent(event);
	isHover = true;
	repaint();
}

void ChooseFriendItem::leaveEvent(QEvent* event)
{
	QWidget::leaveEvent(event);
	isHover = false;
	repaint();
}

void ChooseFriendItem::paintEvent(QPaintEvent* event)
{
	// 修改颜色
	QWidget::paintEvent(event);
	QPainter painter(this);
	if (isHover) {
		painter.fillRect(rect(), QColor(231, 231, 231));
	}
	else {
		painter.fillRect(rect(), QColor(255, 255, 255));
	}

}

// 使得点击非checked的地方，同样能使得check选中和取消
void ChooseFriendItem::mouseReleaseEvent(QMouseEvent* event)
{
	QWidget::mouseReleaseEvent(event);
	checkBox->setChecked(!checkBox->isChecked());
}

void ChooseFriendItem::toggleCheckBox(bool checked)
{
	if (checked) {
		owner->addSelectedFriend(userId,avatar,name);
	}
	else {
		owner->deleteSelectedFriend(userId);
	}
}
