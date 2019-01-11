#include "facerecognizer.h"

#include <vector>

#include <QDebug>
#include <QList>

#include "qtcsv/variantdata.h"
#include "qtcsv/reader.h"

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

    this->m_userinfos.clear();  // 清空原有数据表
    qDebug() << "---- Read UserInfo file:"
             << filepath
             << "----";
    // 获取 csv 文件列表
    QList<QStringList> readData = QtCSV::Reader::readToList(filepath);
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
    std::vector<matrix<float,0,1>> face_descriptors;    // 存储特征结果

    for(int i = 0; i < this->m_userinfos.size(); i++)
    {
        // 加载图片
        QImage tmp_qimage(m_userinfos[i].imgPath());

        // 转换为dlib格式
        array2d<dlib::rgb_pixel> *dlibimage = toArray2d(tmp_qimage);

        // 人脸检测
        std::vector<dlib::rectangle> dets = this->m_face_detector(*dlibimage);
        if(dets.size() == 0)
        {
            break;
        }
        // landmark
        auto shape = (*this->m_landmark_sp)(dlibimage, dets[0]);
        matrix<rgb_pixel> face_chip;

        // 归一化
        extract_image_chip(
                    dlibimage,
                    get_face_chip_details(shape, 150, 0.25),
                    face_chip);

        std::vector<matrix<rgb_pixel>> faces;
        faces.push_back(face_chip);

        // 提取特征
        std::vector<matrix<float,0,1>> face_des = (*this->m_net)(faces);
        matrix<float,0,1> des = face_des[0];
        face_descriptors.push_back(des);
    }



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
}
