#include "snapshotmanager.h"

SnapshotManager::SnapshotManager()
{

}

void SnapshotManager::generateSnapshot(QString name, QString saveFolder, int64_t startRecordTime, int _delta, int64_t startSnapshotTime, int64_t endSnapShotTime)
{
    snapshot_vec.clear();

    init(ActionType::SHAPSHOT, name, saveFolder);

    emit setMaxProgressBar((target_vec.size()));

    startSnapshotTime += startRecordTime;
    endSnapShotTime   += startRecordTime;

    //  20712787084650
    //  20320456037650

    for (const auto& item : target_vec)
    {
//        if (item.time_ns >= 20341776212650 and item.time_ns <= 20346776212650)
//        {
            snapshot_vec.push_back(item);
//        }
    }

    std::sort(snapshot_vec.begin(), snapshot_vec.end(),
              [] (const TargetDataNew& a, const TargetDataNew& b)
    {
        return a.time_ns < b.time_ns;
    });

    for (const auto& item : snapshot_vec)
    {
        groupedSnapshotData[item.time_ns].append(item.snapshotDataToString());
    }

    int64_t delta = _delta * 10e6; // Время в мс
    int64_t startTime = groupedSnapshotData.begin().key();

    for (auto it = groupedSnapshotData.constBegin(); it != groupedSnapshotData.constEnd(); it++)
    {
        int64_t id = it.key();

        if (id == startTime)
        {
            const QStringList& lines = it.value();
            QFile file(QString("%2/snapshot_%1.txt").arg(id).arg(saveFolder));


            if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&file);

                for (const QString& line : lines)
                {
                    out << line;
                }
            }
        }

        if (id - startTime > delta)
        {
            startTime = id;

            const QStringList& lines = it.value();
            QFile file(QString("%2/snapshot_%1.txt").arg(id).arg(saveFolder));


            if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
            {
                QTextStream out(&file);

                for (const QString& line : lines)
                {
                    out << line;
                }
            }
        }

        //emit SetNowStatus(ActionType::SHAPSHOT, 1, id);
    }


    emit SetNowStatus(ActionType::SHAPSHOT, 1, target_vec.size());

    qDebug() << "Генерация снапшота завершена";
}


