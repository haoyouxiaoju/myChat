#pragma once
#include <QWidget>
#include <QRandomGenerator>

class VerifyCodeWidget : public QWidget
{
	Q_OBJECT
public:
	explicit VerifyCodeWidget(QWidget* parent = nullptr);
	// 重写绘图事件. 绘制验证码
	void paintEvent(QPaintEvent* event) override;
	// ⿏标点击时也刷新验证码
	void mousePressEvent(QMouseEvent* event) override;
	// 刷新验证码
	void refreshVerifyCode();
	// 检测验证码是否匹配
	bool checkVerifyCode(const QString& code);
	QString getVerifyCode() const { return verifyCode; }
private:
	// 随机数⽣成器
	QRandomGenerator randomGenerator;
	// 保存验证码的值
	QString verifyCode = "";
	// ⽣成验证码
	QString generateVerifyCode();
signals:
};
