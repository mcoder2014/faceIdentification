#ifndef USERINFO_H
#define USERINFO_H

#include <QObject>
#include <QString>

class UserInfo : public QObject
{
    Q_OBJECT
public:
    explicit UserInfo();
    UserInfo(int index, QString name, QString userId,
             QString imgPath, QObject *parent = nullptr);
    UserInfo(QStringList str, QObject *parent = nullptr);
    UserInfo(const UserInfo& userinfo);

    int index() const;
    void setIndex(int index);
    QString name() const;
    void setName(const QString &name);
    QString userId() const;
    void setUserId(const QString &userId);
    QString imgPath() const;
    void setImgPath(const QString &imgPath);
    bool isUnknown() const;
    void setIsUnknown(bool isUnknown);

    void setUnknownUser();      // 将UserInfo设置为未知用户信息
    QString toString();         // 方便输出测试结果
    QString toSimpleString();   // 输出在屏幕上显示的
    UserInfo& operator= (const UserInfo &userinfo);

private:
    int m_index;            // 序号
    QString m_name;         // 姓名
    QString m_userId;       // 身份识别码，可以基于此码查找到用户的唯一信息
    QString m_imgPath;      // 图像地址，用户信息在数据库中的存档照片

    bool m_isUnknown;        // 是否为未知用户的一个标记
                            // true unknown
                            // false known

};

#endif // USERINFO_H
