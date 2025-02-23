#include "netclient.h"
#include <QNetworkReply>
#include <QUuid>
#include <QNetWorkRequest>
#include "../model/datacenter.h"
#include "webSocket.qpb.h"
#include "user.qpb.h"
#include "message.qpb.h"

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

		if (!user_info) {
			
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
	QNetworkReply* reply = this->sendHttpRequest("/service/user/get_friend_list", body);

	connect(reply, &QNetworkReply::finished, this, [this,reply]() {
		std::shared_ptr<chat_im::GetFriendListRsp> friend_list = this->handleHttpResponse<chat_im::GetFriendListRsp>(reply);

		if (!friend_list) {
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
	QNetworkReply* reply = this->sendHttpRequest("/service/user/get_chat_session_list", body);
	
	connect(reply, &QNetworkReply::finished, this, [this, reply]() {
		std::shared_ptr<chat_im::GetChatSessionListRsp> session_list = this->handleHttpResponse<chat_im::GetChatSessionListRsp>(reply);

		if (!session_list) {
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

		if (!apply_list) {
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

		if (!recent_message) {
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

void NetClient::handleWsMessage(model::Message message)
{
}

void NetClient::handleWsSessionCreate(model::ChatSessionInfo chatSessionInfo)
{
}

void NetClient::handleWsAddFriendApplyReq(model::UserInfo userInfo)
{
}

void NetClient::handleWsAddFriendRsp(model::UserInfo userInfo, bool agree)
{
}

void NetClient::handleWsRemoveFriend(QString userId)
{
}

void NetClient::handleWsResponse(std::shared_ptr<chat_im::NotifyMessage> respObj)
{
	switch (respObj->type())
	{
		//会话消息
	case chat_im::NotifyTypeGadget::CHAT_MESSAGE: {
		model::Message message;
		message.load(respObj->newMessageInfo().messageInfo());
		handleWsMessage(message);
		break;
	}
		//创建会话
	case chat_im::NotifyTypeGadget::CHAT_SESSION_CREATE: {
		model::ChatSessionInfo chatSessionInfo;
		chatSessionInfo.load(respObj->newChatSessionInfo().chatSessionInfo());
		handleWsSessionCreate(chatSessionInfo);
		break;
	}
		//好友申请
	case chat_im::NotifyTypeGadget::FRIEND_ADD_APPLY: {
		model::UserInfo userInfo;
		userInfo.load(respObj->friendAddApply().userInfo());
		handleWsAddFriendApplyReq(userInfo);
		break;
	}
		//发送出去的好友申请被处理了
	case chat_im::NotifyTypeGadget::FROEMD_ADD_PROCESS: {
		model::UserInfo userInfo;
		userInfo.load(respObj->friendAddProcess().userInfo());
		handleWsAddFriendRsp(userInfo, respObj->friendAddProcess().agree());
		break;
	}
		//删除好友
	case chat_im::NotifyTypeGadget::FRIEND_REMOVE: {
		handleWsRemoveFriend(respObj->friendRemove().userId());
		break;
	}

	default: {
		LOG() << "unkown notifyType ! notifyType=" << respObj->type();
		break;

	}
	}
}

