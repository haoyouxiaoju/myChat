#include "netclient.h"
#include <QNetworkReply>
#include <QUuid>
#include <QNetWorkRequest>
#include "../model/datacenter.h"
#include "../toast.h"

#include "webSocket.qpb.h"
#include "user.qpb.h"
#include "message.qpb.h"
#include "transmit.qpb.h"
#include "friend.qpb.h"
#include "notify.qpb.h"
#include "file.qpb.h"
#include "speech.qpb.h"

using namespace model;

NetClient::NetClient(model::DataCenter* dataCenter)
	:dataCenter(dataCenter)
{
}

NetClient::~NetClient()
{
}

//
// 测试网络是否联通
void NetClient::ping()
{
	QNetworkRequest httpReq;
	httpReq.setUrl(QUrl(HTTP_URL + "/ping"));

	QNetworkReply* httpRsp = httpClient.get(httpReq);
	connect(httpRsp, &QNetworkReply::finished, this, [httpRsp]() {
		if (httpRsp->error() != QNetworkReply::NoError) {
			LOG() << "http request is fail:" << httpRsp->errorString();
			return;
		}
		//响应内容
		QByteArray res = httpRsp->readAll();
		LOG() << "res = " << QString(res);
		httpRsp->deleteLater();

		});
}

// 异步获取用户自身信息
void NetClient::getMySelf(const QString& login_session_id)
{
	chat_im::GetUserInfoReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(login_session_id);

	QByteArray body = req.serialize(&this->serializer);
	QNetworkReply* reply = this->sendHttpRequest("/service/user/get_user_info",body);

	connect(reply, &QNetworkReply::finished, this, [reply,this]() {
		std::shared_ptr<chat_im::GetUserInfoRsp> user_info = this->handleHttpResponse<chat_im::GetUserInfoRsp>(reply);

		if (!user_info || !user_info->success()) {
			
			return;
		}
		dataCenter->resetMySelf(user_info);
		emit dataCenter->getMySelfDone();


		});


}

// 异步获取用户的好友列表
void NetClient::getFriendList(const QString& login_session_id)
{
	chat_im::GetFriendListReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(login_session_id);

	QByteArray body = req.serialize(&this->serializer);
	QNetworkReply* reply = this->sendHttpRequest("/service/friend/get_friend_list", body);

	connect(reply, &QNetworkReply::finished, this, [this,reply]() {
		std::shared_ptr<chat_im::GetFriendListRsp> friend_list = this->handleHttpResponse<chat_im::GetFriendListRsp>(reply);

		if (!friend_list || !friend_list->success()) {
			return;
		}

		dataCenter->resetFriendList(friend_list);
		emit dataCenter->getFriendListDone();

		});

}

void NetClient::getChatSessionList(const QString& login_session_id)
{
	chat_im::GetChatSessionListReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(login_session_id);

	QByteArray body = req.serialize(&this->serializer);
	QNetworkReply* reply = this->sendHttpRequest("/service/friend/get_chat_session_list", body);
	
	connect(reply, &QNetworkReply::finished, this, [this, reply]() {
		std::shared_ptr<chat_im::GetChatSessionListRsp> session_list = this->handleHttpResponse<chat_im::GetChatSessionListRsp>(reply);

		if (!session_list || ! session_list->success()) {
			return;
		}
		dataCenter->resetChatSessionList(session_list);
		emit dataCenter->getChatSessionListDone();
		});

}

void NetClient::getApplyList(const QString& login_session_id)
{
	chat_im::GetPendingFriendEventListReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(login_session_id);

	QByteArray body = req.serialize(&this->serializer);
	QNetworkReply* reply = this->sendHttpRequest("/service/friend/get_pending_friend_events", body);

	connect(reply, &QNetworkReply::finished, this, [this, reply]() {
		std::shared_ptr<chat_im::GetPendingFriendEventListRsp> apply_list = this->handleHttpResponse<chat_im::GetPendingFriendEventListRsp>(reply);

		if (!apply_list||!apply_list->success()) {
			return;
		}
		dataCenter->resetApplyList(apply_list);
		emit dataCenter->getApplyListDone();

		});
	
}

