#ifndef GLOBAL_H
#define GLOBAL_H

#include <QSettings>

namespace Global
{
    void setupIcons() noexcept;
    [[nodiscard]] QSettings appSettings() noexcept;

    static constexpr auto appName {"Watch"};
    static constexpr auto organizationName {"Paul Constant"};
}

#endif // GLOBAL_H
