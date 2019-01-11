TEMPLATE = subdirs

SUBDIRS += \
    qtcsv \
    identification

CONFIG += ordered

#DEFINES += QT_NO_DEBUG_OUTPUT   # 禁止 qDebug 输出


message(QT Version: $$[QT_VERSION])