void NetClient::getRecentMessage(const QString& login_session_id, const QString& chat_session_id,bool updataUi)
{
	chat_im::GetRecentMsgReq req;
	req.setRequestId(makeRequestId());
	req.setChatSessionId(chat_session_id);
	req.setSessionId(login_session_id);
	req.setMsgCount(20);

	QByteArray data = req.serialize(&this->serializer);
	QNetworkReply* reply = sendHttpRequest("/service/message_storage/get_recent", data);
	connect(reply, &QNetworkReply::finished, this, [this,reply,chat_session_id,updataUi]() {
		std::shared_ptr<chat_im::GetRecentMsgRsp> recent_message = this->handleHttpResponse<chat_im::GetRecentMsgRsp>(reply);

		if (!recent_message|| !recent_message->success()) {
			return;
		}
		dataCenter->resetRecentMessage(chat_session_id,recent_message);
		if (updataUi) {
			emit dataCenter->getRecentMessageDone(chat_session_id);
		}
		else {
			emit dataCenter->getRecentMessageDoneNotUi(chat_session_id);
		}
		});
}

void NetClient::sendMessage(const QString& loginSessionId, const QString& chatSessionId, model::MessageType type, const QByteArray& body, const QString& extraInfo)
{
	chat_im::NewMessageReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setChatSessionId(chatSessionId);

	chat_im::MessageContent content;
	switch (type)
	{
	case MessageType::TEXT_TYPE: {
		content.setMessageType(chat_im::MessageTypeGadget::MessageType::STRING);
		chat_im::StringMessageInfo info;
		info.setContent(body);
		content.setStringMessage(info);
		break;
	}
	case MessageType::IMAGE_TYPE: {
		content.setMessageType(chat_im::MessageTypeGadget::MessageType::IMAGE);
		chat_im::ImageMessageInfo info;
		info.setFileId("");
		info.setImageContent(body);
		content.setImageMessage(info);
		break;
	}
	case MessageType::FILE_TYPE:{
		content.setMessageType(chat_im::MessageTypeGadget::MessageType::FILE);
		chat_im::FileMessageInfo info;
		info.setFileId("");
		info.setFileName(extraInfo);
		info.setFileSize(body.size());
		info.setFileContents(body);
		content.setFileMessage(info);
		break;
	}
	case MessageType::SPEECH_TYPE: {
		content.setMessageType(chat_im::MessageTypeGadget::MessageType::SPEECH);
		chat_im::SpeechMessageInfo info;
		info.setFileId("");
		info.setFileContents(body);
		content.setSpeechMessage(info);
		break;
	}
	default:
		LOG() << "未知的messageType:" << type;
		return;
	}
	req.setMessage(content);
	QByteArray req_body = req.serialize(&serializer);
	LOG() << "[发送消息] requestId=" << req.requestId() << ", sessionId=" <<	loginSessionId << ", chatSessionId=" << chatSessionId;
	
	QNetworkReply* httpRsp = this->sendHttpRequest("/service/message_transmit/new_message", req_body);

	connect(httpRsp, &QNetworkReply::finished, this, [this,httpRsp,type,body,extraInfo]() {
		std::shared_ptr<chat_im::NewMessageRsp> rsp = this->handleHttpResponse<chat_im::NewMessageRsp>(httpRsp);
		if (!rsp) {
			LOG() << "发送消息失败";
			return;
		}
		if (!rsp->success()) {
			LOG() << "发送消息失败:" << rsp->errmsg();
			emit dataCenter->sendMessageFailed(rsp->errmsg());
			return;
		}
		emit dataCenter->sendMessageDone(type, body, extraInfo);
		});

}

