#ifndef VIDEOCLIPER_H
#define VIDEOCLIPER_H

#include <QObject>
#include <QAbstractVideoSurface>
#include <QGraphicsPixmapItem>

class VideoCliper : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    explicit VideoCliper(QObject *parent = nullptr);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;
    bool present(const QVideoFrame &frame);

    QGraphicsPixmapItem *canvas() const;
    void setCanvas(QGraphicsPixmapItem *canvas);

signals:
    void frameAvailable(QImage frame);      // everyFrame

public slots:

private:
    QGraphicsPixmapItem *m_canvas;          // The canvas to put Frame
    int m_counter;                          // 计数器

};

#endif // VIDEOCLIPER_H
