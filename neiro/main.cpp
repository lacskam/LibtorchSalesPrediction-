#include <torch/torch.h>
#include <iostream>
#include <vector>
#include <QFileDialog>

std::vector<std::tuple<int, int, int, float>> data;



QString getfile() {
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Выберите файл", "", "Файлы с расширением .sex (*.sex);;Все файлы (*.*)");

    if (!filePath.isEmpty()) {

           QFile file(filePath);

           if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
               QTextStream stream(&file);


               QString fileContents = stream.readAll();
               qDebug() << "Содержимое файла:" << fileContents;

               file.close();


               QList<QString> temp = fileContents.split("\n");
               for (int i =0;i<temp.size();i++) {
                    data.push_back({QString::number(temp[i].split(" ")[0]).toInt(),QString::number(temp[i].split(" ")[1].split(".")[2]).toInt(),
                                    QString::number(temp[i].split(" ")[1].split(".")[1]).toInt(),QString::number(temp[i].split(" ")[2]).toFloat());
               }



           } else {
               qDebug() << "Ошибка открытия файла для чтения";
           }
       } else {
           qDebug() << "Отменено пользователем";
       }



}
// Определение нейронной сети
struct DemandPredictionNet : torch::nn::Module {
    DemandPredictionNet(bool is_training=true) {
        // Определение входных и выходных слоев
        if (is_training) {
            input_layer = register_module("input_layer", torch::nn::Linear(3, 64));  // В обучающем режиме 4 входа
        } else {
            input_layer = register_module("input_layer", torch::nn::Linear(3, 64));  // В тестовом режиме 3 вахода
        }
        hidden_layer = register_module("hidden_layer", torch::nn::Linear(64, 32));
        output_layer = register_module("output_layer", torch::nn::Linear(32, 1));
    }

    // Функция прямого распространения
    torch::Tensor forward(torch::Tensor input) {
        input = torch::relu(input_layer(input));
        input = torch::relu(hidden_layer(input));
        return output_layer(input);
    }

    // Слои нейронной сети
    torch::nn::Linear input_layer{nullptr}, hidden_layer{nullptr}, output_layer{nullptr};
};

// Функция генерации случайных тестовых данных
std::vector<std::tuple<int, int, int, float>> generate_test_data(int num_samples) {
    std::vector<std::tuple<int, int, int, float>> data;

    // Генерация случайных данных
    for (int i = 0; i < num_samples; ++i) {
        int product_id = rand() % 10 + 1; // Замените 10 на количество товаров в вашем наборе данных
        int day = rand() % 31 + 1; // Замените 31 на количество дней в месяце
        int month = rand() % 12 + 1; // Замените 12 на количество месяцев в году
        float sales = static_cast<float>(rand() % 100); // Продажи могут быть любыми значениями

        data.emplace_back(product_id, day, month, sales);
    }

    return data;
}

int main() {
    // Инициализация нейронной сети в режиме обучения
    DemandPredictionNet net(true);
    torch::optim::Adam optimizer(net.parameters(), torch::optim::AdamOptions(0.001));

    // Количество эпох обучения
    int num_epochs = 1000;

    // Генерация тестовых данных для обучения
    std::vector<std::tuple<int, int, int, float>> training_data = generate_test_data(1000);

    // Обучение нейронной сети
    for (int epoch = 0; epoch < num_epochs; ++epoch) {
        // Подготовка данных для обучения
        torch::Tensor input_data = torch::empty({training_data.size(), 3}, torch::kFloat32);
        torch::Tensor target_data = torch::empty({training_data.size(), 1}, torch::kFloat32);

        for (size_t i = 0; i < training_data.size(); ++i) {
            input_data[i][0] = static_cast<float>(std::get<0>(training_data[i]));  // айди товара
            input_data[i][1] = static_cast<float>(std::get<1>(training_data[i]));  // день
            input_data[i][2] = static_cast<float>(std::get<2>(training_data[i]));  // месяц


            target_data[i][0] = std::get<3>(training_data[i]);  // целевое значение: количество продаж
        }

        // Обнуление градиентов
        optimizer.zero_grad();

        // Прямое распространение
        torch::Tensor output = net.forward(input_data);

        // Расчет функции потерь
        torch::Tensor loss = torch::mse_loss(output, target_data);

        // Обратное распространение и оптимизация
        loss.backward();
        optimizer.step();

        // Вывод текущей эпохи и потерь
        if (epoch % 100 == 0) {
            std::cout << "Epoch: " << epoch << ", Loss: " << loss.item<float>() << std::endl;
        }
    }

    // Тестирование нейронной сети на новых данных в режиме тестирования
    DemandPredictionNet test_net(false);

    std::vector<std::tuple<int, int, int>> test_inputs = {
        {1, 30, 12}, // Пример входных данных для теста (product_id, day, month)
        {2, 20, 5},
        {3, 5, 8}
        // Добавьте свои тестовые данные
    };

    for (const auto& test_input : test_inputs) {
        torch::Tensor input_tensor = torch::tensor({std::get<0>(test_input), std::get<1>(test_input), std::get<2>(test_input)}, torch::kFloat32);

        torch::Tensor predicted_demand = test_net.forward(input_tensor);

        std::cout << "For input: {" << std::get<0>(test_input) << ", " << std::get<1>(test_input) << ", " << std::get<2>(test_input)
                  << "}, Predicted Demand: " << predicted_demand.item<float>() << std::endl;
    }

    return 0;
}
