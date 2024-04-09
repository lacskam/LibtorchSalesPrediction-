#ifndef WEATHER_H
#define WEATHER_H
#include <QDate>
#include <curl/curl.h>
#include <iostream>
#include "/usr/include/nlohmann/json.hpp"
#include <QFile>

//короч нужно сделатиь что бы эта хуйня в слои долбились паралельно с айди товра
class Weather
{
private:
    CURL *curl;
    CURLcode res;

    std::string api_key = "wZ3pr9T1qVbrtcMF";



    float temperature;
    float humidity;
    float os;
    float wind;


public:
    Weather(QDate *TargetDate);
    Weather(QDate *curentDate,int y);
    void writeFile();
    float get_temperature();
    float get_humidity();
    float getOs();
    float getWindSpeed();





};

#endif // WEATHER_H
