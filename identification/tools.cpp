#include "tools.h"

#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QRgb>

//float maximum(float *p, int length)
//{
//    int max = p[0];
//    for(int i = 1; i < length; i++)
//    {
//        if(p[i]>max)
//        {
//            max = p[i];
//        }
//    }
//}

QImage cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat QImage2cvMat(QImage image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(
                    image.height(),
                    image.width(),
                    CV_8UC4,
                    (void*)image.constBits(),
                    image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(
                    image.height(),
                    image.width(),
                    CV_8UC3,
                    (void*)image.constBits(),
                    image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGBA);
        break;
    case QImage::Format_Indexed8:
//    case QImage::Format_Grayscale8:
        mat = cv::Mat(
                image.height(),
                image.width(),
                CV_8UC1,
                (void*)image.constBits(),
                image.bytesPerLine());
        break;
    }
    return mat;
}

///
/// \brief toArray2d
/// \param pixmap
/// \return
/// 转换为 Array2d 格式数据结构
///
dlib::array2d<dlib::rgb_pixel> *toArray2d(const QPixmap &pixmap)
{
    QImage image = pixmap.toImage();

    int width = image.width();
    int height = image.height();

    dlib::array2d<dlib::rgb_pixel> *array_image =
            new dlib::array2d<dlib::rgb_pixel>(height, width);
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            dlib::rgb_pixel pixel;
            QRgb color = image.pixel(x, y);
            pixel.red = (uchar)qRed(color);
            pixel.blue = (uchar)qBlue(color);
            pixel.green = (uchar)qGreen(color);

            (*array_image)[y][x] = pixel;
        }
    }

    return array_image;

}

/**
 * @brief toArray2d
 * @param image
 * @return
 */
dlib::array2d<dlib::rgb_pixel> *toArray2d(const QImage &image)
{
    int width = image.width();
    int height = image.height();

    dlib::array2d<dlib::rgb_pixel> *array_image =
            new dlib::array2d<dlib::rgb_pixel>(height, width);
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            dlib::rgb_pixel pixel;
            QRgb color = image.pixel(x, y);
            pixel.red = (uchar)qRed(color);
            pixel.blue = (uchar)qBlue(color);
            pixel.green = (uchar)qGreen(color);

            (*array_image)[y][x] = pixel;
        }
    }

    return array_image;
}

///
/// \brief toArray2d
/// \param image
/// \return
///
dlib::array2d<dlib::rgb_pixel> *toArray2d(const Mat &image)
{
    int width = image.cols;
    int height = image.rows;

    dlib::array2d<dlib::rgb_pixel> *array_image =
            new dlib::array2d<dlib::rgb_pixel>(height, width);

    if(image.type() == CV_8UC1)
    {
        for(int x = 0; x < width; x++)
        {
            for(int y = 0; y < height; y++)
            {
                dlib::rgb_pixel pixel;
                // 获得 Mat 对应点像素
                pixel.red = image.at<char>(y,x);
                pixel.green = pixel.red;
                pixel.blue = pixel.red;

                (*array_image)[y][x] = pixel;
            }
        }
    }
    else if(image.type() == CV_8UC3)
    {
        for(int x = 0; x < width; x++)
        {
            for(int y = 0; y < height; y++)
            {
                dlib::rgb_pixel pixel;
                // 获得 Mat 对应点像素
                Vec3b mat_pixel = image.at<Vec3b>(y,x);
                pixel.red = mat_pixel[2];
                pixel.green = mat_pixel[1];
                pixel.blue = mat_pixel[0];

                (*array_image)[y][x] = pixel;
            }
        }
    }
    else if(image.type() == CV_8UC4)
    {
        qDebug() << "toArray2d CV_8UC4";
        for(int x = 0; x < width; x++)
        {
            for(int y = 0; y < height; y++)
            {
                dlib::rgb_pixel pixel;
                // 获得 Mat 对应点像素
                Vec4b mat_pixel = image.at<Vec4b>(y,x);
                pixel.red = mat_pixel[2];
                pixel.green = mat_pixel[1];
                pixel.blue = mat_pixel[0];

                (*array_image)[y][x] = pixel;
            }
        }
    }

    return array_image;
}