void NetClient::changeNickName(const QString& loginSessionId, const QString& nickName)
{
	chat_im::SetUserNicknameReq req;
	req.setSessionId(loginSessionId);
	req.setNickname(nickName);
	req.setRequestId(makeRequestId());
	//
	QByteArray body = req.serialize(&serializer);
	LOG() << "[修改用户昵称] requestId = " << req.requestId() << ",sessionId=" << loginSessionId << ", nickName=" << nickName;

	//发送请求
	QNetworkReply* httpResp = this->sendHttpRequest("/service/user/set_nickname", body);

	//处理
	connect(httpResp, &QNetworkReply::finished, this, [this,httpResp,nickName]() {
		//
		std::shared_ptr<chat_im::SetUserNicknameRsp> rsp = this->handleHttpResponse<chat_im::SetUserNicknameRsp>(httpResp);

		//
		if (!rsp || !rsp->success()) {
			LOG() << "修改用户昵称请求失败";
			return;
		}
		//修改数据
		dataCenter->resetNickName(nickName);

		emit dataCenter->changeNickNameDone();

		});

}

void NetClient::changeDescription(const QString& loginSessionID, const QString& newDesc)
{
	chat_im::SetUserDescriptionReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionID);
	req.setDescription(newDesc);

	QByteArray body = req.serialize(&serializer);
	LOG() << "[修改⽤⼾昵称] requestId=" << req.requestId() << ", sessionId=" <<loginSessionID << ", description=" << newDesc;

	// 2. 发送 http 请求
	QNetworkReply* httpResp = this -> sendHttpRequest("/service/user/set_description", body);
	connect(httpResp, &QNetworkReply::finished, this, [this,httpResp,newDesc]() {
		std::shared_ptr<chat_im::SetUserDescriptionRsp> rsp = this->handleHttpResponse<chat_im::SetUserDescriptionRsp>(httpResp);
		if (!rsp || !rsp->success() ) {
			return;
		}
		dataCenter->resetDescription(newDesc);
		emit dataCenter->changeDescriptionDone();
		});



}


void NetClient::changePhone(const QString& loginSessionId, const QString& newPhone, const QString& verifyCodeId , const QString& verifyCode)
{
	// 1. 构造请求
	chat_im::SetUserPhoneNumberReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setPhoneNumber(newPhone);
	req.setPhoneVerifyCodeId(verifyCodeId);
	req.setPhoneVerifyCode(verifyCode);
	QByteArray body = req.serialize(&serializer);
	LOG() << "[修改⼿机号] requestId=" << req.requestId() << " phoneNumber=" << newPhone << " verifyCodeId=" << verifyCodeId << " verifyCode=" << verifyCode;

	// 2. 发送 HTTP 请求
	QNetworkReply* httpResp = this->sendHttpRequest("/service/user/set_phone", body);

	// 3. 处理 HTTP 响应
	connect(httpResp, &QNetworkReply::finished, this, [this, httpResp, newPhone]() {
		// a) 解析响应
		std::shared_ptr<chat_im::SetUserPhoneNumberRsp> resp = this->handleHttpResponse<chat_im::SetUserPhoneNumberRsp>
			(httpResp);
		if (!resp || !resp->success()) {
			return;
		}
		dataCenter->resetPhone(newPhone);
		// c) 发送信号
		emit dataCenter->changeUserPhoneDone();
		});
}

void NetClient::getVerifyCode(const QString& phone)
{
	chat_im::PhoneVerifyCodeReq req;
	req.setRequestId(makeRequestId());
	req.setPhoneNumber(phone);
	QByteArray body = req.serialize(&serializer);
	LOG() << "[获取验证码] requestId=" << req.requestId() << " phoneNumber=" <<phone;

	// 2. 发送 HTTP 请求
	QNetworkReply* httpResp = this -> sendHttpRequest("/service/user/get_phone_verify_code", body);

	connect(httpResp, &QNetworkReply::finished, this, [this,httpResp,phone]() {
		std::shared_ptr<chat_im::PhoneVerifyCodeRsp> rsp = this->handleHttpResponse<chat_im::PhoneVerifyCodeRsp>(httpResp);
		if (!rsp || !rsp->success()) {
			return;
		}
		dataCenter->resetVerifyCodeId(rsp->verifyCodeId());
		emit dataCenter->getVerifyCodeDone();


		});
}

