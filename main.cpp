#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "autoupdater.h"

void noMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  Q_UNUSED(type);
  Q_UNUSED(context);
  Q_UNUSED(msg);
}

int main(int argc, char *argv[]) {

  try {
    // Init Velopack hooks, MUST be as early as possible in main()
    // Velopack may exit / restart our app at this statement
    Velopack::startup(argv, argc);
  } catch (const std::exception &err) {
    qInfo() << "Error initating auto-updater, msg: " << err.what();
  }

  QGuiApplication app(argc, argv);

  if (!app.arguments().contains(u"--with-debug"_qs)) {
    qInstallMessageHandler(noMessageOutput);
  }

  // Init Auto Updater as a singleton
  qmlRegisterSingletonInstance("VelopackQt", 1, 0, "AutoUpdater",
                               &AutoUpdater::instance());

  QQmlApplicationEngine engine;
  const QUrl url(u"qrc:/VelopackQtSample/Main.qml"_qs);
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
          QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
