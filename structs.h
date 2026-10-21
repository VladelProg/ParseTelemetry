#ifndef STRUCTS_H
#define STRUCTS_H

#include <QDataStream>
#include <QDateTime>

struct SendingStruct
{
    QByteArray data; //данные
    QString name; //имя пакета
    QString source; //источник
    bool confirm;
};

enum ActionType
{
    PARSING,
    SHAPSHOT
};

#pragma pack(push,1)
struct Variance
{
    float r;
    float vel;
    float az;
    float el;
};
#pragma pack(pop)

#pragma pack(push,1)
struct TargetDataNew
{
    quint64 time_ns;
    float range_m;            // дальность, м
    float radarCrossSection;  // ЭПР
    float length_m;           // длина, м
    float width_Hz;           // ширина, Гц
    float radialVelocity_mps; // радиальная скорость, м/с
    float tangentialVelocityAz_mps; // тангенциальная скорость азимутальная, м/с
    float tangentialVelocityEl_mps; // тангенциальная скорость угломестная, м/с
    float azimuth_deg;        // угол азимута цели, град
    float elevation_deg;      // угол места цели, град
    float beam_az_deg;        // азимут луча
    float beam_el_deg;        // ум луча
    float appendage;          //отношение сигнал шум
    uint32_t id;                   // идентификатор цели
    uint32_t type;
    Variance variance;
    friend QDataStream& operator<<(QDataStream& stream, const TargetDataNew& data);
    friend QDataStream& operator>>(QDataStream& stream, TargetDataNew& data);

    // az el
    QDateTime toDate(quint64 time) const
    {
        return QDateTime::fromMSecsSinceEpoch(time);
    }

    QString snapshotDataToString() const
    {
        return QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14\n").arg(id)
                                    .arg(range_m)
                                    .arg(appendage)
                                    .arg(radarCrossSection)
                                    .arg(radialVelocity_mps)
                                    .arg(azimuth_deg)
                                    .arg(elevation_deg)
                                    .arg(beam_az_deg)
                                    .arg(beam_el_deg)
                                    .arg(time_ns)
                                    .arg(variance.r)
                                    .arg(variance.vel)
                                    .arg(variance.az)
                                    .arg(variance.el);
    }

    QString dataToString() const
    {
        return QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13\n").arg(range_m)
                                                        .arg(appendage)
                                                        .arg(radarCrossSection)
                                                        .arg(radialVelocity_mps)
                                                        .arg(azimuth_deg)
                                                        .arg(elevation_deg)
                                                        .arg(beam_az_deg)
                                                        .arg(beam_el_deg)
                                                        .arg(time_ns)
                                                        .arg(variance.r)
                                                        .arg(variance.vel)
                                                        .arg(variance.az)
                                                        .arg(variance.el);
    }
};
#pragma pack(pop)

#endif // STRUCTS_H
