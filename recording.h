#ifndef RECORDING_H
#define RECORDING_H

#include <QLabel>
#include <QWidget>

class Recording : public QWidget
{
    Q_OBJECT

public:
    Recording();

    void startRecording();
    void stopRecording();
    void shootScreen();

private slots:
    void wheelEvent(QWheelEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QLabel *screenshotLabel;
    qreal currentZoom;
    QRectF screenGeometry;
    QRectF zoomArea;
    QPointF lastMousePos;
    Qt::MouseButton buttonPressed;
};

#endif // RECORDING_H
