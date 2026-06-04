#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

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
};
#endif // SERVERWINDOW_H
