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
    QLineEdit* phone_code_edit;
    QPushButton* sendCode_button;

    //this layout
    QGridLayout* layout;

};

#endif // USRINFOWIDGET_H
