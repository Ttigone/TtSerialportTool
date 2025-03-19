#include <QApplication>
#include <QTextCodec>
#include "qt-easy-logger-main/logger.h"

#include "storage/setting_manager.h"
#include "window/main_window.h"

// 编译器版本为 6.5.3 时, 调整宽度和高度时, 控件会改变布局, qwindowkit bug
// 编译器 < 6.5.3 or 编译器 > 6.6.2

#if (WIN32)
#include <Windows.h>
// 异常捕获函数
LONG ApplicationCrashHandler(EXCEPTION_POINTERS* pException) {

  EXCEPTION_RECORD* record = pException->ExceptionRecord;
  QString errCode(QString::number(record->ExceptionCode, 16));
  // (uint*)record->ExceptionAddress, errMod;
  QString crashMsg =
      QString("抱歉，软件发生了崩溃，请重启。错误代码：%1，错误地址：%2")
          .arg(errCode);
  // .arg(errAdr);
  qDebug() << crashMsg;

  return EXCEPTION_EXECUTE_HANDLER;
}
#endif

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QCoreApplication::setApplicationName(
      QStringLiteral("SerialportTool-C3H3_Ttigone"));
  QCoreApplication::setOrganizationName(QStringLiteral("WWB-Qt"));

  // 设置全局字体
  QFont font(":/font/roboto/Roboto-Black.ttf", 10);  // 微软雅黑，10号字体
  app.setFont(font);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  // 适用精确缩放
  // bug, 导致 QComboBox 不能正常工作, 同时字体发虚
  // 以下的作用是取消分辨率
  //QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
  //    Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#elif (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

#if defined(Q_OS_WIN) && (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
  // setDarkBorderToWindow(); // 仅在 Windows 下调用
  //const auto osName = QSysInfo::prettyProductName();
  //if (osName.startsWith("Windows 10") || osName.startsWith("Windows 11")) {
  //  // 风格
  //  //QApplication::setStyle("fusion");
  //}
#endif
  // Use Fusion style on Windows 10 & 11. This enables proper dark mode support.
  // See https://www.qt.io/blog/dark-mode-on-windows-11-with-qt-6.5.
  // TODO: Make style configurable, detect -style argument.
#if defined(Q_OS_WIN) && (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
  // const auto osName = QSysInfo::prettyProductName();
  // if (osName.startsWith("Windows 10") || osName.startsWith("Windows 11")) {
  //   QApplication::setStyle("fusion");
  // }
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
  // QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
  //     Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

  QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

  QString filePath = "config.json";
  Storage::SettingsManager& settingsManager =
      Storage::SettingsManager::instance();
  settingsManager.setTargetStoreFile(filePath);
  settingsManager.saveSettings();

#if (WIN32)
  //注冊异常捕获函数
  SetUnhandledExceptionFilter(
      (LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#endif

  // qInstallMessageHandler(h::Logger::messageHandler);  // 启用功能

  Window::MainWindow w;
  w.show();

  return app.exec();
}
