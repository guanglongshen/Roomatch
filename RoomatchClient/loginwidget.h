#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui { class LoginWidget; }

class LoginWidget : public QWidget {
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

public slots:
    void onTeacherDiscovered(const QString &name, const QString &ip, quint16 port);
    void onTeacherDismissed(const QString &name, const QString &ip, quint16 port);

private slots:



private:
    Ui::LoginWidget *ui;


};

#endif // LOGINWIDGET_H
