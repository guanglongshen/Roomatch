#include "registerwidget.h"
#include "ui_registerwidget.h"
#include "tools.h"

#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTimer>

RegisterWidget::RegisterWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegisterWidget) {
    ui->setupUi(this);

    // 注册页面的 errorLable 需要隐藏
    hideTip();

    // 姓名，中文、英文、数字，1~16 位
    QRegularExpression nameReg("^[a-zA-Z0-9\\p{Han}]{1,16}$");
    QRegularExpressionValidator *nameValidator = new QRegularExpressionValidator(nameReg, this);
    ui->usernameEdit->setValidator(nameValidator);

    // 密码 6~16 位，不包含空格、反斜杠
    QRegularExpression codeReg("^[^\\s\\\\]{6,16}$");
    QRegularExpressionValidator *codeValidator = new QRegularExpressionValidator(codeReg, this);
    ui->pwdEdit->setValidator(codeValidator);
    ui->pwdConfirmEdit->setValidator(codeValidator);

    // 返回登录页面
    connect(ui->backBtn, &QPushButton::clicked, this, [this](){
        emit toLoginPage();
    });

    // 清空所有输入
    connect(ui->clearBtn, &QPushButton::clicked, this, [this](){
        ui->usernameEdit->clear();
        ui->pwdEdit->clear();
        ui->pwdConfirmEdit->clear();
    });

    rewriteForm();

    // 提交注册按钮
    connect(ui->registerBtn, &QPushButton::clicked, this, [this](){
        QString username = ui->usernameEdit->text();
        if (username.isEmpty()) {
            ui->tipLabel->setText("<span style='color: #e74c3c; font-weight: bold;'>❌ 昵称未填写，请输入您的真实姓名！</span>");
            ui->tipLabel->show();
            return ;
        }

        QString pwd = ui->pwdEdit->text();
        QString confirm = ui->pwdConfirmEdit->text();
        if (pwd.length() <= 5 || confirm.length() <= 5) {
            ui->tipLabel->setText("<span style='color: #e74c3c; font-weight: bold;'>❌ 输入的密码长度至少 6 位，请重新检查！</span>");
            ui->tipLabel->show();
            return ;
        }
        if (pwd != confirm) {
            ui->tipLabel->setText("<span style='color: #e74c3c; font-weight: bold;'>❌ 两次输入的密码不一致，请重新检查！</span>");
            ui->tipLabel->show();
            return ;
        }

        ui->tipLabel->setText("<span style='color: #32cd32; font-weight: bold;'>🟢 正在提交注册信息，请稍后...</span>");
        ui->tipLabel->show();
        ui->registerBtn->setEnabled(false);

        QString hashedPwd = Tools::encryptPassword(pwd);
    });
}

RegisterWidget::~RegisterWidget() {
    delete ui;
}

// 重新填表时，将 errorLabel 关闭
void RegisterWidget::rewriteForm() {
    connect(ui->usernameEdit, &QLineEdit::textChanged, this, &RegisterWidget::hideTip);
    connect(ui->pwdEdit, &QLineEdit::textChanged, this, &RegisterWidget::hideTip);
    connect(ui->pwdConfirmEdit, &QLineEdit::textChanged, this, &RegisterWidget::hideTip);
}

// 隐藏 tipLabel
void RegisterWidget::hideTip() {
    ui->tipLabel->hide();
}
