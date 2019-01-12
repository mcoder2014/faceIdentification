#include "facerecognizer.h"

#include <vector>

#include <QDebug>
#include <QList>
#include <QFile>
#include <QFileInfo>

#include "qtcsv/variantdata.h"
#include "qtcsv/reader.h"
#include <flann/flann.hpp>

#include "tools.h"

faceRecognizer::faceRecognizer(QObject *parent) : QObject(parent)
{
    this->m_landmark_sp = nullptr;
    this->m_net = nullptr;
    this->init();
}

///
/// \brief faceRecognizer::setFaceDatabase
/// \param filepath
///
///
void faceRecognizer::setFaceDatabase(QString filepath)
{

    this->m_userinfos.clear();          // 清空原有数据表
    this->m_userfeatures.clear();       // 清空特征集

    QFileInfo fileinfo(filepath);
    qDebug() << "---- Read UserInfo file:"
             << filepath
             << "----"
             << fileinfo.absoluteFilePath();


    // 获取 csv 文件列表
    QList<QStringList> readData = QtCSV::Reader::readToList(fileinfo.absoluteFilePath());
    for ( int i = 0; i < readData.size(); ++i )
    {
        qDebug() << readData.at(i).join(",");
    }

    for( int i = 1; i < readData.size(); i++)
    {
        this->m_userinfos.append(UserInfo(readData[i]));
    }

    // 更新特征值
    qDebug() << "Start update feature vector.";

    for(int i = 0; i < this->m_userinfos.size(); i++)
    {
        // 加载图片
        QImage tmp_qimage(m_userinfos[i].imgPath());

        // 转换为dlib格式
        dlib::array2d<dlib::rgb_pixel> *dlibimage = toArray2d(tmp_qimage);

        // 人脸检测
        std::vector<dlib::rectangle> dets = this->m_face_detector(*dlibimage);
        if(dets.size() == 0)
        {
            break;
        }

        // landmark
        full_object_detection shape = (*this->m_landmark_sp)(*dlibimage, dets[0]);
        matrix<rgb_pixel> face_chip;

        qDebug() << "number of parts: "<< shape.num_parts();
        for(int iter = 0; iter < shape.num_parts(); iter ++)
        {
            qDebug() << "Point:" << iter
                     << " x: " << shape.part(iter).x()
                     << " y: " << shape.part(iter).y();
        }


        // 归一化
        extract_image_chip(
                    *dlibimage,
                    get_face_chip_details(shape, 150, 0.25),
                    face_chip);

        std::vector<matrix<rgb_pixel>> faces;
        faces.push_back(face_chip);

        // 提取特征
        std::vector<matrix<float,0,1>> face_des = (*this->m_net)(faces);
        matrix<float,0,1> des = face_des[0];
        this->m_userfeatures.push_back(des);

        qDebug() << "Des: cols:" << des.nc() <<"rows:"<< des.nr();
    }

    // 创建 flann 快速查找方案
    // 建立特征集
    qDebug() << "Flann Create feature array";
    qDebug() << "userfeatures size: " << this->m_userfeatures.size();

    int feature_size = this->m_userfeatures.size() * 128;
    float *tmp_data = new float[feature_size];
    for(int i = 0; i <this->m_userfeatures.size(); i++)
    {
        QStringList tmpstr;
        matrix<float,0,1> tmp_feature = this->m_userfeatures[i];
        for(int j = 0; j < 128; j++)
        {
//            qDebug() << tmp_feature(j) << " index: " << 128 * i + j;
            tmp_data[128 * i + j] = tmp_feature(j);

            QString num_str;
            tmpstr.append(num_str.setNum(tmp_data[i*128 + j]));
        }
        qDebug() << "User" << this->m_userinfos[i].toString();
        qDebug() << "Feature : "
                 << tmpstr.join(" ");
    }

    ::flann::Matrix<float> *feature = new ::flann::Matrix<float>(
                tmp_data,
                this->m_userfeatures.size(),
                128,
                128);

    this->m_flann_userinfos->buildIndex( *feature );

    qDebug() << "flann build index finished.";

}

