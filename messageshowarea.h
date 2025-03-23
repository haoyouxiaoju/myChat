#ifndef MESSAGESHOWAREA_H
#define MESSAGESHOWAREA_H

#include <QScrollArea>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QLabel>
#include <QFont>
#include <QPushButton>
#include <QPaintDevice>
#include <QPainter>
#include <QPainterPath>
#include <QProcess>
#include "model/data.h"
#include "otherusrinfowidget.h"



//
//会话消息展示
class MessageShowArea : public QScrollArea
{
    Q_OBJECT
public:
    MessageShowArea(QWidget* parent = nullptr);

    void scrollToEndLater();
    
    //尾插
    void addItem(bool isLeft, const model::Message& message);
    //头插
    void addFrontItem(bool isLeft, const model::Message& message);
    void clear();

private:
    QWidget* container;
};


class MessageShowItem : public QWidget{
    Q_OBJECT
public:
    MessageShowItem(bool isLeft);

    static MessageShowItem* MakeMessageItem(bool isLeft,const model::Message& message);

private:
    static QWidget* makeTextMessageItem(const QByteArray& content,bool isLeft);
    static QWidget* makeImageMessageItem(const QString& fileId,const QByteArray& content,bool isLeft);
    static QWidget* makeFileMessageItem(const QString& fileId,const QByteArray& content,bool isLeft ,const QString& fileName);
    static QWidget* makeSoundMessgaeItem(const QString& fileId,const QByteArray& content,bool isLeft);

private:
    bool isLeft;


};

class MessageContent : public QWidget{
    Q_OBJECT
public :
    MessageContent(model::MessageType type,const QString& text, const QString& fileId,const QByteArray& content,bool isLeft);

	void updateUI(const QString& fileId, const QByteArray& content);
protected:
    void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private :
	void saveFile(const QByteArray& content);
    void speechRecognition();
    void speechRecognitionDone(const QString& fileId, bool ok, const QString& errmsg, const QString& text = "");
    void openFileWithSystemDefaultApp(const QString& filePath);

private:
    QLabel* label;
    model::MessageType type;
    QString fileId;
    QByteArray content;
    bool isLeft;
    bool status;    //对于文件 用于标识是否被保存
	QString fileName;
	QProcess process;

};

class MessageImage : public QWidget {
	Q_OBJECT
public:
	MessageImage(const QString& fileId,const QByteArray& content, bool isLeft);
	void updateUI(const QString& fileId, const QByteArray& content);

protected:
	void paintEvent(QPaintEvent* event) override;
private:
	QPushButton* image;
	QString fileId;
	QByteArray content;
	bool isLeft;
};


#endif // MESSAGESHOWAREA_H
