#include "loginwidget.h"
#include "ui_loginwidget.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget) {
    ui->setupUi(this);
}

LoginWidget::~LoginWidget() {
    delete ui;
}

// 获取用户名昵称
QString LoginWidget::getUsername() const {
    // trimmed 删除前后多余空格
    return ui->usernameEdit->text().trimmed();
}

// 获取明文密码
QString LoginWidget::getPassword() const {
    return ui->pwdEdit->text();
}
