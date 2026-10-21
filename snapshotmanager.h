#ifndef SNAPSHOTMANAGER_H
#define SNAPSHOTMANAGER_H

#include <QTextStream>
#include "parsing.h"

class SnapshotManager : public parsing
{
public:
    SnapshotManager();

    void generateSnapshot(QString name, QString saveFolder, int64_t startRecordTime, int _delta, int64_t startSnapshotTime, int64_t endSnapShotTime);
};

#endif // SNAPSHOTMANAGER_H
