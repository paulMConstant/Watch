#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "Network/client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)

  public:
    explicit MainWindow(QWidget* parent = nullptr) noexcept;
    ~MainWindow() noexcept;

  protected:
    bool eventFilter(QObject* object, QEvent*);

  private slots:
    void hideUI() noexcept;
    void showUI() noexcept;
    void toggleFullscreen() noexcept;

  private:
    Ui::MainWindow* ui;
    Client client;

    QTimer hideUITimer;
    static constexpr auto hideDelayMS {1500};

    void initUI() noexcept;
    void initNetwork() noexcept;
    void initSlotConnections() noexcept;

    void showHideDock(QDockWidget* dock) noexcept;
};

#endif // MAINWINDOW_H