void NetClient::changeAvatar(const QString& loginSessionId,const QByteArray& body)
{
	chat_im::SetUserAvatarReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setAvatar(body);

	QByteArray request = req.serialize(&serializer);
	LOG() << "[修改头像] requestId=" << req.requestId() << " loginSessionId="<< loginSessionId;

	// 2. 发送 HTTP 请求
	QNetworkReply* httpResp = this -> sendHttpRequest("/service/user/set_avatar", request);
	connect(httpResp, &QNetworkReply::finished, this, [this, httpResp,body] {
		std::shared_ptr<chat_im::SetUserAvatarRsp> rsp = this->handleHttpResponse<chat_im::SetUserAvatarRsp>(httpResp);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "修改头像失败:" << rsp->errmsg();
			return;
		}
		//
		dataCenter->resetAvatar(body);
		emit dataCenter->changeAvatarDone();
		});

}

void NetClient::deleteFriend(const QString& loginSessionId, const QString& userId)
{
	chat_im::FriendRemoveReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setPeerId(userId);
	QByteArray body = req.serialize(&serializer);

	LOG() << "[删除好友] requestId=" << req.requestId() << " loginSessionId="<< loginSessionId << " userId=" << userId;

	// 2. 发送 HTTP 请求
	QNetworkReply* httpResp = this -> sendHttpRequest("/service/friend/remove_friend", body);
	connect(httpResp, &QNetworkReply::finished, this, [this, httpResp,userId]() {
		std::shared_ptr<chat_im::FriendRemoveRsp> rsp = this->handleHttpResponse<chat_im::FriendRemoveRsp>(httpResp);
		if (!rsp) {
			return;
		}
		//删除失败
		if (!rsp->success()) {
			return;
		}
		this->dataCenter->removeFriend(userId);
		emit dataCenter->deleteFriendDone(userId);
		});

}

void NetClient::addFriendApply(const QString& loginSessionId, const QString& userId)
{
	chat_im::FriendAddReq req; 
	req.setRequestId(makeRequestId());
	req.setRespondentId(userId);
	req.setSessionId(loginSessionId);
	QByteArray body = req.serialize(&serializer);
	LOG() << "[发送好友申请] requestId=" << req.requestId() << " loginSessionId=" << loginSessionId << " userId=" << userId;
	//发送http请求
	QNetworkReply* httpResp = this->sendHttpRequest("/service/friend/add_friend_apply", body);
	//处理http响应
	connect(httpResp, &QNetworkReply::finished, this, [this, httpResp, userId]() {
		std::shared_ptr<chat_im::FriendAddRsp> rsp = this->handleHttpResponse<chat_im::FriendAddRsp>(httpResp);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "发送好友申请失败:" << rsp->errmsg();
			return;
		}
		emit dataCenter->addFriendApplyDone();
		});
}

void NetClient::acceptFriendApply(const QString& loginSessionId, const QString& userId)
{
	chat_im::FriendAddProcessReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setAgree(true);
	req.setNotifyEventId("");
	req.setApplyUserId(userId);
	QByteArray body = req.serialize(&serializer);
	LOG() << "[同意好友申请] requestId=" << req.requestId() << " loginSessionId=" << loginSessionId << " userId=" << userId;
	//发送http请求
	QNetworkReply* httpResp = this->sendHttpRequest("/service/friend/add_friend_process", body);
	//处理http响应
	connect(httpResp, &QNetworkReply::finished, this, [this, httpResp, userId]() {
		std::shared_ptr<chat_im::FriendAddProcessRsp> rsp = this->handleHttpResponse<chat_im::FriendAddProcessRsp>(httpResp);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "同意好友申请失败:" << rsp->errmsg();
			return;
		}
		UserInfo info = dataCenter->removeFromApplyList(userId);
		QList<UserInfo>* friendList = dataCenter->getFriendList();
		if (friendList == nullptr) {
			//还未从服务器获取好友列表,就先从网络加载
			dataCenter->getFriendListAsync();
			return;
		}
		else {
			friendList->push_back(info);
		}
		emit dataCenter->acceptFriendApplyDone(userId,rsp->errmsg());
		});
}

