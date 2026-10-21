#ifndef PARSING_H
#define PARSING_H

#include <QObject>
#include <QDataStream>
#include "structs.h"
#include <QFile>
#include <QDir>
#include <QDebug>

class parsing : public QObject
{
    Q_OBJECT

public:
    parsing();

    QFile file;
    std::vector<TargetDataNew> target_vec;
    virtual void InitializeRecord(QDataStream& stream);
    virtual void readRecord(QDataStream& stream);
    virtual void init(ActionType action, QString name, QString saveFolder);

signals:
    void setMaxProgressBar(qint64 size);
    void SetNowStatus(ActionType action, bool status, qint64 size);
    void SigCorruptedData();

    void startParse();
    void startSave();

    void SigStartRecordTime(double time);

};

#endif // PARSING_H
