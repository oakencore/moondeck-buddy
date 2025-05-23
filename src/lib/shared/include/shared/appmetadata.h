#pragma once

// Qt includes
#include <QObject>
#include <QString>

namespace shared
{
class AppMetadata final : public QObject
{
public:
    enum class App
    {
        Buddy,
        Stream
    };

    explicit AppMetadata(App app);
    ~AppMetadata();

    QString getAppName() const;
    QString getAppName(App app) const;

    QString getLogDir() const;
    QString getLogName() const;
    QString getLogPath() const;

    QString getSettingsDir() const;
    QString getSettingsName() const;
    QString getSettingsPath() const;

    QString getAutoStartDir() const;
    QString getAutoStartName() const;
    QString getAutoStartPath() const;
    QString getAutoStartExec() const;

    QString getDefaultSteamExecutable() const;

private:
    App m_current_app;
};
}  // namespace shared