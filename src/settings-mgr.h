#ifndef SEAFILE_CLIENT_SETTINGS_MANAGER_H
#define SEAFILE_CLIENT_SETTINGS_MANAGER_H

#include <QObject>

/**
 * Settings Manager handles seafile client user settings & preferences
 */
class SettingsManager : public QObject {
    Q_OBJECT

public:
    SettingsManager();

    void loadSettings();
    void setAutoSync(bool);

    bool autoSync() { return auto_sync_; }

    bool notify() { return bubbleNotifycation_; }
    bool autoStart() { return autoStart_; }
    bool encryptTransfer() { return transferEncrypted_; }
    unsigned int maxDownloadRatio() { return maxDownloadRatio_; }
    unsigned int maxUploadRatio() { return maxUploadRatio_; }
    bool allowInvalidWorktree() { return allow_invalid_worktree_; }

    void setNotify(bool notify);
    void setAutoStart(bool autoStart);
    void setEncryptTransfer(bool encrypted);
    void setMaxDownloadRatio(unsigned int ratio);
    void setMaxUploadRatio(unsigned int ratio);
    void setAllowInvalidWorktree(bool val);

    bool hideMainWindowWhenStarted();
    void setHideMainWindowWhenStarted(bool hide);

    void setCheckLatestVersionEnabled(bool enabled);
    bool isCheckLatestVersionEnabled();
    // bool defaultLibraryAlreadySetup();
    // void setDefaultLibraryAlreadySetup();

public:

    // Remove all settings from system when uninstall
    static void removeAllSettings();

private:
    Q_DISABLE_COPY(SettingsManager)

    bool auto_sync_;
    bool bubbleNotifycation_;
    bool autoStart_;
    bool transferEncrypted_;
    bool allow_invalid_worktree_;
    unsigned int maxDownloadRatio_;
    unsigned int maxUploadRatio_;
};

#endif // SEAFILE_CLIENT_SETTINGS_MANAGER_H
