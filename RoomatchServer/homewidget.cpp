#include "homewidget.h"
#include "networkmanager.h"
#include "ui_homewidget.h"

HomeWidget::HomeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomeWidget) {
    ui->setupUi(this);
    // 视觉效果
    ui->contestTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents); // 自动适配内容大小
    ui->contestTree->setRootIsDecorated(false);                                     // 不折叠
    ui->contestTree->setIndentation(0);
    ui->onlineTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->onlineTree->setRootIsDecorated(false);
    ui->onlineTree->setIndentation(0);                                              // 无缩进

    ui->contestModeList->setCurrentRow(0);

    if (problemDialog == nullptr) {
        problemDialog = new AddProblem(this);
    }

    initConn();
}

HomeWidget::~HomeWidget() {
    if (problemDialog) {
        delete problemDialog;
        problemDialog = nullptr;
    }

    delete ui;
}

void HomeWidget::setTeacher(const QString &name) {
    teacher = name;
    ui->teacherLabel->setText(teacher);
}

void HomeWidget::onUpdateStudentStatus(const QString &username, const int &code, const QString &status, const QString &ip) {
    // 查询是否存在此学生
    QList<QTreeWidgetItem*> stu = ui->onlineTree->findItems(username, Qt::MatchExactly, 0);

    if (code == 1) {    // 1 在线
        if (stu.isEmpty()) {  // 新上线
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->onlineTree);
            item->setText(0, username);
            item->setText(1, status);
            item->setText(2, ip);
        } else {
            stu.first()->setText(1, tr("在线"));
            stu.first()->setText(2, ip);
        }
    } else if (code == 0) {     // 离线
        if (!stu.isEmpty()) {   // 找到了这个学生，将这个学生从 QTreeWidget 中删除即可
            delete stu.first();
        }
    }
}

void HomeWidget::initConn() {
    // 退出登录按键
    connect(ui->quitBtn, &QPushButton::clicked, this, &HomeWidget::quitLogin);

    // 学生状态更新的信号
    connect(NetworkManager::instance()->getWorker(), &NetworkWorker::studentStatusChanged, this, &HomeWidget::onUpdateStudentStatus);

    // 点击添加题目
    connect(ui->addProblemBtn, &QPushButton::clicked, this, [this](){
        problemDialog->setWindowTitle(tr("添加题目"));
        problemDialog->exec();
    });
}

