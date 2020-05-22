#ifndef UNCLOSEABLE_DOCK_H
#define UNCLOSEABLE_DOCK_H

#include <QDockWidget>
#include <QCloseEvent>
#include <QTimer>

class UncloseableDock : public QDockWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(UncloseableDock)
  public:
    explicit UncloseableDock(QWidget* parent = nullptr) noexcept;

  protected:
    void closeEvent(QCloseEvent* event);
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    QTimer timer;
    int mouseHoldCount {0};

    static constexpr auto timerCountMS {200};
    static constexpr auto countToShowTopMS {1400};
    static constexpr auto nbCountToShowTop {countToShowTopMS / timerCountMS};
    static constexpr auto topPortionOfWidget {35};

  private slots:
    void holdCount() noexcept;
};

#endif  // UNCLOSEABLE_DOCK_H
