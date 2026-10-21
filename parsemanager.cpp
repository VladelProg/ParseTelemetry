#include "parsemanager.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <cmath>
#include <iostream>

QDataStream& operator<< (QDataStream&stream, const Variance& variance)
{
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream << variance.r;
    stream << variance.az;
    stream << variance.el;
    stream << variance.vel;
    return stream;
}

QDataStream& operator >>(QDataStream&stream, Variance& variance)
{
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream >> variance.r;
    stream >> variance.az;
    stream >> variance.el;
    stream >> variance.vel;
    return stream;
}

QDataStream& operator<< (QDataStream& stream, const TargetDataNew& data)
{
    stream << data.time_ns;
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream << data.range_m;
    stream << data.radarCrossSection; // crossSection
    stream << data.length_m; // lenght_m
    stream << data.width_Hz; // width_hz
    stream << data.radialVelocity_mps;  // radialVel
    stream << data.tangentialVelocityAz_mps; // Tangential AZ
    stream << data.tangentialVelocityEl_mps; // Tangential El
    stream << data.azimuth_deg;
    stream << data.elevation_deg;
    stream << data.beam_az_deg;  // beam az
    stream << data.beam_el_deg;  // beam el
    stream << data.appendage;   //appendage
    stream << data.id;
    stream << data.type;  // type
    stream << data.variance;
    return stream;
}


QDataStream& operator >>(QDataStream& stream, TargetDataNew& data)
{
    stream>>data.time_ns;
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    stream>>data.range_m;
    stream>>data.radarCrossSection;
    stream>>data.length_m;
    stream>>data.width_Hz;
    stream>>data.radialVelocity_mps;
    stream>>data.tangentialVelocityAz_mps;
    stream>>data.tangentialVelocityEl_mps;
    stream>>data.azimuth_deg;
    stream>>data.elevation_deg;
    stream>>data.beam_az_deg;
    stream>>data.beam_el_deg;
    stream>>data.appendage;
    stream>>data.id;
    stream>>data.type;
    stream>>data.variance;
    return stream;
}

ParseManager::ParseManager()
{
//  scope = new ScopeTimer();

//    connect(this, &ParseManager::startParse, scope, &ScopeTimer::);
//    connect(this, &ParseManager::startSave, scope, &ScopeTimer::);
}

//void ParseManager::InitializeRecord(QDataStream& stream)
//{
//    QString name_param;
//    QString type_param;

//    quint32 count_params;
//    quint32 count_values;

//    qint64 pos=stream.device()->pos();
//    stream>>count_params;

//    for(quint32 i=0;i<count_params;i++)
//    {
//        stream>>name_param;
//        stream>>count_values;
//        for(quint32 j=0;j<count_values;j++)
//        {
//            //type_param = "13SendingStruct";
//            stream>>type_param;
//        }
//    }

//    stream.device()->seek(pos);
//}

//void ParseManager::readRecord(QDataStream& stream)
//{
//    QString name_param;
//    QString type_param;

//    quint32 count_params;
//    quint32 count_values;

//    stream>>count_params;
//    float val;
//    for(quint32 i=0;i<count_params;i++)
//    {
//        stream>>name_param;
//        stream>>count_values;
//        for(quint32 j=0;j<count_values;j++)
//        {
//            stream>>type_param;

//            if(type_param == "13SendingStruct")
//            {
//                SendingStruct structt;
//                stream>>structt.data;
//                stream>>structt.name;
//                stream>>structt.source;
//                stream>>structt.confirm;

//                int size_struct = structt.data.size();
//                int n = size_struct / 80;

//                QDataStream target_stream(&structt.data, QIODevice::ReadWrite);

//                target_stream.setByteOrder(QDataStream::LittleEndian);

//                for (int i = 1; i <= n; i++)
//                {
//                    TargetDataNew target;
//                    target_stream >> target;

