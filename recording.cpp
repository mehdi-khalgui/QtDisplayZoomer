#include "recording.h"

#include <QDebug>
#include <QtWidgets>

#include "imageutil.h"
using namespace imageutil;

/**
 * @brief Recording::Recording
 */
Recording::Recording()
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint | Qt::CustomizeWindowHint);
    screenGeometry = QApplication::desktop()->geometry();
    zoomArea = screenGeometry;
    currentZoom = 1.0;

    screenshotLabel = new QLabel(this);
    //    screenshotLabel->setMargin(-3);
    screenshotLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    screenshotLabel->setAlignment(Qt::AlignCenter);
    screenshotLabel->setEnabled(true);
    connect(screenshotLabel, SIGNAL(wheelEvent()), this, SLOT(wheelEvent()));
    connect(screenshotLabel, SIGNAL(mousePressEvent()), this, SLOT(mousePressEvent()));
    connect(screenshotLabel, SIGNAL(mouseMoveEvent()), this, SLOT(mouseMoveEvent()));

    screenshotLabel->setMinimumSize(screenGeometry.width() / 2, screenGeometry.height() / 2);
}

/**
 * @brief Recording::shootScreen
 */
void Recording::shootScreen()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (const QWindow *window = windowHandle()) screen = window->screen();
    if (!screen)
    {
        qDebug() << "Failed to get screen";
        return;
    }

    //    QPixmap originalPixmap = takeScreenShot(area.toRect());
    QPixmap originalPixmap = screen->grabWindow(0).copy(zoomArea.toRect());

    // Draw the mouse (because grab window doesn't capture the mouse)
    QPoint p = QCursor::pos();
    p.rx() -= zoomArea.left();
    p.ry() -= zoomArea.top();
    QPainter painter(&originalPixmap);
    painter.setBrush(QColor(255, 0, 0, 100));
    QPolygon myPolygon;
    myPolygon.putPoints(0, 1, p.x(), p.y());
    myPolygon.putPoints(1, 1, p.x() + 20, p.y() + 10);
    myPolygon.putPoints(2, 1, p.x() + 10, p.y() + 20);
    painter.drawPolygon(myPolygon, Qt::WindingFill);
    painter.end();

    screenshotLabel->setPixmap(originalPixmap.scaled(screenshotLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

/**
 * @brief Recording::wheelEvent
 * @param event
 */
void Recording::wheelEvent(QWheelEvent *event)
{
    qreal newZomm = qBound(0.1, currentZoom - double(event->angleDelta().y()) / 1200.0, 1.0);
    if (newZomm != currentZoom)
    {
        currentZoom = newZomm;

        // Compute the pos ratio of the mouse in displaying window
        QPointF p = screenshotLabel->mapFromGlobal(QCursor::pos());
        qreal px = p.x() / qreal(screenshotLabel->width());
        qreal py = p.y() / qreal(screenshotLabel->height());

        // Compute the real position of the mouse
        qreal zoomCenterX = zoomArea.left() + zoomArea.width() * px;
        qreal zoomCenterY = zoomArea.top() + zoomArea.height() * py;

        // Compute the new dimension of the zoom window
        qreal width = currentZoom * screenGeometry.width();
        qreal height = currentZoom * screenGeometry.height();

        // Compute the new position of the new window to have the same position of the mouse
        qreal left = qBound(0.0, zoomCenterX - px * width, screenGeometry.width() - width);
        qreal top = qBound(0.0, zoomCenterY - py * height, screenGeometry.height() - height);

        zoomArea.setLeft(left);
        zoomArea.setTop(top);
        zoomArea.setWidth(width);
        zoomArea.setHeight(height);
    }
}

/**
 * @brief Recording::mousePressEvent
 * @param event
 */
void Recording::mousePressEvent(QMouseEvent *event)
{
    lastMousePos = event->screenPos();
    if (event->buttons() & Qt::LeftButton)
        buttonPressed = Qt::LeftButton;
    else if (event->buttons() & Qt::RightButton)
        buttonPressed = Qt::RightButton;
}

/**
 * @brief Recording::mouseMoveEvent
 * @param event
 */
void Recording::mouseMoveEvent(QMouseEvent *event)
{
    QPointF vect = (event->screenPos() - lastMousePos);

    if (buttonPressed == Qt::LeftButton)
    {
        // Compute relative moving
        qreal dx = vect.x() / qreal(screenshotLabel->width()) * zoomArea.width();
        qreal dy = vect.y() / qreal(screenshotLabel->height()) * zoomArea.height();

        // Compute new position or zoom window
        qreal left = qBound(0.0, zoomArea.x() - dx, screenGeometry.width() - zoomArea.width());
        qreal top = qBound(0.0, zoomArea.y() - dy, screenGeometry.height() - zoomArea.height());

        if (zoomArea.left() != left) lastMousePos.setX(event->screenPos().x());
        if (zoomArea.top() != top) lastMousePos.setY(event->screenPos().y());

        zoomArea.moveTo(left, top);
    }
    else if (buttonPressed == Qt::RightButton)
    {
        // Compute the new position of the new window to have the same position of the mouse
        qreal left = qBound(0.0, this->geometry().x() + vect.x(), screenGeometry.width() - screenshotLabel->width());
        qreal top = qBound(0.0, this->geometry().y() + vect.y(), screenGeometry.height() - screenshotLabel->height());

        if (this->geometry().left() != left) lastMousePos.setX(event->screenPos().x());
        if (this->geometry().top() != top) lastMousePos.setY(event->screenPos().y());

        QRect newGeometry = this->geometry();
        newGeometry.setLeft(left);
        newGeometry.setTop(top);
        this->setGeometry(left, top, this->geometry().width(), this->geometry().height());
    }
}
