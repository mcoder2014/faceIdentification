#include "videocliper.h"

#include <QAbstractVideoBuffer>
#include <QVideoFrame>
#include <QDebug>
#include <QPixmap>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

VideoCliper::VideoCliper(QObject *parent) : QAbstractVideoSurface(parent)
{
    // init
    this->m_canvas = nullptr;       // initialize as null pointer
    this->m_counter = 0;            // 计数器置零
}

///
/// \brief VideoCliper::supportedPixelFormats
/// \param handleType
/// \return
///
QList<QVideoFrame::PixelFormat> VideoCliper::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    return QList<QVideoFrame::PixelFormat>()
        << QVideoFrame::Format_ARGB32
        << QVideoFrame::Format_ARGB32_Premultiplied
        << QVideoFrame::Format_RGB32
        << QVideoFrame::Format_RGB24
        << QVideoFrame::Format_RGB565
        << QVideoFrame::Format_RGB555
        << QVideoFrame::Format_ARGB8565_Premultiplied
        << QVideoFrame::Format_BGRA32
        << QVideoFrame::Format_BGRA32_Premultiplied
        << QVideoFrame::Format_BGR32
        << QVideoFrame::Format_BGR24
        << QVideoFrame::Format_BGR565
        << QVideoFrame::Format_BGR555
        << QVideoFrame::Format_BGRA5658_Premultiplied
        << QVideoFrame::Format_AYUV444
        << QVideoFrame::Format_AYUV444_Premultiplied
        << QVideoFrame::Format_YUV444
        << QVideoFrame::Format_YUV420P
        << QVideoFrame::Format_YV12
        << QVideoFrame::Format_UYVY
        << QVideoFrame::Format_YUYV
        << QVideoFrame::Format_NV12
        << QVideoFrame::Format_NV21
        << QVideoFrame::Format_IMC1
        << QVideoFrame::Format_IMC2
        << QVideoFrame::Format_IMC3
        << QVideoFrame::Format_IMC4
        << QVideoFrame::Format_Y8
        << QVideoFrame::Format_Y16
        << QVideoFrame::Format_Jpeg
        << QVideoFrame::Format_CameraRaw
        << QVideoFrame::Format_AdobeDng;
}

///
/// \brief VideoCliper::present
/// \param frame
/// \return bool
///
bool VideoCliper::present(const QVideoFrame &frame)
{
//    qDebug() << "video cliper";
    if(this->m_canvas == nullptr)
    {
        qDebug() << "m_canvas is nullptr!";
        return false;
    }

    if (frame.isValid())
    {
        QVideoFrame cloneFrame(frame);
        if(!cloneFrame.map(QAbstractVideoBuffer::ReadOnly))
        {
            qDebug() << "map false";
            return false;
        }

//        QImage image(cloneFrame.bits(),
//                     cloneFrame.width(),
//                     cloneFrame.height(),
//                     cloneFrame.bytesPerLine(),
//                     QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));
        // 直接拷贝QImage会出现画面倒置情况，推测是 bits顺序相反导致的，
        // 借助opencv倒转图片，并实现深拷贝
        cv::Mat mat;
        if(cloneFrame.pixelFormat() == QVideoFrame::Format_RGB32)
        {
            mat = cv::Mat(
                        cloneFrame.height(),
                        cloneFrame.width(),
                        CV_8UC4,
                        (void*)cloneFrame.bits(),
                        (size_t)cloneFrame.bytesPerLine());
        }

        // 旋转 180 度
        flip(mat,mat,0);
        flip(mat,mat,1);

        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, (int)mat.step, QImage::Format_ARGB32);

//        qDebug() << "Frame width " << cloneFrame.width()
//                 << " height " << cloneFrame.height()
//                 << " pixelFormat " << cloneFrame.pixelFormat();

        // 判断是由于共享内存区域，导致内存被释放
        QImage deepCopy = image.copy();
        m_canvas->setPixmap(QPixmap::fromImage(deepCopy));  // update canvas

        this->m_counter ++;
        if(this->m_counter / 10 ==0)
        {
            // 降低发送帧的频率，减少卡顿
            this->m_counter = 0;    // 计数器置零
            emit frameAvailable(deepCopy);
        }
        // emit frameAvailable(deepCopy);

        cloneFrame.unmap();
//        qDebug() << "present finished!";

        return true;
    }

    return false;

}

QGraphicsPixmapItem *VideoCliper::canvas() const
{
    return m_canvas;
}

void VideoCliper::setCanvas(QGraphicsPixmapItem *canvas)
{
    m_canvas = canvas;
}
