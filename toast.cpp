#include "toast.h"
#include <QLabel>
#include <QDialog>
#include <QScreen>
#include <QApplication>
#include <QHBoxLayout>
#include <QTimer>

Toast::Toast(const QString& text) : QDialog(nullptr) {
	// 1. 设置基本参数
	this->setFixedSize(500, 100);
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setWindowIcon(QIcon(":/image/logo.png"));
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setStyleSheet("QDialog { background-color: rgb(255, 255, 255); border-radius: 10px;}");
	// 2. 计算摆放位置
	QScreen* screen = QApplication::primaryScreen();
	int width = screen->size().width();
	int height = screen->size().height();
	int x = (width - this->width()) / 2;
	int y = height - this->height() - 100;
	this->move(x, y);
	// 3. 添加内容
	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(20, 20, 20, 20);
	this->setLayout(layout);
	QLabel* label = new QLabel();
	label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	label->setAlignment(Qt::AlignCenter);
	label->setText(text);
	QFont font;
	font.setPointSize(18);
	label->setFont(font);
	layout->addWidget(label);
	// 4. 实现 2s 后⾃动关闭窗⼝
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, [=]() {
		timer->stop();
		this->close();
		});
	timer->start(2000);
}
void Toast::showMessage(const QString& text) {
	Toast* toast = new Toast(text);
	toast->show();
}

Toast::~Toast()
{
}