void NetClient::rejectFriendApply(const QString& loginSessionId, const QString& userId)
{
	chat_im::FriendAddProcessReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setAgree(false);
	req.setNotifyEventId("");
	req.setApplyUserId(userId);
	QByteArray body = req.serialize(&serializer);
	LOG() << "[拒绝好友申请] requestId=" << req.requestId() << " loginSessionId=" << loginSessionId << " userId=" << userId;
	//发送http请求
	QNetworkReply* httpResp = this->sendHttpRequest("/service/friend/add_friend_process", body);
	//处理http响应
	connect(httpResp, &QNetworkReply::finished, this, [this, httpResp, userId]() {
		std::shared_ptr<chat_im::FriendAddProcessRsp> rsp = this->handleHttpResponse<chat_im::FriendAddProcessRsp>(httpResp);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "拒绝好友申请失败:" << rsp->errmsg();
			return;
		}
		dataCenter->removeFromApplyList(userId);
		emit dataCenter->rejectFriendApplyDone(userId,rsp->errmsg());
		});
}

void NetClient::createGroupChatSession(const QString& loginSessionId, const QList<QString>& memberList)
{
	chat_im::ChatSessionCreateReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setMemberIdList(memberList);
	req.setChatSessionName("群聊");
	QByteArray body = req.serialize(&serializer);

	LOG() << "[创建群聊会话] requestId=" << req.requestId() << " loginSessionId=" << loginSessionId << " memberList=" << memberList;
	QNetworkReply* reply = this->sendHttpRequest("/service/friend/create_chat_session", body);
	connect(reply, &QNetworkReply::finished, this, [this, reply]() {
		std::shared_ptr<chat_im::ChatSessionCreateRsp> rsp = this->handleHttpResponse<chat_im::ChatSessionCreateRsp>(reply);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "创建群聊会话失败:" << rsp->errmsg();
			return;
		}
		emit dataCenter->createGroupChatSessionDone();
		});

}

void NetClient::getChatSessionMember(const QString& loginSessionId, const QString& chat_session_id)
{
	chat_im::GetChatSessionMemberReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setChatSessionId(chat_session_id);
	QByteArray body = req.serialize(&serializer);
	LOG() << "[获取会话成员列表] requestId=" << req.requestId() << " loginSessionId=" << loginSessionId << " chatSessionId=" << chat_session_id;
	QNetworkReply* reply = this->sendHttpRequest("/service/friend/get_chat_session_member", body);
	connect(reply, &QNetworkReply::finished, this, [this, reply,chat_session_id]() {
		std::shared_ptr<chat_im::GetChatSessionMemberRsp> rsp = this->handleHttpResponse<chat_im::GetChatSessionMemberRsp>(reply);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "获取会话成员列表失败:" << rsp->errmsg();
			return;
		}
		dataCenter->resetChatSessionMember(chat_session_id, rsp);
		emit dataCenter->getChatSessionMemberDone(chat_session_id);
		});
}

void NetClient::searchAddFriend(const QString& loginSessionId, const QString& keyWord)
{
	chat_im::FriendSearchReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setSearchKey(keyWord);

	QByteArray body = req.serialize(&serializer);
	LOG() << "[搜索添加好友] requestId=" << req.requestId() << " loginSessionId=" << loginSessionId << " keyWord=" << keyWord;
	QNetworkReply* reply = this->sendHttpRequest("/service/friend/search_friend", body);
	connect(reply, &QNetworkReply::finished, this, [this, reply]() {
		std::shared_ptr<chat_im::FriendSearchRsp> rsp = this->handleHttpResponse<chat_im::FriendSearchRsp>(reply);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "搜索添加好友失败:" << rsp->errmsg();
			return;
		}
		dataCenter->resetSearchAddFriendList(rsp);
		emit dataCenter->searchAddFriendDone();
		});
}

