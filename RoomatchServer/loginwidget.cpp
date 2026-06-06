#include "databasemanager.h"
#include "loginwidget.h"
#include "ui_loginwidget.h"


#include <QMessageBox>
#include <QRegularExpressionValidator>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget) {
    ui->setupUi(this);

    // 姓名，中文、英文、数字，1~16 位
    QRegularExpression nameReg("^[a-zA-Z0-9\\p{Han}]{1,16}$");
    QRegularExpressionValidator *nameValidator = new QRegularExpressionValidator(nameReg, this);
    ui->usernameEdit->setValidator(nameValidator);

    // 密码 6~16 位，不包含空格、反斜杠
    QRegularExpression codeReg("^[^\\s\\\\]{6,16}$");
    QRegularExpressionValidator *codeValidator = new QRegularExpressionValidator(codeReg, this);
    ui->pwdEdit->setValidator(codeValidator);

    // 点击登录按钮，发出信号，等待数据库进行验证
    connect(ui->loginBtn, &QPushButton::clicked, this, [this](){
        QString username = ui->usernameEdit->text();
        if (username.isEmpty()) {
            QMessageBox::information(this, tr("登录提示"), tr("<span style='color: #e74c3c; font-weight: bold;'>❌ 输入的昵称长度至少 1 位，请重新检查！</span>"));
            return ;
        }

        QString pwd = ui->pwdEdit->text();
        if (pwd.length() <= 5) {
            QMessageBox::information(this, tr("登录提示"), tr("<span style='color: #e74c3c; font-weight: bold;'>❌ 输入的密码长度至少 6 位，请重新检查！</span>"));
            return ;
        }

        QString hashedPwd = Tools::encryptPassword(pwd);
        USERINFO loginInfo{ username, hashedPwd, 0 };
        setLoginEnableBtn(false);                       // 暂时关闭按钮可用状态，直到接收到数据库查询结果才恢复
        emit DatabaseManager::instance()->loginRequest(loginInfo);
    });

    // 点击注册按钮，发出信号，让主窗口跳转到 RegisterPage 页面
    connect(ui->registerBtn, &QPushButton::clicked, this, [this](){
        emit toRegisterPage();
    });
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

void LoginWidget::setLoginEnableBtn(bool t) {
    ui->loginBtn->setEnabled(t);
}
