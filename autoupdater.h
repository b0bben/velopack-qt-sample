#ifndef AUTOUPDATER_H
#define AUTOUPDATER_H

#include "Velopack.hpp"
#include <QDebug>
#include <QObject>
#include <QThread>

constexpr auto kUpdateUrl = "https://releases.mollohq.com/";

class UpdateWorker : public QObject {
  Q_OBJECT
public:
  UpdateWorker(Velopack::UpdateManagerSync &existingManager) {
    manager = existingManager;
  }

public Q_SLOTS:
  void downloadUpdate() {
    try {
      // Init Velopack auto-updater with our url
      manager.setUrlOrPath(kUpdateUrl);

      // Check for updates first so we have all the info
      updInfo = manager.checkForUpdates();

      if (updInfo != nullptr) {
        qInfo() << __FUNCTION__ << "Downloading new version: "
                << QString::fromStdString(updInfo->targetFullRelease->version);
        manager.downloadUpdates(updInfo->targetFullRelease.get());

        qInfo() << __FUNCTION__ << "Downloaded version: "
                << QString::fromStdString(updInfo->targetFullRelease->version);
        Q_EMIT updateDownloaded();
      } else {
        qInfo() << __FUNCTION__
                << "Trying to update, even though we don't have a new version! "
                   "This shouldn't happen...";
      }
    } catch (const std::exception &err) {
      qWarning() << __FUNCTION__
                 << "Updating failed with error: " << err.what();
      Q_EMIT updateDownloadFailed();
    }
  }

signals:
  void updateDownloadFailed();
  void updateDownloaded();

private:
  Velopack::UpdateManagerSync manager;
  std::shared_ptr<Velopack::UpdateInfo> updInfo{};
};

class AutoUpdater : public QObject {
  Q_OBJECT

  Q_PROPERTY(
      QString currentUpdateChannel READ currentUpdateChannel WRITE
          setCurrentUpdateChannel NOTIFY currentUpdateChannelChanged FINAL)
  Q_PROPERTY(QString currentVersion READ currentVersion WRITE setCurrentVersion
                 NOTIFY currentVersionChanged FINAL)
  Q_PROPERTY(
      bool updateReadyToInstall READ updateReadyToInstall WRITE
          setUpdateReadyToInstall NOTIFY updateReadyToInstallChanged FINAL)
  Q_PROPERTY(QString newVersion READ newVersion WRITE setNewVersion NOTIFY
                 newVersionChanged FINAL)

public:
  explicit AutoUpdater(QObject *parent = nullptr);
  ~AutoUpdater() override;

public:
  static AutoUpdater &instance() {
    static AutoUpdater _instance;
    return _instance;
  }
  bool updateReadyToInstall() const;
  void setUpdateReadyToInstall(bool newUpdateReady);

  QString currentUpdateChannel() const;
  void setCurrentUpdateChannel(const QString &newCurrentUpdateChannel);

  QString currentVersion() const;
  void setCurrentVersion(const QString &newCurrentVersion);

  QString newVersion() const;
  void setNewVersion(const QString &newNewVersion);

  Q_INVOKABLE void applyUpdateAndRestart();
  Q_INVOKABLE void checkForUpdates();

signals:
  void updateReadyToInstallChanged();
  void currentUpdateChannelChanged();
  void currentVersionChanged();
  void newVersionChanged();
  void updatingFailed();

private:
  QString m_currentUpdateChannel;
  QString m_currentVersion;
  QString m_newVersion;
  bool m_updateReadyToInstall = false;
  bool m_updateDownloaded = false;
  Velopack::UpdateManagerSync manager{};
  std::shared_ptr<Velopack::UpdateInfo> updInfo{};

  QThread workerThread;
  UpdateWorker worker;
};

#endif // AUTOUPDATER_H
