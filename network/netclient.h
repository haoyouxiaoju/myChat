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
	const QString HTTP_URL = "http://192.168.91.128:9000";
	const QString WEBSOCKET_URL = "ws://192.168.91.128:9001/ws";

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

	//发送消息
	void sendMessage(const QString& loginSessionId, const QString& chatSessionId, model::MessageType type, const QByteArray& body, const QString& extraInfo = "");

	//修改昵称请求
	void changeNickName(const QString& loginSessionId, const QString& nickName);
	void changeDescription(const QString& loginSessionID, const QString& newDesc);

	//修改手机号
	void changePhone(const QString& loginSessionId, const QString& newPhone, const QString& verifyCodeId,const QString& verifyCode);
	//获取验证码
	void getVerifyCode(const QString& phone);
	void changeAvatar(const QString& loginSessionId, const QByteArray& body);
	//删除好友
	void deleteFriend(const QString& loginSessionId, const QString& userId);
	//发送好友申请
	void addFriendApply(const QString& loginSessionId, const QString& userId);
	//同意好友申请
	void acceptFriendApply(const QString& loginSessionId, const QString& userId);
	//拒绝好友申请
	void rejectFriendApply(const QString& loginSessionId, const QString& userId);
	//创建群聊会话
	void createGroupChatSession(const QString& loginSessionId, const QList<QString>& memberList);

	//获取会话成员列表
	void getChatSessionMember(const QString& loginSessionId, const QString& chat_session_id);

	//搜索添加好友
	void searchAddFriend(const QString& loginSessionId, const QString& keyWord);

	//搜索历史消息
	void searchMessage(const QString& loginSessionId,const QString& chat_session_id, const QString& keyWord);

	//用户登录
	void userLogin(const QString& username, const QString& password);

	//手机号登录
	void phoneLogin(const QString& phone, const QString& verifycode,const QString& verifycodeId);

	//手机号注册
	void phoneRegister(const QString& phone, const QString& password, const QString& verifycode, const QString& verifycodeId);

	//获取单文件内容
	void getSingleFile(const QString& loginSessionId, const QString& fileId);
	//语音识别
	void speechRecognition(const QString& loginSessionId, const QString& fileId, const QByteArray& content);

	//
	// 生成一个请求id
	static QString makeRequestId();

	void initWebSocket();
private:
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
