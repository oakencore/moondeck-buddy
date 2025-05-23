// header file include
#include "os/apolloapps.h"

// system/Qt includes
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

// local includes
#include "shared/loggingcategories.h"

namespace
{
#if defined(Q_OS_LINUX)
QString getConfigDir()
{
    const auto xdg_config_env = qgetenv("XDG_CONFIG_HOME");
    if (!xdg_config_env.isEmpty())
    {
        const QDir xdg_cnnfig_dir{xdg_config_env};
        if (xdg_cnnfig_dir.exists())
        {
            return xdg_cnnfig_dir.absolutePath();
        }
    }

    return QDir::cleanPath(QDir::homePath() + "/.config");
}
#endif
}  // namespace

namespace os
{
ApolloApps::ApolloApps(QString filepath)
    : m_filepath{std::move(filepath)}
{
}

// NOLINTNEXTLINE(*-cognitive-complexity)
std::optional<std::set<QString>> ApolloApps::load()
{
    QString filepath{m_filepath};
    if (filepath.isEmpty())  // Fallback to places where we could expect the file to exist
    {
#if defined(Q_OS_WIN)
        const QSettings settings(R"(HKEY_LOCAL_MACHINE\Software\ClassicOldSong\Apollo)", QSettings::NativeFormat);
        filepath = settings.value("Default").toString();
        if (!filepath.isEmpty())
        {
            filepath = QDir::cleanPath(filepath + "/config/apps.json");
        }
#elif defined(Q_OS_LINUX)
        filepath = QDir::cleanPath(getConfigDir() + "/apollo/apps.json");
#else
    #error OS is not supported!
#endif
    }

    qCDebug(lc::os) << "selected filepath for Apollo apps:" << filepath;
    if (filepath.isEmpty())
    {
        qCWarning(lc::os) << "filepath for Apollo apps is empty!";
        return std::nullopt;
    }

    QFile file{filepath};
    if (!file.open(QFile::ReadOnly))
    {
        qCWarning(lc::os) << "file" << filepath << "could not be opened! Reason:" << file.errorString();
        return std::nullopt;
    }

    const auto data{file.readAll()};

    QJsonParseError     parser_error;
    const QJsonDocument json_data{QJsonDocument::fromJson(data, &parser_error)};
    if (json_data.isNull())
    {
        qCWarning(lc::os) << "failed to decode JSON data! Reason:" << parser_error.errorString() << "| data:" << data;
        return std::nullopt;
    }

    qCDebug(lc::os).noquote() << "Apollo apps file content:" << Qt::endl << json_data.toJson(QJsonDocument::Indented);
    if (json_data.isObject())
    {
        const auto json_object = json_data.object();
        const auto apps_it{json_object.find("apps")};
        if (apps_it != json_object.end() && apps_it->isArray())
        {
            std::set<QString> parsed_apps{};

            const auto apps = apps_it->toArray();
            if (apps.isEmpty())
            {
                qCDebug(lc::os) << "there are no Apollo apps to parse.";
                return parsed_apps;
            }

            for (const auto& app : apps)
            {
                if (!app.isObject())
                {
                    qCDebug(lc::os) << "skipping entry as it's not an object:" << app;
                    continue;
                }

                const auto app_object = app.toObject();
                const auto name_it{app_object.find("name")};
                if (name_it == app_object.end())
                {
                    qCDebug(lc::os) << "skipping entry as it does not contain \"name\" field:" << app_object;
                    continue;
                }

                if (!name_it->isString())
                {
                    qCDebug(lc::os) << "skipping entry as the \"name\" field does not contain a string:" << *name_it;
                    continue;
                }

                parsed_apps.insert(name_it->toString());
            }

            qCDebug(lc::os) << "parsed the following Apollo apps:"
                            << QSet<QString>{std::begin(parsed_apps), std::end(parsed_apps)};
            return parsed_apps;
        }
    }

    qCWarning(lc::os) << "file" << m_filepath << "could not be parsed!";
    return std::nullopt;
}
}  // namespace os
