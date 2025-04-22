#pragma once

// system/Qt includes
#include <QString>
#include <optional>
#include <set>

namespace os
{
class ApolloApps
{
    Q_DISABLE_COPY(ApolloApps)

public:
    explicit ApolloApps(QString filepath);
    virtual ~ApolloApps() = default;

    std::optional<std::set<QString>> load();

private:
    QString m_filepath;
};
}  // namespace os