///
/// \brief faceRecognizer::faceRecognition
/// \param image
///
void faceRecognizer::faceRecognition(QImage image)
{
//    qDebug() << "Image Size : "
//             << " width: " << image.width()
//             << " height: " << image.height();

    this->m_time.restart();     // 计时器重新开始

    // 转换为dlib格式
    dlib::array2d<dlib::rgb_pixel> *dlibimage = toArray2d(image);

    // 人脸检测
    std::vector<dlib::rectangle> dets = this->m_face_detector(*dlibimage);

//    qDebug() << "Detected faces:" << dets.size();
    if(dets.size() == 0)
    {
        qDebug() << "Detected faces: 0";
        return;
    }

    // 对每个检测到的人脸进行 landmark
    std::vector<matrix<rgb_pixel>> faces;
    for(int i = 0; i < dets.size(); i++)
    {
        full_object_detection shape = (*this->m_landmark_sp)(*dlibimage, dets[i]);
        matrix<rgb_pixel> face_chip;
        extract_image_chip(
                    *dlibimage,
                    get_face_chip_details(shape, 150, 0.25),
                    face_chip);     // 归一化

        faces.push_back(face_chip);
    }

    // 提取特征
    std::vector<matrix<float,0,1>> face_des = (*this->m_net)(faces);

    // 可以一口气查询多个 首先构建查询矩阵
    int num_user = face_des.size();     // 识别到的人数
    float *tmp_data = new float[num_user * 128];
    for(int i = 0; i < num_user; i++)
    {
        matrix<float,0,1> tmp_feature = face_des[i];
        for(int j = 0; j < 128; j++)
        {
            tmp_data[128 * i + j] = tmp_feature(j);
        }
    }

    ::flann::Matrix<float> queries(tmp_data,(size_t)num_user, 128, 128);   // 查询矩阵
    std::vector<std::vector<size_t>> indices;                // 储存查询结果
    std::vector< std::vector<float>> dists;    // 储存距离结果
    int nn = 1;     // 只查找最近的点

    /**
    int knnSearch(const Matrix<ElementType>& queries,
                                 std::vector< std::vector<size_t> >& indices,
                                 std::vector<std::vector<DistanceType> >& dists,
                                 size_t knn,
                           const SearchParams& params) const
      */

    ::flann::SearchParams searchparams;

    m_flann_userinfos->knnSearch(queries, indices, dists, (size_t)nn, searchparams);    // 查找最近的 k 个邻域点

    // 设置 人脸区域
    QVector<QRectF> face_region;
    QVector<UserInfo> face_info;
    for(int i=0;i<dets.size(); i++)
    {
        QRectF rect(dets[i].left(),dets[i].top(), dets[i].width(),dets[i].height());
        face_region.push_back(rect);
    }

    for(int i = 0; i < dists.size(); i++)
    {
        float distance = (dists[i])[0];
        if(distance < this->m_threshold)
        {
            // 记录中有该用户
            int userIndex = (int)(indices[i])[0];
            UserInfo userinfo = this->m_userinfos[userIndex];
            face_info.push_back(userinfo);
//            qDebug() << i <<": " << userinfo.name()
//                     << " userIndex: "<< userIndex;
        }
        else
        {
            // 记录中无该用户

//            qDebug() << i << ": 未知";
            UserInfo userinfo;
            userinfo.setUnknownUser();
            face_info.push_back(userinfo);
        }
    }

    qDebug() << "Detected faces: " << dets.size()
             << "Cost time: " << this->m_time.elapsed()/1000.0 << "S";
    for(int i = 0 ; i <face_info.size(); i++)
    {
        qDebug() << i << " " << face_info[i].toString()
                 << "Distance: " << (dists[i])[0];
    }

}

float faceRecognizer::threshold() const
{
    return m_threshold;
}

void faceRecognizer::setThreshold(float threshold)
{
    m_threshold = threshold;
}

///
/// \brief faceRecognizer::init
/// 初始化一些重要部分，比如几个识别器
///
void faceRecognizer::init()
{
    m_face_detector = get_frontal_face_detector();  // 初始化检测器

    // landmark shape predictor
    m_landmark_sp = new shape_predictor();
    deserialize("shape_predictor_5_face_landmarks.dat") >> *m_landmark_sp;

    // image net
    m_net = new anet_type();
    deserialize("dlib_face_recognition_resnet_model_v1.dat") >> *m_net;

    // flann construct function
    // 新建4棵 kd 树
    this->m_flann_userinfos = new ::flann::Index<::flann::L2<float> >(
                ::flann::KDTreeIndexParams(4));

    // 初始化阈值
    this->setThreshold(0.1);
}
