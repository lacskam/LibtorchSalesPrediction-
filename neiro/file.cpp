#include "file.h"

void writeFile(int prod, int day, int mou, float dem) {
    QFile file(QString::number(prod) + "_prod.sex");

    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream stream(&file);
        {
            stream << QString::number(dem) + " 2024." + QString::number(mou) + "." + QString::number(day) << endl;
        }

    } else {
        qDebug() << "Ошибка открытия файла";
    }
    file.close();
}

std::vector<std::tuple<int, int, int, float>> getfile() {
    std::vector<std::tuple<int, int, int, float>> data;
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Выберите файл", "", "Файлы с расширением .sex (*.sex);;Все файлы (*.*)");

    if (!filePath.isEmpty()) {
        QFile file(filePath);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);

            QString fileContents = stream.readAll();

            file.close();

            QList<QString> temp = fileContents.split("\n");
            for (int i = 0; i < temp.size() - 1; i++) {
                if ((temp[i].split(" ")[3]).toInt() !=0 ) {
                    data.push_back({(temp[i].split(" ")[0]).toInt(), (temp[i].split(" ")[1]).toInt(),
                                    temp[i].split(" ")[2].toInt(), (temp[i].split(" ")[3]).toFloat()});
                    qDebug() << (temp[i].split(" ")[0]).toInt() << " " << (temp[i].split(" ")[1]).toInt() << " "
                             << temp[i].split(" ")[2].toInt() << " " << (temp[i].split(" ")[3]).toFloat();
                }
            }
        } else {
            qDebug() << "Ошибка открытия файла для чтения";
        }
    } else {
        qDebug() << "Отменено пользователем";
    }
    return data;
}