void NetClient::searchMessage(const QString& loginSessionId,const QString& chat_session_id, const QString& keyWord)
{
	chat_im::MsgSearchReq req;
	req.setRequestId(makeRequestId());
	req.setChatSessionId(chat_session_id);
	req.setSessionId(loginSessionId);
	req.setSearchKey(keyWord);

	QByteArray body = req.serialize(&serializer);
	LOG() << "[搜索历史消息] requestId=" << req.requestId() << " loginSessionId=" << loginSessionId << " chatSessionId=" << chat_session_id << " keyWord=" << keyWord;
	QNetworkReply* reply = this->sendHttpRequest("/service/message_storage/search_history", body);
	connect(reply, &QNetworkReply::finished, this, [this, reply, chat_session_id]() {
		std::shared_ptr<chat_im::MsgSearchRsp> rsp = this->handleHttpResponse<chat_im::MsgSearchRsp>(reply);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "搜索历史消息失败:" << rsp->errmsg();
			return;
		}
		dataCenter->resetSearchMessageList(rsp->messageList());
		emit dataCenter->searchMessageDone();
		});
}

void NetClient::userLogin(const QString& username, const QString& password)
{
	chat_im::UserLoginReq req;
	req.setRequestId(makeRequestId());
	req.setUserName(username);
	req.setPassword(password);

	QByteArray body = req.serialize(&serializer);
	LOG() << "[用户登录] requestId=" << req.requestId() << " username=" << username << " password=" << password;
	QNetworkReply* reply = this->sendHttpRequest("/service/user/username_login", body);
	//
	connect(reply, &QNetworkReply::finished, this, [this, reply]() {
		std::shared_ptr<chat_im::UserLoginRsp> rsp = this->handleHttpResponse<chat_im::UserLoginRsp>(reply);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "用户登录失败:" << rsp->errmsg();
			return;
		}
		dataCenter->setLoginSessionId(rsp->loginSessionId());
		emit dataCenter->userLoginDone(true,"");
		});

}

void NetClient::phoneLogin(const QString& phone, const QString& verifycode,const QString& verifycodeId)
{
	chat_im::PhoneLoginReq req;
	req.setRequestId(makeRequestId());
	req.setPhoneNumber(phone);
	req.setVerifyCode(verifycode);
	req.setVerifyCodeId(verifycodeId);

	QByteArray body = req.serialize(&serializer);
	LOG() << "[手机号登录] requestId=" << req.requestId() << " phone=" << phone << " verifycode=" << verifycode << " verifycodeId=" << verifycodeId;
	QNetworkReply* reply = this->sendHttpRequest("/service/user/phone_login", body);
	//
	connect(reply, &QNetworkReply::finished, this, [this,reply]() {
		std::shared_ptr<chat_im::PhoneLoginRsp> rsp = this->handleHttpResponse<chat_im::PhoneLoginRsp>(reply);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "手机号登录失败:" << rsp->errmsg();
			emit dataCenter->phoneLoginDone(false, rsp->errmsg());
			return;
		}
		dataCenter->setLoginSessionId(rsp->loginSessionId());
		emit dataCenter->phoneLoginDone(true, "");

		});

}

void NetClient::phoneRegister(const QString& phone, const QString& password, const QString& verifycode, const QString& verifycodeId)
{
	chat_im::PhoneRegisterReq req;
	req.setRequestId(makeRequestId());
	req.setPhoneNumber(phone);
	req.setPassword(password);
	req.setVerifyCode(verifycode);
	req.setVerifyCodeId(verifycodeId);
	
	QByteArray body = req.serialize(&serializer);
	LOG() << "[手机号注册] requestId=" << req.requestId() << " phone=" << phone << " password=" << password << " verifycode=" << verifycode << " verifycodeId=" << verifycodeId;
	QNetworkReply* reply = this->sendHttpRequest("/service/user/phone_register", body);
	//
	connect(reply, &QNetworkReply::finished, this, [this, reply]() {
		std::shared_ptr<chat_im::PhoneRegisterRsp> rsp = this->handleHttpResponse<chat_im::PhoneRegisterRsp>(reply);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "手机号注册失败:" << rsp->errmsg();
			emit dataCenter->phoneRegisterDone(false, rsp->errmsg());
			return;
		}
		emit dataCenter->phoneRegisterDone(true, "");
		});
}

