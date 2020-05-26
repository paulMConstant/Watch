#ifndef PASSWORD_H
#define PASSWORD_H

#include <QByteArray>

class Password
{
  public:
    [[nodiscard]] bool equals(const QByteArray& other) noexcept;
    void readFromUser() noexcept;

  private:
    QByteArray password;

    [[nodiscard]] QByteArray readConsole() noexcept;
    void setStdinEcho(bool enable) noexcept;
};

#endif  // PASSWORD_H
