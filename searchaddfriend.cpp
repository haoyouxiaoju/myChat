#include "searchaddfriend.h"


searchAddFriend::searchAddFriend(QList<model::UserInfo>* user_list, QWidget* parent)
	:QDialog(parent)
{
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);

	LOG() << "searchAddFriend";
	this->setFixedSize(210, 300);
	layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(layout);
	searchLabel = new QLabel("搜索结果");
	searchLabel->setFixedHeight(30);
	searchLabel->setAlignment(Qt::AlignCenter);
	layout->addWidget(searchLabel);
	friendArea = new SessionFriendArea();
	layout->addWidget(friendArea);
	for (auto e : *user_list) {
		LOG() << "userId:" << e.userId << ",phone:" << e.phone;
	}
	for (const auto& e : *user_list) {

		friendArea->addItem(ItemType::SEARCHADDFRIENDITEM, e);
	}
}


searchAddFriend::~searchAddFriend()
{
}
