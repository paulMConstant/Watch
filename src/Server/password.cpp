#include "password.h"  // NOLINT[build/include_subdir]

#ifdef WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#include <QTextStream>

#include <Messages/passwordconventions.h>

Password::Password(const QByteArray& password) noexcept :
    password(password)
{
}

bool Password::operator==(const Password& other) const noexcept
{
    return (other.password == password);
}

void Password::readFromUser() noexcept
{
    auto out = QTextStream(stdout);
    for (;;)
    {
        out << "Enter the server password :";
        out.flush();
        auto passwordEntry = readConsole();
        out << "\nEnter the server password again :";
        out.flush();
        if (readConsole() == passwordEntry)
        {
            out << "\nPassword saved.\n";
            password = passwordEntry;
            break;
        }
        out << "\nThe passwords do not match. Try again !\n\n";
        out.flush();
    }
}

void Password::setStdinEcho(bool enable) noexcept
{
#ifdef WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if( !enable )
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode );

#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if( !enable )
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

QByteArray Password::readConsole() noexcept
{
    auto in = QTextStream(stdin);
    setStdinEcho(false);
    auto passwordEntry = PasswordConventions::hash(in.readLine());
    setStdinEcho(true);
    return passwordEntry;
}
