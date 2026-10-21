#ifndef SCOPETIMER_H
#define SCOPETIMER_H

#include <QObject>
#include <chrono>
#include <QMap>

#define MARKER_OF_TIME ScopeTimer _p(__PRETTY_FUNCTION__)
#define MARKER_OF_TIME_INSCOPE(NAME) ScopeTimer _p(QString(__PRETTY_FUNCTION__) + "->" + #NAME)

class ScopeTimer
{
public:
    ScopeTimer(QString name);
    static void Open();
    static void Close();
     ~ScopeTimer();
private:
    static std::chrono::high_resolution_clock::time_point program_start;

    QString _name;
    //std::chrono::time_point<std::chrono::_V2::system_clock , std::chrono::duration<std::chrono::high_resolution_clock::time_point, std::chrono::high_resolution_clock::time_point>> dur;
    quint64 dur;

    static QMap<QString, quint64> map;
    static QMap<QString, quint64> map_count;

    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point end;
};


#endif // SCOPETIMER_H
