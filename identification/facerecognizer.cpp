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
        QString tmpstr;
        matrix<float,0,1> tmp_feature = this->m_userfeatures[i];
        for(int j = 0; j < 128; j++)
        {
//            qDebug() << tmp_feature(j) << " index: " << 128 * i + j;
            tmp_data[128 * i + j] = tmp_feature(j);

            QString num_str;
            tmpstr = tmpstr + num_str.setNum(tmp_data[i*128 + j]) + " ";
        }
        qDebug() << "User"
                 << "index " << this->m_userinfos[i].index()
                 << "name " << this->m_userinfos[i].name()
                 << "id " << this->m_userinfos[i].userId()
                 << "ImgPath " << this->m_userinfos[i].imgPath();
        qDebug() << "Feature : "
                 << tmpstr;
    }

    ::flann::Matrix<float> *feature = new ::flann::Matrix<float>(
                tmp_data,
                this->m_userfeatures.size(),
                128,
                128);

    this->m_flann_userinfos->buildIndex( *feature );

    qDebug() << "flann build index finished.";

}

void faceRecognizer::faceRecognition(QImage image)
{

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
}
