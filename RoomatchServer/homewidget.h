#ifndef HOMEWIDGET_H
#define HOMEWIDGET_H

#include <QWidget>

namespace Ui { class HomeWidget; }

class HomeWidget : public QWidget {
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);
    ~HomeWidget();

    void setTeacher(const QString &name);

private:
    Ui::HomeWidget *ui;

    QString teacher;
};

#endif // HOMEWIDGET_H