void NetClient::getSingleFile(const QString& loginSessionId, const QString& fileId)
{
	chat_im::GetSingleFileReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setFileId(fileId);

	QByteArray body = req.serialize(&serializer);
	LOG() << "[获取单文件内容] requestId=" << req.requestId() << " loginSessionId=" << loginSessionId << " fileId=" << fileId;
	QNetworkReply* reply = this->sendHttpRequest("/service/file/get_single_file", body);
	//
	connect(reply, &QNetworkReply::finished, this, [this, reply,fileId]() {
		std::shared_ptr<chat_im::GetSingleFileRsp> rsp = this->handleHttpResponse<chat_im::GetSingleFileRsp>(reply);
		if (!rsp) {
			return;
		}
		if (!rsp->success()) {
			LOG() << "获取单文件内容失败:" << rsp->errmsg();
			return;
		}
		emit dataCenter->getSingleFileDone(fileId,rsp->fileData().fileContent());
		});
}

void NetClient::speechRecognition(const QString& loginSessionId, const QString& fileId, const QByteArray& content)
{
	chat_im::SpeechRecognitionReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(loginSessionId);
	req.setSpeechContent(content);

	LOG() << "[获取语音转文字内容] requestId=" << req.requestId() << " loginSessionId=" << loginSessionId << " fileId=" << fileId;
	QByteArray body = req.serialize(&serializer);
	QNetworkReply* reply = this->sendHttpRequest("/service/speech/recognition", body);

	connect(reply, &QNetworkReply::finished, this, [this, reply, fileId]() {
		std::shared_ptr<chat_im::SpeechRecognitionRsp> rsp = this->handleHttpResponse<chat_im::SpeechRecognitionRsp>(reply);
		if (!rsp || !rsp->success()) {
			emit dataCenter->speechRecognitionDone(fileId, false, "语音识别服务失败");
			return;
		}
		emit dataCenter->speechRecognitionDone(fileId, true, "", rsp->recognitionResult());
		});
}

QString NetClient::makeRequestId()
{
	return QString("R"+QUuid::createUuid().toString());
}


void NetClient::initWebSocket()
{
	//连接服务
	connect(&websocketClient, &QWebSocket::connected, this, [this]() {
		LOG() << "connected";
		sendAuthentication();

		});

	//断开连接
	connect(&websocketClient, &QWebSocket::disconnected, this, []() {

		});

	//错误
	connect(&websocketClient, &QWebSocket::errorOccurred, this, []() {

		});

	//文本消息
	connect(&websocketClient, &QWebSocket::textMessageReceived, this, []() {

		});

	//二进制消息
	connect(&websocketClient, &QWebSocket::binaryMessageReceived, this, [this](const QByteArray& byteArray) {
		LOG() << "webSocket binary receive";
		std::shared_ptr<chat_im::NotifyMessage> respObj = std::make_shared<chat_im::NotifyMessage>();
		respObj->deserialize(&this->serializer,byteArray);
		handleWsResponse(respObj);
		});


	//开始 websocketClient 连接
	websocketClient.open(WEBSOCKET_URL);
	
}


void NetClient::sendAuthentication()
{
	if (!websocketClient.isValid()) {
		LOG() << "websocket inValid";
		return;
	}
	chat_im::ClientAuthenticationReq req;
	req.setRequestId(makeRequestId());
	req.setSessionId(dataCenter->getLoginSessionId());

	QByteArray body = req.serialize(&serializer);
	websocketClient.sendBinaryMessage(body);
	LOG() << "[ws身份验证] requestId=" << req.requestId() << ",loginSessionId=" << req.sessionId();


}

QNetworkReply* NetClient::sendHttpRequest(const QString& api_path, const QByteArray& body)
{
	QNetworkRequest req;
	req.setUrl(QUrl(HTTP_URL + api_path));
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-protobuf");
	
	QNetworkReply* reply = httpClient.post(req, body);
	return reply;
}


