#pragma once
#include <QObject>
#include <QString>
#include <QHash>

#include "data.h"
#include "../network/netclient.h"

namespace model {

		// 登录会话id
		// 这个信息是登录成功后, 服务器返回的.
		// 返回之后这个数据需要被客⼾端持久化保存在⽂件中.
		// 后续每次启动, 都从⽂件中拿到这个内容.
		// 当⽤⼾显式点击 "退出登录", 则删除这个信息
	class DataCenter : public QObject {
		Q_OBJECT

	public:
		static DataCenter* getInstance();

		//初始数据文件
		void initDataFile();

		//保存必要的数据到文件
		void saveDataFile();

		//从文件加载必要的数据
		//如果文件和文件夹不存在则创建文件夹和文件，文件通过initDataFile()进行创建且初始化内容
		void loadDataFile(); 

	private:
		DataCenter();
		~DataCenter();






	private:
		QString loginSessionId = "";

		//用户信息
		UserInfo* myself = nullptr;

		//当前用户的好友列表
		QList<UserInfo>* friendList = nullptr;

		//当前用户的会话列表
		QList<ChatSessionInfo>* chatSessionList = nullptr;

		//当前用户选择的消息会话id
		QString currentChatSessionId = "";

		//待处理的好友申请列表
		QList<UserInfo>* applyList = nullptr;

		//每个会话中的用户列表		id->list
		QHash<QString, QList<UserInfo>>* memberList = nullptr;

		//当前用户的最新的会话消息内容，chatSessionId -> Message
		QHash<QString, QList<Message>>* recentMessages = nullptr;

		//未读消息数量的统计，chatSessionId-> count
		QHash<QString, int>* unreadMessageCount = nullptr;

		//搜索用户结果（寻找用户）
		QList<UserInfo> *searchUserResult = nullptr;
	
		//当前历史消息搜索的结果
		QList<Message>* searchMessageResult = nullptr;

		//短信验证码的验证id
		QString currentVerifyCodeId = "";

		NetClient netClient;

		DataCenter();




	};


}
