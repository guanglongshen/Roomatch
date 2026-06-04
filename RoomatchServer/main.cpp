#include "serverwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSettings>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 设置本地基础信息以及配置信息
    QCoreApplication::setOrganizationName("Roomatch");
    QCoreApplication::setApplicationName("Roomatch Server");
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::applicationDirPath());
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "RoomatchServer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    ServerWindow w;
    w.show();
    return QCoreApplication::exec();
}
