#pragma once
#include <QString>
#include <QIcon>
#include <QUuid>
#include <QDateTime>
#include <QPixmap>
#include <QFile>
#include <QDebug>
#include <QFIleInfo>
#include "base.qpb.h"

namespace model{

/***** util *****/

// 定义一个宏 LOG()
static inline QString getFileName(const QString& path){
    QFileInfo info(path);
    return info.fileName();
}
#define POSITION QString("[%1:%2]").arg(model::getFileName(__FILE__),QString::number(__LINE__))
#define LOG() qDebug().noquote()<<POSITION

//		获取时间戳
static inline int64_t getTime(){
    return QDateTime::currentSecsSinceEpoch();
}

//		时间戳格式化"MM-dd HH:mm:ss"
static inline QString formatTime(int64_t time_s){
    return QDateTime::fromSecsSinceEpoch(time_s).toString("MM-dd HH:mm:ss");
}

//		QByteArray 转成 QIcon
static inline QIcon makeQIcon(const QByteArray& data){
    QPixmap pixmap;
    pixmap.loadFromData(data);
    QIcon icon(pixmap);
    return icon;
}

//		读写文件操作
//		读取文件，得到QByteArray
static inline QByteArray localFileToQByteArray(const QString& path){
    QFile file(path);
    file.open(QFile::ReadOnly);
    if(file.isOpen()!=true){
        LOG()<<"文件打开失败";
        return QByteArray();
    }
    QByteArray data = file.readAll();
    file.close();
    return data;
}
//		将QByteArray 写入指定文件
static inline bool writeQByteArrayToFile(const QString& path ,const QByteArray& data){

    QFile file(path);
    file.open(QFile::WriteOnly);
    if(file.isOpen()!=true){
        LOG()<<"文件打开失败";
        return false;
    }
    file.write(data);
    file.flush();
    file.close();

    return true;
}

/***** 用户信息 *****/
class UserInfo{

public:
    QString userId;				//用户编号
    QString nickname;			//用户昵称
    QString description;		//用户签名
    QString phone;				//手机号码
    QIcon avatar;				//用户头像

public:
    //*
    // 
    void load(const chat_im::UserInfo& info) {
        this->userId = info.userId();
        this->nickname = info.nickname();
        this->description = info.description();
        this->phone = info.phone();
        if (info.avatar().isEmpty()) {
            //如果没拿到头像数据
            this->avatar = QIcon(":/resource/images/xiaoju.jpg");
        }
        else {
            this->avatar =makeQIcon(info.avatar());
        }
    }

};


/***** 消息信息 *****/
enum MessageType{
    TEXT_TYPE,					//文本
    IMAGE_TYPE,					//图片
    FILE_TYPE,					//文件
    SPEECH_TYPE,				//语言
    UNKNOW_TYPE,                //未知类型

};

class Message{
public:
    QString messageId;			//消息编号);
    QString chatiSessionId;		//消息会话的编号
    QString time;				//消息时间
    MessageType messageType;	//消息类型
    UserInfo sender;			//发送者的信息
    QByteArray content;			//消息的正文
    QString fileId;				//文件的标识，当消息为文件，图片，语言时才有效，为文本时设置为""
    QString fileName;			//文件名称，消息为文本时设置为""

public:
    void load(const chat_im::MessageInfo& info) {
        this->messageId = info.messageId();
        this->chatiSessionId = info.chatSessionId();
		this->time = formatTime(info.timeStamp());
		this->sender.load(info.sender());
		switch (info.data().messageType()) {
		case chat_im::MessageTypeGadget::STRING: {
			this->messageType = MessageType::TEXT_TYPE;
			this->content = info.data().stringMessage().content().toUtf8();
			break;
		}
		case chat_im::MessageTypeGadget::IMAGE: {
            this->messageType = MessageType::IMAGE_TYPE;
            this->fileId = info.data().imageMessage().fileId();
            this->content = info.data().imageMessage().imageContent();
            break;
		}
        case chat_im::MessageTypeGadget::FILE: {
            this->messageType = MessageType::FILE_TYPE;
            this->fileId = info.data().fileMessage().fileId();
            this->fileName = info.data().fileMessage().fileName();
            if (info.data().fileMessage().hasFileContents()) {
                this->content = info.data().fileMessage().fileContents();
            }
            break;
        }
        case chat_im::MessageTypeGadget::SPEECH: {
            this->messageType = MessageType::SPEECH_TYPE;
            this->fileId = info.data().speechMessage().fileId();
            if (info.data().speechMessage().hasFileContents()) {
                this->content = info.data().speechMessage().fileContents();
            }
            break;
        }
        default: {
            this->messageType = MessageType::UNKNOW_TYPE;
        }
		}

    }


