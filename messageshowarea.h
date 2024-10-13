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
#include "model/data.h"
#include "otherusrinfowidget.h"




class MessageShowArea : public QScrollArea
{
    Q_OBJECT
public:
    MessageShowArea(QWidget* parent = nullptr);
    
    void addItem(bool isLeft, const model::Message& message);
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

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QLabel* label;
    model::MessageType type;
    QString fileId;
    QByteArray content;
    bool isLeft;

};

#endif // MESSAGESHOWAREA_H
