#ifndef VIDEOCLIPER_H
#define VIDEOCLIPER_H

#include <QObject>
#include <QAbstractVideoSurface>
#include <QGraphicsPixmapItem>
#include <QTime>

class VideoCliper : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit VideoCliper(QObject *parent = nullptr);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;
    bool present(const QVideoFrame &frame);

    QGraphicsPixmapItem *canvas() const;
    void setCanvas(QGraphicsPixmapItem *canvas);

    int frameInterval() const;
    void setFrameInterval(int frameInterval);

signals:
    void frameAvailable(QImage frame);      // everyFrame

public slots:

private:
    QGraphicsPixmapItem *m_canvas;          // The canvas to put Frame
    QTime m_time;                           // 计时器
    int m_frameInterval;                    // 发送帧频率, ms为单位

};

#endif // VIDEOCLIPER_H
