#include "verifycodewidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDateTime>


VerifyCodeWidget::VerifyCodeWidget(QWidget* parent)
	: QWidget{ parent }, randomGenerator(QDateTime::currentMSecsSinceEpoch()),
	verifyCode(generateVerifyCode())
{
	// 启动时也⽣成⼀个随机的验证码
	verifyCode = generateVerifyCode();
}
QString VerifyCodeWidget::generateVerifyCode()
{
	QString code;
	// ⽣成随机验证码序列
	for (int i = 0; i < 4; ++i)
	{
		// 验证码只有⼤写字⺟, 避免出现⼀些容易混淆的结果, ⽐如 o 和 0, ⽐如 1 I l 等.
		int init = 'A';
		code += static_cast<QChar>(init + randomGenerator.generate() % 26);
	}
	return code;
}
void VerifyCodeWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QPen pen;
	QFont font("楷体", 25, QFont::Bold, true);
	painter.setFont(font);
	// 画点: 添加随机噪点
	for (int i = 0; i < 100; i++)
	{
		pen = QPen(QColor(randomGenerator.generate() % 256,
			randomGenerator.generate() % 256, randomGenerator.generate() % 256));
		painter.setPen(pen);
		painter.drawPoint(randomGenerator.generate() % 180,
			randomGenerator.generate() % 80);
	}
	// 画线: 添加随机⼲扰线
	for (int i = 0; i < 5; i++)
	{
		pen = QPen(QColor(randomGenerator.generate() % 256,
			randomGenerator.generate() % 256, randomGenerator.generate() % 256));
		painter.setPen(pen);
		painter.drawLine(randomGenerator.generate() % 180,
			randomGenerator.generate() % 80, randomGenerator.generate() % 180,
			randomGenerator.generate() % 80);
	}
	// 绘制验证码
	for (int i = 0; i < verifyCode.size(); i++)
	{
		pen = QPen(QColor(randomGenerator.generate() % 255,
			randomGenerator.generate() % 255, randomGenerator.generate() % 255));
		painter.setPen(pen);
		painter.drawText(5 + 20 * i, randomGenerator.generate() % 10, 30, 30,
			Qt::AlignCenter, QString(verifyCode[i]));
	}
}
void VerifyCodeWidget::mousePressEvent(QMouseEvent* event)
{
	refreshVerifyCode();
}
void VerifyCodeWidget::refreshVerifyCode()
{
	verifyCode = generateVerifyCode();
	update();
}
bool VerifyCodeWidget::checkVerifyCode(const QString& code)
{
	// 按照⼤⼩写不敏感的⽅式进⾏匹配
	return verifyCode.compare(code, Qt::CaseInsensitive) == 0;
}
