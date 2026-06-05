#ifndef LOGKEEPER_H
#define LOGKEEPER_H

#include <QDialog>

namespace Ui { class LogKeeper; }

class LogKeeper : public QDialog {
    Q_OBJECT

public:
    explicit LogKeeper(QWidget *parent = nullptr);
    ~LogKeeper();

    void addSystemLog(const QString &event, const QString &statusText, const QString &source);

private:
    Ui::LogKeeper *ui;
    int MAX_LOG_COUNT;
};

#endif // LOGKEEPER_H
