#include "autoupdater.h"

#include <QString>
#include <QtDebug>

AutoUpdater::AutoUpdater(QObject *parent) : QObject(parent), worker(manager) {

  connect(&worker, &UpdateWorker::updateDownloaded, this, [=]() {
    // We have downloaded a new update, notify UI
    qDebug() << __FUNCTION__ << "We have downloaded a new update, notify UI";
    setNewVersion(QString::fromStdString(updInfo->targetFullRelease->version));
    setUpdateReadyToInstall(true);
  });

  worker.moveToThread(&workerThread);
  connect(&workerThread, &QThread::finished, &worker, &QObject::deleteLater);
  workerThread.start();

  // Do a check on startup
  checkForUpdates();
}

AutoUpdater::~AutoUpdater() {
  workerThread.quit();
  workerThread.wait(200);
};

void AutoUpdater::checkForUpdates() {

  try {
    // Init Velopack auto-updater
    manager.setUrlOrPath(kUpdateUrl);

    qInfo() << "Current version: " << manager.getCurrentVersion();

    // Initial check for updates on startup
    updInfo = manager.checkForUpdates();

    if (updInfo == nullptr) {
      // no updates available
      qInfo() << "No updates available, running latest version \\o/";
    } else {
      setNewVersion(
          QString::fromStdString(updInfo->targetFullRelease->version));
      qInfo() << "Update available: " << newVersion()
              << ", starting download in background...";

      // Call the worker method which lives in another thread
      QMetaObject::invokeMethod(&worker, "downloadUpdate",
                                Qt::QueuedConnection);
    }
  } catch (const std::exception &err) {
    qInfo() << "Error initating auto-updater, msg: " << err.what();
  }
}

void AutoUpdater::applyUpdateAndRestart() {
  if (!updateReadyToInstall()) {
    return;
  }

  try {
    if (updInfo != nullptr) {
      qInfo() << __FUNCTION__ << "Downloading and installing new update: "
              << QString::fromStdString(updInfo->targetFullRelease->version);
      // We should now have the package downloaded, so update and restart the
      // app
      manager.applyUpdatesAndRestart(updInfo->targetFullRelease.get());
    } else {
      qInfo() << __FUNCTION__
              << "Trying to update, even tho we don't have a new version! This "
                 "shouldn't happen...";
    }
  } catch (const std::exception &err) {
    qWarning() << __FUNCTION__ << "Updating failed with error: " << err.what();
    Q_EMIT updatingFailed();
  }
}

bool AutoUpdater::updateReadyToInstall() const {
  return m_updateReadyToInstall;
}

void AutoUpdater::setUpdateReadyToInstall(bool newUpdateReady) {
  if (m_updateReadyToInstall == newUpdateReady)
    return;
  m_updateReadyToInstall = newUpdateReady;
  Q_EMIT updateReadyToInstallChanged();
}

QString AutoUpdater::currentUpdateChannel() const {
  return m_currentUpdateChannel;
}

void AutoUpdater::setCurrentUpdateChannel(
    const QString &newCurrentUpdateChannel) {
  if (m_currentUpdateChannel == newCurrentUpdateChannel)
    return;
  m_currentUpdateChannel = newCurrentUpdateChannel;
  Q_EMIT currentUpdateChannelChanged();
}

QString AutoUpdater::currentVersion() const { return m_currentVersion; }

void AutoUpdater::setCurrentVersion(const QString &newCurrentVersion) {
  if (m_currentVersion == newCurrentVersion)
    return;
  m_currentVersion = newCurrentVersion;
  Q_EMIT currentVersionChanged();
}

QString AutoUpdater::newVersion() const { return m_newVersion; }

void AutoUpdater::setNewVersion(const QString &newNewVersion) {
  if (m_newVersion == newNewVersion)
    return;
  m_newVersion = newNewVersion;
  Q_EMIT newVersionChanged();
}
