#include "datacenter.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QStandardPaths>



namespace model {
	DataCenter::DataCenter()
		:netClient(this)
	{

		//chatSessionList = new QList<ChatSessionInfo>();
		//applyList = new QList<UserInfo>();
		searchUserResult = new QList<UserInfo>();
		searchMessageResult = new QList<Message>();
		recentMessages = new QHash<QString, QList<Message>>();
		unreadMessageCount = new QHash<QString, int>();


	}

	DataCenter::~DataCenter()
	{
		//需要delete前面new出来的QList和QHash
		//
		delete myself;//随退出而删除
		// ...
		// detodo

	}
	DataCenter* DataCenter::getInstance()
	{
		static DataCenter instance;

		return &instance;
	}


	void DataCenter::initDataFile()
	{
		QString f_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "ChatClient.json";
		
		QFile file(f_path);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qCritical() << POSITION << "Cannot open file:" << file.errorString() <<",file path:" << f_path;
			return;
		}

		//写入初始内容
		QString data("{\n\n}");
		file.write(data.toUtf8());
		file.close();


	}

	void DataCenter::saveDataFile()
	{
		QString f_path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/ChatClient.json";

		QFile file(f_path);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			qCritical() << POSITION << "Cannot open file:" << file.errorString() <<",file path:" << f_path;
			return;
		}

		QJsonObject jsonObject;
		jsonObject["loginSessionId"] = loginSessionId;

		QJsonObject jsonUnread;
		for (auto it = unreadMessageCount->begin(); it != unreadMessageCount->end(); ++it) {
			jsonUnread[it.key()] = it.value();
		}
		jsonObject["unread"] = jsonUnread;

		QJsonDocument jsonDoc(jsonObject);

		QString s = jsonDoc.toJson();
		file.write(s.toUtf8());

		file.close();

	}

	void DataCenter::loadDataFile()
	{
		//  appData 目录
		QString bashPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

		QString f_path = bashPath + "/ChatClient.json";

		QDir dir;
		if (!dir.exists(bashPath)) {//文件夹不存在则创建
			dir.mkpath(bashPath);
		}

		QFileInfo fileInfo(f_path);
		if (!fileInfo.exists()) {//文件不存在则创建
			initDataFile();
		}

		QFile file(f_path);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qCritical() << POSITION << "Cannot open file:" << file.errorString() <<",file path:" << f_path;
			return;
		}

		//获取 基础内容 json文件
		QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
		if (jsonDoc.isNull()) {
			qCritical() << POSITION << "Invalid JSON format";
		}

		QJsonObject jObject = jsonDoc.object();

		loginSessionId = jObject["loginSessionId"].toString();

		this->unreadMessageCount->clear();
		QJsonObject unread = jObject["unread"].toObject();
		for (auto i = unread.begin(); i != unread.end(); ++i) {
			(*this->unreadMessageCount)[i.key()] = i.value().toInt();
		}

		//检查前面读到的内容是否正确
		if (loginSessionId == "") {
			qCritical() << POSITION << "SessionId is null";
			return;
		}


		file.close();

			

	}

	ChatSessionInfo* DataCenter::findChatSessionByUserId(const QString& userId)
	{
		if (this->chatSessionList == nullptr) {
			return nullptr;
		}
		for (auto& e : *chatSessionList) {
			if (e.userId == userId) {
				return &e;
			}
		}

		return nullptr;

	}

	void DataCenter::topChatSessionInfo(const QString& chat_session_id)
	{
		if (this->chatSessionList == nullptr) {
			return;
		}
		auto e = this->chatSessionList->begin();
		for (; e != this->chatSessionList->end(); ++e) {
			if ((*e).chatSessionId == chat_session_id) {
				break;
			}
		}
		const ChatSessionInfo info = *e;
		this->chatSessionList->erase(e);
		this->chatSessionList->push_front(info);

	}

	QString DataCenter::getLoginSessionId()
	{
		return this->loginSessionId;
	}


	void DataCenter::resetMySelf(std::shared_ptr<chat_im::GetUserInfoRsp> myself)
	{
		if (this->myself == nullptr) {
			this->myself = new UserInfo();
		}

		this->myself->load(myself->userInfo());
	}

	void DataCenter::getMySelfAsync()
	{
		netClient.getMySelf(loginSessionId);
	}

	UserInfo* DataCenter::getMySelf()
	{
		return this->myself;
	}

	void DataCenter::resetFriendList(std::shared_ptr<chat_im::GetFriendListRsp> friend_list)
	{
		if (this->friendList == nullptr) {
			this->friendList = new QList<UserInfo>();
		}


		//清空
		this->friendList->clear();

		for (const auto& e : friend_list->friendList()) {
			// chat_im::userInfo => model::userInfo
			UserInfo u_info;
			u_info.load(e);
			
			this->friendList->push_back(u_info);

		}
	}

	void DataCenter::getFriendListAsync()
	{
		netClient.getFriendList(loginSessionId);
	}

	QList<UserInfo>* DataCenter::getFriendList()
	{
		return this->friendList;
	}

	void DataCenter::resetChatSessionList(std::shared_ptr<chat_im::GetChatSessionListRsp> session_list)
	{
		if (this->chatSessionList == nullptr) {
			this->chatSessionList = new QList<model::ChatSessionInfo>();
		}

		this->chatSessionList->clear();

		for (const auto& e : session_list->chatSessionInfoList()) {
			model::ChatSessionInfo info;
			info.load(e);
			this->chatSessionList->push_back(info);
		}

	}

	void DataCenter::getChatSessionListAsync()
	{
		netClient.getChatSessionList(loginSessionId);
	}

	QList<ChatSessionInfo>* DataCenter::getChatSessionList()
	{
		return this->chatSessionList;
	}



	void DataCenter::resetApplyList(std::shared_ptr<chat_im::GetPendingFriendEventListRsp> apply_list)
	{
		if (this->applyList == nullptr) {
			this->applyList = new QList<UserInfo>();
		}
		this->applyList->clear();
		for (const auto& e : apply_list->event()) {
			UserInfo info;
			info.load(e.sender());
			this->applyList->push_back(info);
		}

	}

	void DataCenter::getApplyListAsync()
	{
		netClient.getApplyList(loginSessionId);
	}

	QList<UserInfo>* DataCenter::getApplyList()
	{
		return this->applyList;
	}

	ChatSessionInfo* DataCenter::getChatSessionInfo(const QString& chat_session_id)
	{
		if (this->chatSessionList == nullptr)
			return nullptr;
		for (auto& e : *this->chatSessionList) {
			if (e.chatSessionId == chat_session_id) {
				return &e;
			}
		}
		return nullptr;
	}

	const QString& DataCenter::getCurrentChatSessionId()
	{
		return currentChatSessionId;
	}

	void DataCenter::setCurrentChatSessionId(const QString& chat_session_id)
	{
		this->currentChatSessionId = chat_session_id;
	}

	void DataCenter::resetRecentMessage(const QString& chat_session_id ,std::shared_ptr<chat_im::GetRecentMsgRsp> recentMessage_list)
	{
		QList<Message>& message_list = (*recentMessages)[chat_session_id];
		message_list.clear();

		for (const auto& e : recentMessage_list->messageList()) {
			Message message;
			message.load(e);
			message_list.push_back(message);
		}

	}

	void DataCenter::getRecentMessageAsync(const QString& chat_session_id,bool updataUi)
	{
		netClient.getRecentMessage(loginSessionId,chat_session_id,updataUi);
	}

	QList<model::Message>* DataCenter::getRecentMessageList(const QString& chat_session_id)
	{
		if (recentMessages->contains(chat_session_id) == false) {
			return nullptr;
		}
		return &(*recentMessages)[chat_session_id];
	}

}
