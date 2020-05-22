#include "HMI/Helpers/uncloseabledock.h"

UncloseableDock::UncloseableDock(QWidget* parent) noexcept : QDockWidget(parent)
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

void UncloseableDock::closeEvent(QCloseEvent* event)
{
    event->ignore();
    hide();
    setTitleBarWidget(new QWidget(this));
}

void UncloseableDock::enterEvent(QEvent* event)
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

void UncloseableDock::leaveEvent(QEvent* event)
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

void UncloseableDock::holdCount() noexcept
{
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