//                    if(std::isnan(target.azimuth_deg))
//                    {
//                        emit SigCorruptedData();
//                        break;
//                    }

//                    target_vec.push_back(target);
//                }
//            }
//            if (type_param != "NULL" and (name_param == "STD.angleX" or name_param == "STD.angleY"))
//            {
//                stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
//                stream >> val;
//            }
//        }
//    }
//}


void ParseManager::generateSnapshot(QString name, QString saveFolder, int64_t startRecordTime, int _delta, int64_t startSnapshotTime, int64_t endSnapShotTime)
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

//    QFile file(QString("%3/snapshot_%1_%2.txt").arg(startSnapshotTime).arg(endSnapShotTime).arg(saveFolder));

//    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
//    {
//        QTextStream out(&file);

//        for (auto it = snapshot_vec.begin(); it != snapshot_vec.end(); it++)
//        {
//            const QString& line = it->snapshotDataToString();
//            out << line;
//        }
//    }

    emit SetNowStatus(ActionType::SHAPSHOT, 1, target_vec.size());

    qDebug() << "Генерация снапшота завершена";
}


void ParseManager::init(ActionType action, QString name, QString saveFolder)
{
    target_vec.clear();
    groupedData.clear();
    this->saveFolder = saveFolder;
//    MARKER_OF_TIME_INSCOPE("Parse");
//    {
    QByteArray arr;
    file.setFileName(name);
//    QFile file(name);
    QDataStream stream(arr);
    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "Не удалось прочитать";
    }

    int size = file.size();
    emit setMaxProgressBar(size);

    QFile::Permissions p=file.permissions();
    if(!(p & QFile::Permission::WriteGroup))
        file.setPermissions(file.permissions()|QFile::Permission::WriteGroup);

    QDir dir(saveFolder);
    if (!dir.exists())
    {
        dir.mkpath(saveFolder); // На всякий
    }

    qint64 start_time;

    QDataStream out(&file);
    out >> start_time;

    InitializeRecord(out);

    start_time -= QDateTime::currentDateTime().toMSecsSinceEpoch();
    readRecord(out);

    //int posBeforeRead = file.pos();

    emit SetNowStatus(action, 0, file.pos());

    while(!out.atEnd())
    {
        qint64 time;
        out>>time;
        QDateTime date = QDateTime::fromMSecsSinceEpoch(time);

        readRecord(out);

        emit SetNowStatus(action, 0, file.pos());
    }

//    }

    int64_t start_record_time = 0;
//  400004279349
//  20712787084650
//  20320456037650
//  500004279349
    auto it = std::min_element(target_vec.begin(), target_vec.end(),
              [] (const TargetDataNew& a, const TargetDataNew& b)
    {
        return a.time_ns < b.time_ns;
    });

    if (it != target_vec.end())
    {
        start_record_time = it->time_ns;
    }

    std::sort(target_vec.begin(), target_vec.end(),
              [] (const TargetDataNew& a, const TargetDataNew& b)
    {
//        if (a.range_m > 20000)
//        {
//            //qDebug() << "";
//        }
        return a.range_m < b.range_m;
    });

//    int ti = target_vec.end()->time_ns;

    emit SigStartRecordTime(start_record_time);

    if (action == ActionType::PARSING)
    {
        saving();
    }
}

void ParseManager::saving()
{
    for (const auto& item : target_vec)
    {   // Радиаль
        groupedData[item.id].append(item.dataToString());
    }

    emit setMaxProgressBar((groupedData.constEnd() - 1).key() + 1);

    for (auto it = groupedData.constBegin(); it != groupedData.constEnd(); it++)
    {
        int id = it.key() + 1;
        const QStringList& lines = it.value();
        QFile file(QString("%2/target_%1.txt").arg(id).arg(saveFolder));

        if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&file);

            for (const QString& line : lines)
            {
                out << line;
            }
        }

        emit SetNowStatus(ActionType::PARSING, 1, id);
    }

    qDebug() << "Работа окончена";
}
