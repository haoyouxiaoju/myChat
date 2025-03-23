#ifndef MESSAGEEDITAREA_H
#define MESSAGEEDITAREA_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollBar>
#include <QLabel>

#include "model/datacenter.h"


class MessageEditArea : public QWidget
{
    Q_OBJECT
public:
    MessageEditArea(QWidget *parent = nullptr);

    QPlainTextEdit* edit;
    QLabel* tiplabel;           //用于提示显示，默认情况下不可见
    QPushButton* sendBtn;       //发送
    QPushButton* sendImageBtn;
    QPushButton* sendFileBtn;
    QPushButton* sendSoundBtn;
    QPushButton* showHitstoryBtn;

private:

    //初始化槽和信号
    void initSignalSlot();

    //发送文本消息
    void sendTextMessage();
	//发送图片消息
	void clickSendImageBtn();
	//发送文件消息
	void clickSendFileBtn();
    //开始录制语音
    void soundRecordPressed();
    //结束录制语音
    void soundRecordReleased();
    //发送语音
    void sendSound(const QString& path);

    //把消息添加到消息显示区
    void addSelfMessage(model::MessageType type, const QByteArray& body, const QString& extraInfo);

    //处理其他人发来的消息
    void addOtherMessage(const model::Message& message);


private:
    QWidget* owner;

};

#endif // MESSAGEEDITAREA_H
