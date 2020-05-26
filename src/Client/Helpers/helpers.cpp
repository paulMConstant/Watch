#include "Helpers/helpers.h"

QString Helpers::String::cleanHtml(QString str) noexcept
{
    str.replace('<', "&lt;");
    str.replace('>', "&gt;");
    return str;
}
