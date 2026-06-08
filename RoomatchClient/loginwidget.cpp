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

    if (ui->onlineTeacherComboBox->findText(displayText) == -1) {
        ui->onlineTeacherComboBox->addItem(displayText, uniqueKey);
    }
}
