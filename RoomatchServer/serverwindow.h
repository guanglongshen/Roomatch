#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include "logkeeper.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerWindow; }
QT_END_NAMESPACE

class ServerWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow() override;

private:
    Ui::ServerWindow *ui;
    LogKeeper *logKeeper = nullptr;

    void initDatabase();
    void initConnection();
    void initNet();
    void initStackedWidget();
    void smoothChangePage(QWidget *OLD, QWidget *NEW);
};
#endif // SERVERWINDOW_H
