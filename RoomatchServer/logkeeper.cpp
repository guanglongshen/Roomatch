#include "logkeeper.h"
#include "ui_logkeeper.h"

#include <QDateTime>

LogKeeper::LogKeeper(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LogKeeper) {
    ui->setupUi(this);
    this->setWindowTitle(tr("日志记录"));

    // 视觉效果，将 TreeWidget 的缩进取消
    ui->systemLogTree->setRootIsDecorated(false);
    MAX_LOG_COUNT = 200;
    ui->logCount->setText(QString("0/%1").arg(MAX_LOG_COUNT));
}

LogKeeper::~LogKeeper() {
    delete ui;
}

void LogKeeper::addSystemLog(const QString &event, const QString &statusText, const QString &source) {
    // 获取时间戳
    QString currentTime = QDateTime::currentDateTime().toString("MM-dd-yyyy, hh:mm:ss AP");

    // 创建一个表格行条目
    QTreeWidgetItem *item = new QTreeWidgetItem(ui->systemLogTree);

    // 填充各列内容
    item->setText(0, currentTime);
    item->setText(1, event);
    item->setText(2, statusText);
    item->setText(3, source);

    // 视觉效果
    item->setForeground(0, QBrush(QColor(100, 149, 237)));
    item->setForeground(3, QBrush(QColor(46, 139, 87)));
    QFont sourceFont = item->font(3);
    sourceFont.setBold(true);
    item->setFont(3, sourceFont);

    while (ui->systemLogTree->topLevelItemCount() > MAX_LOG_COUNT) {
        QTreeWidgetItem *deleteItem = ui->systemLogTree->takeTopLevelItem(0);
        delete deleteItem;
    }

    ui->systemLogTree->scrollToItem(item);
    int total = ui->systemLogTree->topLevelItemCount();
    ui->logCount->setText(QString::number(total) + QString("/%1").arg(MAX_LOG_COUNT));
}
