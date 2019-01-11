#ifndef TOOLS_H
#define TOOLS_H

#include <QImage>
#include <QVector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <dlib/image_processing.h>
#include <dlib/image_io.h>


using namespace cv;

//float maximum(float *p, int length);        // 找出最大值
QImage cvMat2QImage(const cv::Mat& mat);    // 转换格式
cv::Mat QImage2cvMat(QImage image);         // 转换格式
dlib::array2d<dlib::rgb_pixel> *toArray2d(const QPixmap &pixmap);  // QPixmap 转换为array2d
dlib::array2d<dlib::rgb_pixel> *toArray2d(const QImage &image);
dlib::array2d<dlib::rgb_pixel> *toArray2d(const Mat &image);

#endif // TOOLS_H
