#ifndef MESSAGEEDITAREA_H
#define MESSAGEEDITAREA_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollBar>


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
    QWidget* owner;

};

#endif // MESSAGEEDITAREA_H
