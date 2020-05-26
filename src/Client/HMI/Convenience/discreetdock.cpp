#include "HMI/Convenience/discreetdock.h"

DiscreetDock::DiscreetDock(QWidget* parent) noexcept : QDockWidget(parent)
{
    setTitleBarWidget(new QWidget(this));
    timer.setInterval(timerCountMS);
    connect(&timer, SIGNAL(timeout()), this, SLOT(holdCount()));
    connect(this, &QDockWidget::topLevelChanged, [this]()
    {
        if (isFloating())
        {
            setTitleBarWidget(nullptr);
        }
    });
}

void DiscreetDock::closeEvent(QCloseEvent* event)
{
    event->ignore();
    hide();
    setTitleBarWidget(new QWidget(this));
}

void DiscreetDock::enterEvent(QEvent* event)
{
    event->ignore();
    if (isFloating())
    {
        timer.stop();
        return;
    }
    timer.start();
    mouseHoldCount = 0;
}

void DiscreetDock::leaveEvent(QEvent* event)
{
    event->ignore();
    if (isFloating())
    {
        timer.stop();
        return;
    }
    timer.stop();
    setTitleBarWidget(new QWidget(this));
}

void DiscreetDock::holdCount() noexcept
{
    constexpr auto nbCountToShowTop {countToShowTopMS / timerCountMS};
    constexpr auto topPortionOfWidget {35};

    const auto cursorPos {mapFromGlobal(QCursor::pos())};
    if (cursorPos.y() < topPortionOfWidget
            && cursorPos.x() >= 0
            && cursorPos.x() <= width())
    {
        if (++mouseHoldCount >= nbCountToShowTop)
        {
            setTitleBarWidget(nullptr);
        }
    }
    else if (mouseHoldCount > 0)
    {
        mouseHoldCount = 0;
        setTitleBarWidget(new QWidget(this));
    }
}
