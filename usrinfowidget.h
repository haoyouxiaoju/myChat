#ifndef USRINFOWIDGET_H
#define USRINFOWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QDiaLog>
#include <QApplication>

class usrInfoWidget : public QDialog
{
    Q_OBJECT
public:
    explicit usrInfoWidget(QWidget *parent = nullptr);

signals:



protected:
    void changeEvent(QEvent* event) override;

private:
    void clickAvatar();
    void clickAvatarDone();


private:
    //初始化 槽和信号的连接
    void initSlotsToSignals();
    
    //修改用户昵称
    void clickNameModifyBtn();
    void clickNameSubmitBtn();
	void clickNameSubmitBtnDone();

    //修改用户签名
    void clickSignatureModifyBtn();
    void clickSignatureSubmitBtn();
    void clickSignatureSubmitBtnDone();

   //修改用户电话号码 
    void clickPhoneModifyBtn();
    void clickPhoneSubmitBtn();
    void clickPhoneSubmitBtnDone();

    //获取验证码
    void clickSendVerityCodeBtn();

private:
    QPushButton* avatar;
    //序号
    QLabel* id_label;
    QLineEdit* id_edit;
    //昵称
    QLabel* userName_label;
    QLineEdit* userName_edit;
    QPushButton* userName_ModifiyButton;
    QPushButton* userName_SubmitButton;
    //签名
    QLabel* desc_label;
    QLineEdit* desc_edit;
    QPushButton* desc_ModifiyButton;
    QPushButton* desc_SubmitButton;
    //电话
    QLabel* phone_label;
    QLineEdit* phone_edit;
    QPushButton* phone_ModifiyButton;
    QPushButton* phone_SubmitButton;
    QLabel* code_label;
    QLineEdit* phone_code_edit;
    QPushButton* sendCode_button;
    QString phoneToChange;
    int time;

    //this layout
    QGridLayout* layout;

};

#endif // USRINFOWIDGET_H
