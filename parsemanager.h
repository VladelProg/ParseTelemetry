#ifndef PARSEMANAGER_H
#define PARSEMANAGER_H

#include <QObject>
#include "structs.h"
#include "scopetimer.h"
#include <QFile>
#include "parsing.h"

class ParseManager : public parsing
{
    Q_OBJECT
public:
    ScopeTimer* scope;
    explicit ParseManager();

//    void InitializeRecord(QDataStream& stream);
//    void readRecord(QDataStream& stream);

    QMap<int64_t, QStringList> groupedData;
    QMap<int64_t, QStringList> groupedSnapshotData;

    QFile file;
    QString saveFolder;
    std::vector<TargetDataNew> target_vec;
    std::vector<TargetDataNew> snapshot_vec;
    QVector<QFile*> files;

signals:
    void setMaxProgressBar(qint64 size);
    void SetNowStatus(ActionType action, bool status, qint64 size);
    void SigCorruptedData();

    void startParse();
    void startSave();

    void SigStartRecordTime(double time);


public slots:
    void init(ActionType action, QString name, QString saveFolder);
    void generateSnapshot(QString name, QString saveFolder, int64_t startRecordTime, int delta, int64_t startSnapshotTime, int64_t endSnapShotTime);
    void saving();
};

#endif // PARSEMANAGER_H
