#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>

namespace Ui { class RegisterWidget; }

class RegisterWidget : public QWidget {
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

signals:
    void toLoginPage();

private:
    Ui::RegisterWidget *ui;

    void rewriteForm();
    void hideTip();
};

#endif // REGISTERWIDGET_H
