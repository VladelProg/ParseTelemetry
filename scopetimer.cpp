#include "scopetimer.h"

//ScopeTimer::ScopeTimer()
//{
//    start = std::chrono::high_resolution_clock::now();
//}

QMap<QString, quint64> ScopeTimer::map;
QMap<QString, quint64> ScopeTimer::map_count;
std::chrono::high_resolution_clock::time_point ScopeTimer::program_start;


ScopeTimer::ScopeTimer(QString name)
{
    _name = name;

    start = std::chrono::high_resolution_clock::now();

//     end = std::chrono::high_resolution_clock::now();
}

ScopeTimer::~ScopeTimer()
{
    end = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    if (map.contains(_name)){
        map[_name] += dur;
        map_count[_name]++;
    }

    else{
        map.insert(_name, dur);
        map_count[_name] = 1;
    }
}
