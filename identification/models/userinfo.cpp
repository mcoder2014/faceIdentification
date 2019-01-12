#include "userinfo.h"

#include <QStringList>
#include <QString>

UserInfo::UserInfo() : QObject(nullptr)
{
    this->setIsUnknown(true);
}

UserInfo::UserInfo(int index, QString name, QString userId, QString imgPath, QObject *parent)
    :QObject (parent),m_index(index),m_name(name),m_userId(userId),m_imgPath(imgPath)
{
    // Initialization
    this->setIsUnknown(false);
}

UserInfo::UserInfo(QStringList str, QObject *parent)
    : QObject (parent)
{
    if(str.size() == 4)
    {
        m_index = str[0].toInt();
        m_name = str[1];
        m_userId = str[2];
        m_imgPath = str[3];
    }
    this->setIsUnknown(false);

}

UserInfo::UserInfo(const UserInfo &userinfo)
    :QObject(userinfo.parent()),m_index(userinfo.m_index),
      m_name(userinfo.m_name),m_userId(userinfo.m_userId),
      m_imgPath(userinfo.m_imgPath),m_isUnknown(userinfo.m_isUnknown)
{

}

int UserInfo::index() const
{
    return m_index;
}

void UserInfo::setIndex(int index)
{
    m_index = index;
}

QString UserInfo::name() const
{
    return m_name;
}

void UserInfo::setName(const QString &name)
{
    m_name = name;
}

QString UserInfo::imgPath() const
{
    return m_imgPath;
}

void UserInfo::setImgPath(const QString &imgPath)
{
    m_imgPath = imgPath;
}

bool UserInfo::isUnknown() const
{
    return m_isUnknown;
}

void UserInfo::setIsUnknown(bool isUnknown)
{
    m_isUnknown = isUnknown;
}

void UserInfo::setUnknownUser()
{
    this->setIndex(-1);
    this->setName("未知用户");
    this->setUserId("00000000000");
    this->setImgPath("Unknown");
    this->setIsUnknown(true);
}

QString UserInfo::toString()
{
    QStringList str_list;
    str_list << "Index: " + QString().setNum(m_index)
             << "Name: " + m_name
             << "UserId: " + m_userId
             << "ImgPath: " + m_imgPath
             << "IsUnknown: " + QString().setNum(m_isUnknown);
    return str_list.join(",");
}

QString UserInfo::toSimpleString()
{
    QStringList str_list;
    str_list << "Index: " + QString().setNum(m_index)
             << "Name: " + m_name
             << "UserId: " + m_userId;
    return str_list.join(",");
}

UserInfo& UserInfo::operator=(const UserInfo &userinfo)
{

    this->m_index = userinfo.m_index;
    this->m_name = userinfo.m_name;
    this->m_userId = userinfo.m_userId;
    this->m_imgPath = userinfo.m_imgPath;
    this->m_isUnknown = userinfo.m_isUnknown;

    return *this;
}

QString UserInfo::userId() const
{
    return m_userId;
}

void UserInfo::setUserId(const QString &userId)
{
    m_userId = userId;
}
