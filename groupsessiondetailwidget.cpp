#include "groupsessiondetailwidget.h"
#include "debug.h"

GroupSessionDetailWidget::GroupSessionDetailWidget(const model::ChatSessionInfo& sessionInfo, QWidget* parent)
	: QDialog(parent), sessionInfo(sessionInfo)
{
	int thisHeight;
	if (parent != nullptr) {
		thisHeight = parent->height();
	}
	else {
		thisHeight = 654;
	}
	//widget 的大小设置
	//
	this->setFixedHeight(thisHeight);
	this->setFixedWidth(0);

	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setStyleSheet("QWidget{border-bottom:2px solid rgb(238, 238, 238)} \
							QLabel{ font-size:17px bold rgb(0,0,0) }");

	//主布局设置
	layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 15, 0, 15);
	layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	this->setLayout(layout);
	
	//会话成员展示
	QWidget* avatarTabel_widget = new QWidget();
	//avatarTabel_widget->setMaximumHeight(320);
	avatarTabel_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	avatarTabel_Layout = new QGridLayout();
	avatarTabel_Layout->setSpacing(5);
	avatarTabel_Layout->setContentsMargins(10, 0, 10, 0);
	avatarTabel_Layout->setAlignment(Qt::AlignLeft| Qt::AlignTop);
	avatarTabel_widget->setLayout(avatarTabel_Layout);

	layout->addWidget(avatarTabel_widget);

	//历史记录按钮
	QWidget* messageHistory_Widget = new QWidget();
	messageHistory_Widget->setFixedHeight(45);
	messageHistory_Widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QHBoxLayout* messageHistory_layout = new QHBoxLayout();
	messageHistory_layout->setSpacing(0);
	messageHistory_layout->setContentsMargins(10, 0, 0, 10);
	messageHistory_Widget->setLayout(messageHistory_layout);

	QLabel* messageHistory_label = new QLabel("聊天记录");
	messageHistory_label->setFixedHeight(45);
	messageHistory_layout->addWidget(messageHistory_label, 0, Qt::AlignLeft|Qt::AlignHCenter);
	
	QPushButton* messageHistory_button = new QPushButton(QIcon(":/resource/images/right_arrow.png"),"");
	messageHistory_button->setFixedSize(QSize(35, 45));
	messageHistory_button->setIconSize(QSize(20, 20));
	messageHistory_button->setStyleSheet("QPushButton{border:none;background-color:transparent;}");
	messageHistory_layout->addWidget(messageHistory_button, 0, Qt::AlignRight);
	
	layout->addWidget(messageHistory_Widget);
	
	//其他功能按键
	QWidget* otherOption_Widget = new QWidget();
	otherOption_Widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	QGridLayout* otherOption_layout = new QGridLayout();
	otherOption_layout->setSpacing(0);
	otherOption_layout->setContentsMargins(15,15, 15, 10);
	otherOption_layout->setAlignment(Qt::AlignTop );
	otherOption_Widget->setLayout(otherOption_layout);
	
	QLabel* label1 = new QLabel("消息免打扰");
	label1->setFixedHeight(35);
	QLabel* label2 = new QLabel("置顶聊天");
	label2->setFixedHeight(35);

	otherOption_layout->addWidget(label1, 0, 0, Qt::AlignLeft);
	otherOption_layout->addWidget(label2, 1, 0, Qt::AlignLeft);

	layout->addWidget(otherOption_Widget);

	QPushButton* clearHistoryButton = new QPushButton("清空聊天记录");
	clearHistoryButton->setFixedHeight(45);
	clearHistoryButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	clearHistoryButton->setStyleSheet("QPushButton{color:red;font-size:15px;border:none;}");
	layout->addWidget(clearHistoryButton, Qt::AlignCenter | Qt::AlignTop);
	
	QPushButton* deleteFriendButton = new QPushButton("删除好友");
	deleteFriendButton->setFixedHeight(45);
	deleteFriendButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	deleteFriendButton->setStyleSheet("QPushButton{color:red;font-size:15px;border:none;}");
	layout->addWidget(deleteFriendButton, Qt::AlignCenter | Qt::AlignTop);

#if TEXT_UI
	addAvatarItem(QIcon(":/resource/images/xiaoju.jpg"), "haoyouxiaoju");
	addAvatarItem(QIcon(":/resource/images/plus_sign.png"), "添加");
	addAvatarItem(QIcon(":/resource/images/lishijilu.png"), "添加");
	addAvatarItem(QIcon(":/resource/images/tupian.png"), "添加");
	addAvatarItem(QIcon(":/resource/images/more.png"), "添加");
	AvatarItem* item =new AvatarItem(QIcon(":/resource/images/more.png"), "添加");
	addAvatarItem(item);
	connect(item->avatar_button, &QPushButton::clicked, this, []() {
		ChooseFriendDialog* dialog = new ChooseFriendDialog("haoyouxiaoju");
		dialog->exec();
		
		});
#endif

}



GroupSessionDetailWidget::~GroupSessionDetailWidget()
{}

void GroupSessionDetailWidget::addAvatarItem(const QIcon& icon, const QString& name)
{
	if (cur_Col >= Max_Col) {
		cur_Col = 0;
		cur_Row++;
	}
	avatarTabel_Layout->addWidget(new AvatarItem(icon, name,this),cur_Row,cur_Col++);
}

void GroupSessionDetailWidget::addAvatarItem(AvatarItem* item)
{
	if (cur_Col >= Max_Col) {
		cur_Col = 0;
		cur_Row++;
	}
	avatarTabel_Layout->addWidget(item,cur_Row,cur_Col++);
}

void GroupSessionDetailWidget::show_widget()
{
	QVariantAnimation* animation = new QVariantAnimation(this);
	animation->setDuration(300); // 
	animation->setEasingCurve(QEasingCurve::InOutCirc);
	animation->setStartValue(0);
	animation->setEndValue(270);
	connect(animation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
		this->setFixedWidth(value.toReal()); // 根据新的宽度调整大小  
		});
	this->show();
	animation->start();
	

}
