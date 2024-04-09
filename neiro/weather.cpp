#include "weather.h"
#include <QDebug>
#include <unistd.h>
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *buffer) {
    size_t total_size = size * nmemb;
    buffer->append((char*)contents, total_size);
    return total_size;
}


Weather::Weather(QDate *TargetDate)
{
    std::string buffer;


     std::string api_url = "https://archive-api.open-meteo.com/v1/era5?latitude=52.4345&longitude=30.975&start_date="+TargetDate->toString("yyyy-MM-dd").toStdString()+"&end_date="
     ""+TargetDate->toString("yyyy-MM-dd").toStdString()+"&hourly=temperature_2m&hourly=wind_speed_10m&hourly=relative_humidity_2m&hourly=precipitation";
   std::cout<<api_url;

    curl = curl_easy_init();
    if (curl) {
            // Установка URL для запросаы
            curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());

            // Устанавливаем callback функцию для обработки ответа
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

            // Добавляем заголовок с API ключом
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, ("X-Yandex-API-Key: " + api_key).c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            // Выполнение запроса

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            } else {
                // Обработка полученного ответа
                std::cout << "Response received: " << buffer << std::endl;
            }
             auto data = nlohmann::json::parse(buffer);
             temperature=0;
             humidity=0;
             os=0;
             for (int i=0;i<24;i++) {
                 double t = data["hourly"]["temperature_2m"][i];
                int h = data["hourly"]["relative_humidity_2m"][i];
                double o = data["hourly"]["precipitation"][i];
                double w = data["hourly"]["wind_speed_10m"][i];

                 temperature += t;
                humidity += h;
                os += o;
                wind +=w;
             }
             temperature/=24;
             humidity/=24;
             os/=24;
            wind/=24;


                // Вывод данных
                std::cout << "Средняя Температура: " << temperature << " градусов Цельсия" << std::endl;
                std::cout << "Среднаяя Влажность: " << humidity << "%" << std::endl;

                 std::cout << "Осададки: " << os*100<<"%"<< std::endl;
                 std::cout << "Ветер: " << wind<<"м/c"<< std::endl;



            // Освобождаем ресурсы
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } else {
            std::cerr << "Failed to initialize curl." << std::endl;
        }


}

Weather::Weather(QDate *curentDate,int y) {
    std::string buffer;


     std::string api_url = "https://api.open-meteo.com/v1/forecast?latitude=52.4345&longitude=30.975&start_date="+curentDate->toString("yyyy-MM-dd").toStdString()+"&end_date="
     ""+curentDate->toString("yyyy-MM-dd").toStdString()+"&hourly=temperature_2m&hourly=wind_speed_10m&hourly=relative_humidity_2m&hourly=precipitation";
   std::cout<<api_url;

    curl = curl_easy_init();
    if (curl) {

            curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());


            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);


            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, ("X-Yandex-API-Key: " + api_key).c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);



            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            } else {

                std::cout << "Response received: " << buffer << std::endl;
            }
             auto data = nlohmann::json::parse(buffer);
             temperature=0;
             humidity=0;
             os=0;
             for (int i=0;i<24;i++) {
                 double t = data["hourly"]["temperature_2m"][i];
                int h = data["hourly"]["relative_humidity_2m"][i];
                double o = data["hourly"]["precipitation"][i];
                double w = data["hourly"]["wind_speed_10m"][i];

                 temperature += t;
                humidity += h;
                os += o;
                wind +=w;
             }
             temperature/=24;
             humidity/=24;
             os/=24;
            wind/=24;


                std::cout << "Средняя Температура: " << temperature << " градусов Цельсия" << std::endl;
                std::cout << "Среднаяя Влажность: " << humidity << "%" << std::endl;

                 std::cout << "Осададки: " << os*100<<"%"<< std::endl;
                 std::cout << "Ветер: " << wind<<"м/c"<< std::endl;



            // Освобождаем ресурсы
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        } else {
            std::cerr << "Failed to initialize curl." << std::endl;
        }
}


float Weather::get_temperature() {
    return temperature;
}


float Weather::get_humidity() {
    return humidity;
}


float Weather::getOs() {
    return os;
}

float Weather::getWindSpeed() {
    return wind;
}
