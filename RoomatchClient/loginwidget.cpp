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

void LoginWidget::onTeacherDiscovered(const QString &name, const QString &ip, quint16 port) {
    QString displayText = QString("%1(%2)").arg(name, ip);
    QString uniqueKey = QString("%1:%2").arg(ip).arg(port);

    int index = ui->onlineTeacherComboBox->findText(displayText);
    if (index == -1) {
        ui->onlineTeacherComboBox->addItem(displayText, uniqueKey);
    }

    // UI 提醒
    if (ui->onlineTeacherComboBox->count() != 0) {
        ui->onlineTeacherComboBox->setPlaceholderText(tr("下拉选择教师"));
    }
}

void LoginWidget::onTeacherDismissed(const QString &name, const QString &ip, quint16 port) {
    QString displayText = QString("%1(%2)").arg(name, ip);
    QString uniqueKey = QString("%1:%2").arg(ip).arg(port);

    // 得到离线教师的那条消息
    int index = ui->onlineTeacherComboBox->findText(displayText);
    if (index != -1) {
        ui->onlineTeacherComboBox->removeItem(index);
    }

    if (ui->onlineTeacherComboBox->count() == 0) {
        ui->onlineTeacherComboBox->setPlaceholderText(tr("暂无在线教师"));
    }
}
