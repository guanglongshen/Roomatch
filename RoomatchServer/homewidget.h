#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>
#include "addproblem.h"

namespace Ui { class HomeWidget; }

class HomeWidget : public QWidget {
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);
    ~HomeWidget();

    void setTeacher(const QString &name);
    AddProblem* getAddproblem() { return problemDialog; }

signals:
    void quitLogin();

private slots:
    void onUpdateStudentStatus(const QString &username, const int &code, const QString &status, const QString &ip);

private:
    Ui::HomeWidget *ui;

    QString teacher;

    void initConn();// 常规信号

    // 添加题目
    AddProblem *problemDialog = nullptr;
};

#endif // HOMEWIDGET_H
