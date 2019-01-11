#include "userinfo.h"

#include <QStringList>
#include <QString>

UserInfo::UserInfo() : QObject(nullptr)
{

}

UserInfo::UserInfo(int index, QString name, QString userId, QString imgPath, QObject *parent)
    :QObject (parent),m_index(index),m_name(name),m_userId(userId),m_imgPath(imgPath)
{
    // Initialization
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

}

UserInfo::UserInfo(const UserInfo &userinfo)
    :QObject(userinfo.parent()),m_index(userinfo.m_index),
      m_name(userinfo.m_name),m_userId(userinfo.m_userId),
      m_imgPath(userinfo.m_imgPath)
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

QString UserInfo::userId() const
{
    return m_userId;
}

void UserInfo::setUserId(const QString &userId)
{
    m_userId = userId;
}
