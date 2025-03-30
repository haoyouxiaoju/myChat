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
		myself = new UserInfo();

		
		//提前初始化netClient 还未登录 loginid是不存在的所以不能在此处直接初始化websocket;
		//netClient.initWebSocket();

	//保存头像,由于每次接受消息都会接受到一个头像数据,但头像数据大部分情况下都已出现过,
	//又因重新接受导致新icon出现,从而内存占比大幅增加
	//所以考虑用map来处理,减少icon的内存消耗
		__avatar = new QMap<QByteArray, QIcon>();


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

	void DataCenter::initWebsocker()
	{
		netClient.initWebSocket();
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

	const QIcon& DataCenter::getIcon(const QByteArray& data)
	{
		auto item = __avatar->find(data);
		if (item != __avatar->end()) {
			return item.value();
		}
		else {
		//此数据还有有创建icon,则创建icon并填入
			(*__avatar)[data] = makeQIcon(data);
			return (*__avatar)[data];
		}
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

	void DataCenter::topChatSessionInfo(const ChatSessionInfo& chat_session_info)
	{
		if (this->chatSessionList == nullptr) {
			return;
		}
		auto e = this->chatSessionList->cbegin();
		for (; e != this->chatSessionList->cend(); ++e) {
			if ((*e).chatSessionId == chat_session_info.chatSessionId) {
				break;
			}
		}
		if (e == this->chatSessionList->cend()) {
			return;
		}

		ChatSessionInfo info = chat_session_info;

		this->chatSessionList->erase(e);
		this->chatSessionList->push_front(info);
		

	}

	const QString& DataCenter::getLoginSessionId()
	{
		return this->loginSessionId;
	}

	const QString& DataCenter::getVerifyCodeId()
	{
		return this->currentVerifyCodeId;
	}

	UserInfo* DataCenter::getFriendById(const QString& user_id)
	{
		//如果friendList为空,则直接返回
		if (this->friendList == nullptr) {
			this->getFriendListAsync();
			return nullptr;
		}
		//遍历查找
		for (auto& i : *this->friendList) {
			if (i.userId == user_id) {
				return &i;
			}
		}
		return nullptr;

	}

	UserInfo DataCenter::removeFromApplyList(const QString& user_id)
	{
		if (this->applyList == nullptr) {
			return UserInfo();
		}
		for (int i = 0; i < this->applyList->size(); ++i) {
			if (this->applyList->at(i).userId == user_id) {
				UserInfo info =this->applyList->at(i);
				this->applyList->removeAt(i);
				return info;
			}
		}
		return UserInfo();
	}

	ChatSessionInfo* DataCenter::findChatSessionBySessionId(const QString& chat_session_id)
	{
		//如果chatSessionList为空,则直接返回
		if (chatSessionList == nullptr) {
			this->getChatSessionListAsync();
			return nullptr;
		}
		//遍历查找
		for (auto i = chatSessionList->begin(); i != chatSessionList->end(); ++i) {
			if ((*i).chatSessionId == chat_session_id) {
				return &(*i);
			}
		}
		return nullptr;
	
	}

	QList<UserInfo>* DataCenter::getMemberListBySessionId(const QString& chat_session_id)
	{
		QList<UserInfo>& list  =(*this->memberList)[chat_session_id];
		if (memberList == nullptr || list.isEmpty()) {
			return nullptr;;
		}
		return &list;
	}

	QList<UserInfo>* DataCenter::getSearchUserResult()
	{
		return this->searchUserResult;
	}

	QList<Message>* DataCenter::getSearchMessageResult()
	{
		return this->searchMessageResult;
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

	void DataCenter::sendTextMessageAsync(const QString& chat_session_id, const QString& body)
	{
		netClient.sendMessage(loginSessionId, chat_session_id, model::MessageType::TEXT_TYPE, body.toUtf8());
	}

	const Message& DataCenter::addMessage(const Message& message)
	{
		const QString& chatSessionId = message.chatiSessionId;
		(*recentMessages)[chatSessionId].push_back(message);
		return (*recentMessages)[chatSessionId].back();
	}

	void DataCenter::clearUnread(const QString& chatSessionId)
	{
		(*unreadMessageCount)[chatSessionId] = 0;
		//
		saveDataFile();
	}

	void DataCenter::addUnread(const QString& chatSessionId)
	{
		++(*unreadMessageCount)[chatSessionId];
		//
		saveDataFile();
	}

	int DataCenter::getUnread(const QString& chatSessionId)
	{

		return (*unreadMessageCount)[chatSessionId];
	}

	void DataCenter::receiveMessage(const QString& chatSessionId)
	{
		//如果收到的消息所属会话不是当前所选中的消息会话,则不展示处理
		if (chatSessionId != currentChatSessionId) {
			addUnread(chatSessionId);
		}
		else {
			//展示
			//获取最后一天消息
			const Message& lastMessage = (*recentMessages)[chatSessionId].back();
			//通知 界面添加一条新消息
			emit this->receiveMessageDone(lastMessage);
		}
		//再发送信号,更新会话列表中的显示lastMessage
		emit this->updateLastMessage(chatSessionId);
	}

	void DataCenter::changeNickNameAsync(const QString& nickName)
	{
		netClient.changeNickName(loginSessionId,nickName);
	}

	void DataCenter::changeDescriptionAsync(const QString& newDesc)
	{
		netClient.changeDescription(loginSessionId, newDesc);
	}

	void DataCenter::changePhoneAsync(const QString& newPhone, const QString& verifyCodeId, const QString& verifyCode)
	{
		netClient.changePhone(loginSessionId, newPhone,verifyCodeId,verifyCodeId);
	}

	void DataCenter::getVerifyCodeAsync(const QString& phone)
	{
		netClient.getVerifyCode(phone);
	}

	void DataCenter::changeAvatarAsync(const QByteArray& body)
	{
		netClient.changeAvatar(loginSessionId,body);
	}

	void DataCenter::deleteFriendAsync(const QString& userId)
	{
		netClient.deleteFriend(loginSessionId,userId);
	}

	void DataCenter::addFriendApplyAsync(const QString& userId)
	{
		netClient.addFriendApply(loginSessionId, userId);
	}

	void DataCenter::acceptFriendApplyAsync(const QString& userId)
	{
		netClient.acceptFriendApply(loginSessionId, userId);
	}

	void DataCenter::rejectFriendApplyAsync(const QString& userId)
	{
		netClient.rejectFriendApply(loginSessionId, userId);
	}

	void DataCenter::createGroupChatSessionAsync(const QList<QString>& memberList)
	{
		netClient.createGroupChatSession(loginSessionId,memberList);
	}

	void DataCenter::getChatSessionMemberAsync(const QString& chat_session_id)
	{
		netClient.getChatSessionMember(loginSessionId, chat_session_id);
	}

	void DataCenter::searchAddFriendAsync(const QString& keyWord)
	{
		netClient.searchAddFriend(loginSessionId, keyWord);
	}

	void DataCenter::searchMessageAsync(const QString& keyWord)
	{
		netClient.searchMessage(loginSessionId,currentChatSessionId, keyWord);
	}

	void DataCenter::loginAsync(const QString& username, const QString& password)
	{
		netClient.userLogin(username, password);
	}

	void DataCenter::phoneLoginAsync(const QString& phone, const QString& verifycode)
	{
		netClient.phoneLogin(phone, verifycode,this->getVerifyCodeId());
	}

	void DataCenter::phoneRegisterAsync(const QString& phone, const QString& password, const QString& verifycode)
	{
		netClient.phoneRegister(phone, password, verifycode, this->getVerifyCodeId());
	}

	void DataCenter::getSingleFileAsync(const QString& fileId)
	{
		netClient.getSingleFile(loginSessionId,fileId);
	}

	void DataCenter::sendImageMessageAsync(const QString& chat_session_id, const QByteArray& body)
	{
		netClient.sendMessage(loginSessionId, chat_session_id, model::MessageType::IMAGE_TYPE, body);
	}

	void DataCenter::sendFileMessageAsync(const QString& chat_session_id, const QByteArray& body, const QString& fileName)
	{
		netClient.sendMessage(loginSessionId, chat_session_id,model::MessageType::FILE_TYPE, body, fileName);
	}

	void DataCenter::sendSpeechMessageAsync(const QString& chat_session_id, const QByteArray& content)
	{
		netClient.sendMessage(loginSessionId, chat_session_id, model::MessageType::SPEECH_TYPE,content);
	}

	void DataCenter::speechRecognitionAsync(const QString& fileId, const QByteArray& content)
	{
		netClient.speechRecognition(loginSessionId, fileId, content);
	}

	

	void DataCenter::resetNickName(const QString& nickName)
	{
		this->myself->nickname = nickName;
	}

	void DataCenter::resetDescription(const QString& description)
	{
		this->myself->description = description;
	}
	void DataCenter::resetPhone(const QString& phone) {
		this->myself->phone = phone;
	}

	void DataCenter::resetVerifyCodeId(const QString& id)
	{
		this->currentVerifyCodeId = id;
	}

	void DataCenter::resetAvatar(const QByteArray& body)
	{
		this->myself->avatar = body;
	}

	void DataCenter::removeFriend(const QString& id)
	{
		if (friendList == nullptr) {
			this->getFriendListAsync();
			return;
		}
		
		friendList->removeIf([id](const UserInfo& userInfo) {
			return userInfo.userId == id;
			});

		chatSessionList->removeIf([id,this](const ChatSessionInfo& info) {
			if (info.userId != id || info.userId == "") {
				return false;
			}
			if (info.chatSessionId == this->currentChatSessionId) {
				emit this->clearCurrentSession();
			}
			return true;


			});
	}

	void DataCenter::resetChatSessionMember(const QString& chat_session_id, std::shared_ptr<chat_im::GetChatSessionMemberRsp> member_list)
	{
		if (this->memberList == nullptr) {
			this->memberList = new QHash<QString, QList<UserInfo>>();
		}
		QList<UserInfo>& list = (*this->memberList)[chat_session_id];
		list.clear();
		for (const auto& e : member_list->memberInfoList()) {
			UserInfo info;
			info.load(e);
			list.push_back(info);
		}
	}

	void DataCenter::resetSearchAddFriendList(std::shared_ptr<chat_im::FriendSearchRsp> rsp)
	{
		if (this->searchUserResult == nullptr) {
			this->searchUserResult = new QList<UserInfo>();
		}
		this->searchUserResult->clear();
		for (const auto& e : rsp->userInfo()) {
			UserInfo info;
			info.load(e);
			this->searchUserResult->push_back(info);
		}
	}

	void DataCenter::resetSearchMessageList(const QList<chat_im::MessageInfo>& messageList)
	{
		if (this->searchMessageResult == nullptr) {
			this->searchMessageResult = new QList<Message>();
		}
		LOG() << "this->searchMessageResult.size() = " << this->searchMessageResult->size();
		this->searchMessageResult->clear();
		LOG() << "this->searchMessageResult.size() = " << this->searchMessageResult->size();
		for (const auto& e : messageList) {
			Message message;
			message.load(e);
			this->searchMessageResult->push_back(message);
		}
	}

	void DataCenter::setLoginSessionId(const QString& loginSessionId)
	{
		this->loginSessionId = loginSessionId;
		saveDataFile();
	}

}
