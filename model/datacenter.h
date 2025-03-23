#pragma once
#include <QObject>
#include <QString>
#include <QHash>

#include "../network/netclient.h"

#include "user.qpb.h"
#include "friend.qpb.h"
#include "message.qpb.h"
#include "data.h"


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

		const QIcon& getIcon(const QByteArray& data);

		ChatSessionInfo* findChatSessionByUserId(const QString& userId);

		void topChatSessionInfo(const ChatSessionInfo& chat_session_info);




		//
		//获取用户自身信息
		void resetMySelf(std::shared_ptr<chat_im::GetUserInfoRsp> myself);
		void getMySelfAsync();
		UserInfo* getMySelf();

		//
		//获取好友列表信息
		void resetFriendList(std::shared_ptr<chat_im::GetFriendListRsp> friend_list);
		void getFriendListAsync();
		QList<UserInfo>* getFriendList();

		//
		//获取会话列表信息
		void resetChatSessionList(std::shared_ptr<chat_im::GetChatSessionListRsp> session_list);
		void getChatSessionListAsync();
		QList<ChatSessionInfo>* getChatSessionList();

		//
		//获取好友申请列表
		void resetApplyList(std::shared_ptr<chat_im::GetPendingFriendEventListRsp> apply_list);
		void getApplyListAsync();
		QList<UserInfo>* getApplyList();

		//
		//获取会话的信息
		ChatSessionInfo* getChatSessionInfo(const QString& chat_session_id);
		//获取当前选中会话的会话id
		const QString& getCurrentChatSessionId();
		void setCurrentChatSessionId(const QString& chat_session_id);

		//
		//获取会话最近消息
		void resetRecentMessage(const QString& chat_session_id,std::shared_ptr<chat_im::GetRecentMsgRsp> recentMessage_list);
		void getRecentMessageAsync(const QString& chat_session_id,bool updataUi);
		QList<model::Message>* getRecentMessageList(const QString& chat_session_id);

		//发送消息
		void sendTextMessageAsync(const QString& chat_session_id, const QString& body);
		const Message& addMessage(const Message& message);
		
		//未读消息数据处理
		void clearUnread(const QString& chatSessionId);
		void addUnread(const QString& chatSessionId);
		int getUnread(const QString& chatSessionId);

		//收到服务器发来的消息
		void receiveMessage(const QString& chatSessionId);

		//修改用户昵称
		void changeNickNameAsync(const QString& nickName);
		//修改用户签名
		void changeDescriptionAsync(const QString& newDesc);
		//修改用户电话
		void changePhoneAsync(const QString& newPhone,const QString& verifyCodeId,const QString& verifyCode);
		void getVerifyCodeAsync(const QString& phone);

		//修改头像
		void changeAvatarAsync(const QByteArray& body);
		//删除好友
		void deleteFriendAsync(const QString& userId);
		//发送好友申请
		void addFriendApplyAsync(const QString& userId);
		//同意好友申请
		void acceptFriendApplyAsync(const QString& userId);
		//拒绝好友申请
		void rejectFriendApplyAsync(const QString& userId);

		//创建群聊会话
		void createGroupChatSessionAsync(const QList<QString>& memberList);

		//获取会话成员列表
		void getChatSessionMemberAsync(const QString& chat_session_id);

		//搜索用户
		void searchAddFriendAsync(const QString& keyWord);

		//搜索历史消息
		void searchMessageAsync(const QString& keyWord);

		//用户登录
		void loginAsync(const QString& username, const QString& password);

		//手机号登录
		void phoneLoginAsync(const QString& phone,const QString& verifycode);

		//手机号注册
		void phoneRegisterAsync(const QString& phone,const QString& password,const QString& verifycode);

		//获取文件内容
		void getSingleFileAsync(const QString& fileId);

		//发送图片消息
		void sendImageMessageAsync(const QString& chat_session_id, const QByteArray& body);

		//发送文件消息
		void sendFileMessageAsync(const QString& chat_session_id, const QByteArray& body, const QString& fileName);

		//发送语音消息
		void sendSpeechMessageAsync(const QString& chat_session_id, const QByteArray& content);
		//语音识别
		void speechRecognitionAsync(const QString& fileId, const QByteArray& content);



	public:


		void resetNickName(const QString& nickName);
		void resetDescription(const QString& description);
		void resetPhone(const QString& phone);
		void resetVerifyCodeId(const QString& id);
		void resetAvatar(const QByteArray& body);
		void removeFriend(const QString& id);
		void resetChatSessionMember(const QString& chat_session_id, std::shared_ptr<chat_im::GetChatSessionMemberRsp> member_list);
		void resetSearchAddFriendList(std::shared_ptr<chat_im::FriendSearchRsp> searchUserResult);
		void resetSearchMessageList(const QList<chat_im::MessageInfo>& messageList);
		void setLoginSessionId(const QString& loginSessionId);
		const QString& getLoginSessionId();
		const QString& getVerifyCodeId();
		UserInfo* getFriendById(const QString& user_id);
		UserInfo removeFromApplyList(const QString& user_id);
		ChatSessionInfo* findChatSessionBySessionId(const QString& chat_session_id);
		QList<UserInfo>* getMemberListBySessionId(const QString& chat_session_id);
		QList<UserInfo>* getSearchUserResult();
		QList<Message>* getSearchMessageResult();


	private:
		DataCenter();
		~DataCenter();

	signals:
		void getMySelfDone();//已获取用户自身信息完成
		void getFriendListDone();//已获取好友列表信息完成
		void getChatSessionListDone();
		void getApplyListDone();
		void getRecentMessageDone(const QString& chat_session_id);
		void getRecentMessageDoneNotUi(const QString& chat_session_id);
		void sendMessageFailed(const QString& err_msg);//发送消息失败
		void sendMessageDone(model::MessageType type, const QByteArray& body, const QString& extraInfo);//发送消息结束
		void updateLastMessage(const QString& chatSessionId);	//修改聊天会话列表中会话显示的最后一条消息
		void receiveMessageDone(const Message& message);		//收到消息结束
		void changeNickNameDone();		//修改用户昵称结束
		void changeDescriptionDone();	//	修改用户签名结束
		void changeUserPhoneDone();	//修改用户手机号结束
		void getVerifyCodeDone();//获取验证码结束
		void changeAvatarDone();//修改用户头像结束
		void deleteFriendDone(const QString& user_id);//删除好友结束
		void clearCurrentSession();
		void addFriendApplyDone();//添加好友结束
		void reveiveFriendApplyDone();//接收好友申请结束
		void acceptFriendApplyDone(const QString& userId,const QString& errmsg);//接受好友申请结束
		void rejectFriendApplyDone(const QString& userId, const QString& errmsg);//拒绝好友申请结束
		void reveiveFriendProcessAccept(const UserInfo& userInfo);//接收到接受好友申请
		void reveiveFriendProcessReject(const UserInfo& userInfo);//接受到拒绝好友申请
		void createGroupChatSessionDone();//创建群聊会话结束
		void receiveChatSessionDone();//接收到会话
		void getChatSessionMemberDone(const QString& chat_session_id);//获取会话成员列表结束
		void searchAddFriendDone();//搜索添加好友结束
		void searchMessageDone();//搜索历史消息结束
		void userLoginDone(bool ok ,const QString& reason);//用户登录结束
		void phoneLoginDone(bool ok,const QString& reason);//手机号登录结束
		void phoneRegisterDone(bool ok, const QString& reason);//手机号注册结束
		void getSingleFileDone(const QString& fileId,const QByteArray& data);//获取单文件内容结束
		void speechRecognitionDone(const QString& fileId,bool success,const QString errmsg,const QString body = "");









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

		QMap<QByteArray, QIcon>* __avatar;
	};


}