    static Message MakeMessage(MessageType type,const QString& chatiSessionId,const UserInfo& sender,const QByteArray& content,const QString& other_data){
        switch(type){
            case TEXT_TYPE:
                return MakeTextMessage(chatiSessionId,sender,content);
                break;
            case IMAGE_TYPE:
                return MakeImageMessage(chatiSessionId,sender,content);
                break;
            case FILE_TYPE:
                return MakeFileMessage(chatiSessionId,sender,content,other_data);
                break;
            case SPEECH_TYPE:
                return MakeSpeechMessage(chatiSessionId,sender,content);
                break;
            defult:
                return Message();

        }
        return Message();
    }
private:
    static QString makeMessageId(){
        return	QUuid::createUuid().toString();
    }
    static Message MakeTextMessage(const QString& chatiSessionId ,const UserInfo& sender,const QByteArray& content){
        Message message;
        message.messageType = TEXT_TYPE;
        message.chatiSessionId=chatiSessionId;
        message.sender = sender;
        message.content = content;
        message.messageId = makeMessageId();
        message.time = formatTime(getTime());
        message.fileId="";
        message.fileName="";
        return message;
    }
    static Message MakeImageMessage(const QString& chatiSessionId ,const UserInfo& sender,const QByteArray& content){
        Message message;
        message.messageType = IMAGE_TYPE;
        message.chatiSessionId=chatiSessionId;
        message.sender = sender;
        message.content = content;
        message.messageId = makeMessageId();
        message.time = formatTime(getTime());
        message.fileId="";
        message.fileName="";
        return message;
    }
    static Message MakeFileMessage(const QString& chatiSessionId ,const UserInfo& sender,const QByteArray& content,const QString& other_message){
        Message message;
        message.messageType = FILE_TYPE;
        message.chatiSessionId=chatiSessionId;
        message.sender = sender;
        message.content = content;
        message.messageId = makeMessageId();
        message.time = formatTime(getTime());
        message.fileId="";
        message.fileName=other_message;
        return message;
    }
    static Message MakeSpeechMessage(const QString& chatiSessionId ,const UserInfo& sender,const QByteArray& content){
        Message message;
        message.messageType = SPEECH_TYPE;
        message.chatiSessionId=chatiSessionId;
        message.sender = sender;
        message.content = content;
        message.messageId = makeMessageId();
        message.time = formatTime(getTime());
        message.fileId="";
        message.fileName="";
        return message;
    }
};

/***** 会话信息 *****/
class ChatSessionInfo{

public:
    QString chatSessionId;		//会话编号
    QString chatSessionName;	//会话名称
    Message lastMessage;		//表示最新消息
    QIcon avatar;				//会话头像
    QString userId;

public:
    void load(const chat_im::ChatSessionInfo& info) {
        this->chatSessionId = info.chatSessionId();
        this->chatSessionName = info.chatSessionName();
        if (info.hasPrevMessage()) {
			lastMessage.load(info.prevMessage());
        }
        if (info.hasAvatar()) {
            this->avatar = makeQIcon(info.avatar());
        }
        if (info.hasSingleId()) {
            this->userId = info.singleId();
        }
    }

};

}
/***** namespace model end *****/
