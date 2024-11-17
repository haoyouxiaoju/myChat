#pragma once

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include "messageshowarea.h"
#include "framelesswidget.h"

class messageHistoryWidget : public FramelessWidget
{
	Q_OBJECT

public:
	static messageHistoryWidget* getMessageHistoryWidget();

private:
	messageHistoryWidget(QWidget *parent=nullptr);
	~messageHistoryWidget();

	void addItem(const model::Message& message);
protected:
	void mousePressEvent(QMouseEvent* event) override;

private:
	QGridLayout* layout;
	QLabel* title;
	QLineEdit* search;
	QVBoxLayout* message_list_widget_layout;



    static messageHistoryWidget* w;

};

