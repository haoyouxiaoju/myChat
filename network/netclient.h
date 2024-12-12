#pragma once
#include <QNetworkAccessManager>
#include <QWebSocket>
#include <QProtobufSerializer>
#include <QObject>
#include <QNetWorkReply>
#include "notify.qpb.h"
#include "../model/data.h"

//#include "../model/datacenter.h"

#define TEXT_SERVER 1

namespace model {
	class DataCenter;

}

class NetClient : public QObject
{
	Q_OBJECT
#if TEXT_SERVER
	const QString HTTP_URL = "http://127.0.0.1:8000";
	const QString WEBSOCKET_URL = "http://127.0.0.1:8001/ws";

#else
	const QString HTTP_URL = "http://127.0.0.1:8000";
	const QString WEBSOCKET_URL = "http://127.0.0.1:8001/ws";

#endif



public:
	NetClient(model::DataCenter *dataCenter);
	~NetClient();

public:
	void ping();

	//
	//
	void getMySelf(const QString& login_session_id);
	void getFriendList(const QString& login_session_id);
	void getChatSessionList(const QString& login_session_id);
	void getApplyList(const QString& login_session_id);
	void getRecentMessage(const QString& login_session_id, const QString& chat_session_id,bool updataUi);

	//
	// 生成一个请求id
	static QString makeRequestId();

private:
	void initWebSocket();
	void sendAuthentication();
	QNetworkReply* sendHttpRequest(const QString& api_path,const QByteArray& body);

	template<typename T>
	std::shared_ptr<T> handleHttpResponse(QNetworkReply* reply) {
		if (reply->error() != QNetworkReply::NoError) {
			LOG() << "http 请求失败:" << reply->errorString();
			reply->deleteLater();
			return std::shared_ptr<T>();
		}

		QByteArray body = reply->readAll();

		std::shared_ptr<T> ptr = std::make_shared<T>();
		ptr->deserialize(&this->serializer, body);

		if (!ptr->success()) {
			LOG() << "request_id = " << ptr->requestId() << ", errmsg=" << ptr->errmsg();
			reply->deleteLater();
			return std::shared_ptr<T>();
		}

		LOG() << "请求响应成功 ， request_id=" << ptr->requestId();
		reply->deleteLater();
		return ptr;
	}


	void handleWsMessage(model::Message message);
	void handleWsSessionCreate(model::ChatSessionInfo chatSessionInfo);
	void handleWsAddFriendApplyReq(model::UserInfo userInfo);
	void handleWsAddFriendRsp(model::UserInfo userInfo, bool agree);
	void handleWsRemoveFriend(QString userId);
	void handleWsResponse(std::shared_ptr<chat_im::NotifyMessage> respObj);

private:
	QNetworkAccessManager httpClient;
	QWebSocket websocketClient;

	model::DataCenter* dataCenter;

	//protobuf 序列化器
	QProtobufSerializer serializer;
	


};
