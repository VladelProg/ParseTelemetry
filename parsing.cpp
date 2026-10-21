#include "parsing.h"

parsing::parsing()
{

}

void parsing::InitializeRecord(QDataStream& stream)
{
    QString name_param;
    QString type_param;

    quint32 count_params;
    quint32 count_values;

    qint64 pos=stream.device()->pos();
    stream>>count_params;

    for(quint32 i=0;i<count_params;i++)
    {
        stream>>name_param;
        stream>>count_values;
        for(quint32 j=0;j<count_values;j++)
        {
            //type_param = "13SendingStruct";
            stream>>type_param;
        }
    }

    stream.device()->seek(pos);
}

void parsing::readRecord(QDataStream& stream)
{
    QString name_param;
    QString type_param;

    quint32 count_params;
    quint32 count_values;

    stream>>count_params;
    float val;
    for(quint32 i=0;i<count_params;i++)
    {
        stream>>name_param;
        stream>>count_values;
        for(quint32 j=0;j<count_values;j++)
        {
            stream>>type_param;

            if(type_param == "13SendingStruct")
            {
                SendingStruct structt;
                stream>>structt.data;
                stream>>structt.name;
                stream>>structt.source;
                stream>>structt.confirm;

                int size_struct = structt.data.size();
                int n = size_struct / 80;

                QDataStream target_stream(&structt.data, QIODevice::ReadWrite);

                target_stream.setByteOrder(QDataStream::LittleEndian);

                for (int i = 1; i <= n; i++)
                {
                    TargetDataNew target;
                    target_stream >> target;

//                    if(std::isnan(target.azimuth_deg))
//                    {
//                        emit SigCorruptedData();
//                        break;
//                    }

                    target_vec.push_back(target);
                }
            }
            if (type_param != "NULL" and (name_param == "STD.angleX" or name_param == "STD.angleY"))
            {
                stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
                stream >> val;
            }
        }
    }
}

void parsing::init(ActionType action, QString name, QString saveFolder)
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