//接收服务器传来的新会话消息
void NetClient::handleWsMessage(model::Message message)
{
	QList<Message>* message_list = dataCenter->getRecentMessageList(message.chatiSessionId);
	if (message_list == nullptr) {
		//还未从服务器获取消息列表,就先从网络加载
		connect(dataCenter, &DataCenter::getRecentMessageDoneNotUi, dataCenter, &DataCenter::receiveMessage, Qt::UniqueConnection);
		dataCenter->getRecentMessageAsync(message.chatiSessionId, false);
	}
	else {
		message_list->push_back(message);
		dataCenter->receiveMessage(message.chatiSessionId);
	}

}

void NetClient::handleWsSessionCreate(model::ChatSessionInfo chatSessionInfo)
{
	QList<ChatSessionInfo>* chatSessionList = dataCenter->getChatSessionList();
	if (chatSessionList == nullptr) {
		//还未从服务器获取会话列表,就先从网络加载
		dataCenter->getChatSessionListAsync();
		return;
	}
	//添加到会话列表
	chatSessionList->push_front(chatSessionInfo);
	//
	emit dataCenter->receiveChatSessionDone();
}

void NetClient::handleWsAddFriendApplyReq(model::UserInfo userInfo)
{
	//添加数据到applyList中
	QList<UserInfo>* applyList = dataCenter->getApplyList();
	if (applyList == nullptr) {
		//还未从服务器获取好友申请列表,就先从网络加载
		dataCenter->getApplyListAsync();
		return;
	}
	applyList->push_front(userInfo);
	emit dataCenter->reveiveFriendApplyDone();
}

void NetClient::handleWsAddFriendRsp(model::UserInfo userInfo, bool agree)
{
	if (agree) {
		QList<UserInfo>* friendList = dataCenter->getFriendList();
		if (friendList == nullptr) {
			//还未从服务器获取好友列表,就先从网络加载
			dataCenter->getFriendListAsync();
			return;
		}
		friendList->push_back(userInfo);
		emit dataCenter->reveiveFriendProcessAccept(userInfo);
	}
	else {
		emit dataCenter->reveiveFriendProcessReject(userInfo);
	}
}

void NetClient::handleWsRemoveFriend(QString userId)
{
	dataCenter->removeFriend(userId);
	emit dataCenter->deleteFriendDone(userId);
}

void NetClient::handleWsResponse(std::shared_ptr<chat_im::NotifyMessage> respObj)
{
	switch (respObj->type())
	{
		//会话消息
	case chat_im::NotifyTypeGadget::NotifyType::CHAT_MESSAGE :{
		model::Message message;
		message.load(respObj->newMessageInfo().messageInfo());
		handleWsMessage(message);
		break;
	}
		//创建会话
	case chat_im::NotifyTypeGadget::NotifyType::CHAT_SESSION_CREATE: {
		model::ChatSessionInfo chatSessionInfo;
		chatSessionInfo.load(respObj->newChatSessionInfo().chatSessionInfo());
		handleWsSessionCreate(chatSessionInfo);
		break;
	}
		//好友申请
	case chat_im::NotifyTypeGadget::NotifyType::FRIEND_ADD_APPLY: {
		model::UserInfo userInfo;
		userInfo.load(respObj->friendAddApply().userInfo());
		handleWsAddFriendApplyReq(userInfo);
		break;
	}
		//发送出去的好友申请被处理了
	case chat_im::NotifyTypeGadget::NotifyType::FRIEMD_ADD_PROCESS: {
		model::UserInfo userInfo;
		userInfo.load(respObj->friendAddProcess().userInfo());
		handleWsAddFriendRsp(userInfo, respObj->friendAddProcess().agree());
		break;
	}
		//删除好友
	case chat_im::NotifyTypeGadget::NotifyType::FRIEND_REMOVE: {
		handleWsRemoveFriend(respObj->friendRemove().userId());
		break;
	}

	default: {
		LOG() << "unkown notifyType ! notifyType=" << respObj->type();
		break;

	}
	}
}

