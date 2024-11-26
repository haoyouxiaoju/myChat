#pragma once

#include <QWidget>
#include <QDialog>

class Toast : public QDialog
{
	Q_OBJECT

public:
	//Toast(QWidget *parent);
	Toast(const QString& text);

	~Toast();

	static void showMessage(const QString& text);

};
