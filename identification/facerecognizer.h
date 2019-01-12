#ifndef FACERECOGNIZER_H
#define FACERECOGNIZER_H

#include <QObject>
#include <QVector>
#include <QImage>
#include <QRectF>
#include <QTime>

#include <dlib/dnn.h>
//#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <opencv2/core.hpp>
//#include <opencv2/flann.hpp>
#include <flann/flann.hpp>

#include "models/userinfo.h"


using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------

// The next bit of code defines a ResNet network.  It's basically copied
// and pasted from the dnn_imagenet_ex.cpp example, except we replaced the loss
// layer with loss_metric and made the network somewhat smaller.  Go read the introductory
// dlib DNN examples to learn what all this stuff means.
//
// Also, the dnn_metric_learning_on_images_ex.cpp example shows how to train this network.
// The dlib_face_recognition_resnet_model_v1 model used by this example was trained using
// essentially the code shown in dnn_metric_learning_on_images_ex.cpp except the
// mini-batches were made larger (35x15 instead of 5x5), the iterations without progress
// was set to 10000, and the training dataset consisted of about 3 million images instead of
// 55.  Also, the input layer was locked to images of size 150.
template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<
                            alevel0<
                            alevel1<
                            alevel2<
                            alevel3<
                            alevel4<
                            max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2,
                            input_rgb_image_sized<150>
                            >>>>>>>>>>>>;

// ----------------------------------------------------------------------------------------


class faceRecognizer : public QObject
{
    Q_OBJECT
public:
    explicit faceRecognizer(QObject *parent = nullptr);
    void setFaceDatabase(QString filepath);         // 设置人脸数据库

    float threshold() const;
    void setThreshold(float threshold);

signals:
    // 发送人脸识别结果的信号
    void recongnitionResult(QVector<QRectF> rects, QVector<UserInfo> userinfos);

public slots:
    void faceRecognition(QImage image);             // 识别人脸

private:
    frontal_face_detector m_face_detector;         // 人脸检测
    shape_predictor *m_landmark_sp;                 // 人脸特征点
    anet_type *m_net;                               // 人脸特征提取网络，化特征为 128 维特征向量
    float m_threshold;                              // 判定是否为同一用户的阈值

    QVector<UserInfo> m_userinfos;                  // 用户信息
    QVector<matrix<float,0,1>> m_userfeatures;      // 测量出的用户特征
    ::flann::Index<::flann::L2<float>> *m_flann_userinfos;   // 快速查找用户
    QTime m_time;                                   // 计时器，用来测量人脸识别花费时间

    void init();                                    // 初始化
};

#endif // FACERECOGNIZER_H
