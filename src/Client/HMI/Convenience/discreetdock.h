#ifndef DISCREETDOCK_H
#define DISCREETDOCK_H

#include <QDockWidget>
#include <QCloseEvent>
#include <QTimer>

class DiscreetDock : public QDockWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(DiscreetDock)
  public:
    explicit DiscreetDock(QWidget* parent = nullptr) noexcept;

  protected:
    void closeEvent(QCloseEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    QTimer timer;
    int mouseHoldCount {0};

    static constexpr auto timerCountMS {200};
    static constexpr auto countToShowTopMS {1400};

  private slots:
    void holdCount() noexcept;
};

#endif  // DISCREETDOCK_H
