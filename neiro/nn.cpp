#include "nn.h"
#include "plot.h"

const int input_size = 2;
const int output_size = 1;
const int hidden_size = 256;
const int num_epochs = 1000;
const double learning_rate = 1;

void learn(int* prod) {
    SalesPredictionModel model;
    torch::optim::Adam optimizer(model.parameters(), torch::optim::AdamOptions(learning_rate));
    torch::nn::MSELoss loss;
    std::vector<std::tuple<int, int, int, float>> data;

    data = getfile(prod);

    auto [id, days, months, sales] = normalize_data(data);
    auto dataset = torch::data::datasets::TensorDataset({ id, days, months, sales });
    auto data_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
        std::move(dataset), torch::data::DataLoaderOptions().batch_size(64));

    for (int epoch = 0; epoch < num_epochs; ++epoch) {
        for (auto& batch : *data_loader) {
            auto id_batch = batch.data()[0];
            auto days_batch = batch.data()[1];
            auto months_batch = batch.data()[2];
            auto sales_batch1 = batch.data()[3];
            auto sales_batch = torch::stack({ sales_batch1 }).unsqueeze(2);

            // Логируем входные данные
            optimizer.zero_grad();

            auto predictions = model.forward(id_batch, days_batch, months_batch);
            auto l = loss(predictions, sales_batch);

            l.backward();
            optimizer.step();

            std::cout << "epoch: " << epoch << "  loss:" << l.item<float>() << std::endl;
        }
    }

    std::cout << "Training finished, save? (1,0)" << std::endl;
    bool h;
    std::cin >> h;
    if (h == 1) {
        torch::serialize::OutputArchive archive;
        model.save(archive);
        QString path = "models/" + QString::number(*prod) + "model.pt";
        archive.save_to(path.toStdString());
    }
}

float denormalize_value(float normalized_value, float mean, float std) {
    return (normalized_value * std) + mean;
}

std::tuple<torch::Tensor, torch::Tensor, torch::Tensor> normalize_input_data(
    torch::Tensor id_tensor, torch::Tensor days_tensor, torch::Tensor months_tensor,
    float id_mean, float id_std, float days_mean, float days_std, float months_mean, float months_std) {

    auto normalized_id = id_tensor;
    auto normalized_days = (days_tensor - days_mean) / days_std;
    auto normalized_months = (months_tensor - months_mean) / months_std;

    // Логирование нормализованных значений
    std::cout << "Normalized input values:" << std::endl;
    std::cout << "Normalized ID: " << normalized_id << std::endl;
    std::cout << "Normalized Days: " << normalized_days << std::endl;
    std::cout << "Normalized Months: " << normalized_months << std::endl;

    return std::make_tuple(normalized_id, normalized_days, normalized_months);
}

QMap<QDateTime, float> prediction(QList<QDate>* endDate, qint32* pickedprod) {
    SalesPredictionModel model;
    QMap<QDateTime, float> result;
    QString path = "models/" + QString::number(*pickedprod) + "model.pt";
    torch::serialize::InputArchive archive;
    archive.load_from(path.toStdString());
    model.load(archive);

    model.eval();

    std::vector<std::tuple<int, int, int, float>> data;
    data = getfile(pickedprod);

    auto id_data = torch::tensor(torch::fmap(data, [](const auto& tup) { return std::get<0>(tup); }), torch::kFloat32);
    auto days_data = torch::tensor(torch::fmap(data, [](const auto& tup) { return std::get<1>(tup); }), torch::kFloat32);
    auto months_data = torch::tensor(torch::fmap(data, [](const auto& tup) { return std::get<2>(tup); }), torch::kFloat32);
    auto sales_data = torch::tensor(torch::fmap(data, [](const auto& tup) { return std::get<3>(tup); }), torch::kFloat32);

    auto id_mean = torch::mean(id_data).item<float>();
    auto id_std = torch::std(id_data).item<float>();
    auto days_mean = torch::mean(days_data).item<float>();
    auto days_std = torch::std(days_data).item<float>();
    auto months_mean = torch::mean(months_data).item<float>();
    auto months_std = torch::std(months_data).item<float>();
    auto sales_mean = torch::mean(sales_data).item<float>();
    auto sales_std = torch::std(sales_data).item<float>();

    QDate tempdate = endDate->at(0);
    torch::Tensor test_id;
    torch::Tensor test_day;
    torch::Tensor test_month;
    torch::Tensor predicted_output;

    while (tempdate != endDate->at(1)) {
        test_id = torch::tensor({ *pickedprod }, torch::kFloat32);
        test_day = torch::tensor({ tempdate.day() }, torch::kFloat32);
        test_month = torch::tensor({ tempdate.month() }, torch::kFloat32);

        auto [normalized_id, normalized_days, normalized_months] = normalize_input_data(
            test_id, test_day, test_month, id_mean, id_std, days_mean, days_std, months_mean, months_std
        );

        predicted_output = model.forward(normalized_id, normalized_days, normalized_months);

        // Денормализация и добавление в результат
        float denormalized_output = denormalize_value(predicted_output.item<float>(), sales_mean, sales_std);
        result.insert(QDateTime(tempdate.startOfDay()), std::abs(denormalized_output));

        tempdate = tempdate.addDays(1);
    }

    return result;
}

std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor> normalize_data(
    const std::vector<std::tuple<int, int, int, float>>& data) {
    std::vector<int> ids, days, months;
    std::vector<float> sales;

    std::transform(data.begin(), data.end(), std::back_inserter(ids), [](const auto& tup) { return std::get<0>(tup); });
    std::transform(data.begin(), data.end(), std::back_inserter(days), [](const auto& tup) { return std::get<1>(tup); });
    std::transform(data.begin(), data.end(), std::back_inserter(months), [](const auto& tup) { return std::get<2>(tup); });
    std::transform(data.begin(), data.end(), std::back_inserter(sales), [](const auto& tup) { return std::get<3>(tup); });

    torch::Tensor ids_tensor = torch::tensor(ids, torch::kFloat32);
    torch::Tensor days_tensor = torch::tensor(days, torch::kFloat32);
    torch::Tensor months_tensor = torch::tensor(months, torch::kFloat32);
    torch::Tensor sales_tensor = torch::tensor(sales, torch::kFloat32);

    // Z-нормализация
    auto z_normalize = [](torch::Tensor& tensor) {
        auto mean = tensor.mean().item<float>();
        auto std = tensor.std().item<float>();

        tensor = (tensor - mean) / std;

        std::cout << "Normalized values:" << std::endl;
        std::cout << tensor << std::endl;
    };

    z_normalize(days_tensor);
    z_normalize(months_tensor);
    z_normalize(sales_tensor);

    return std::make_tuple(ids_tensor, days_tensor, months_tensor, sales_tensor);
}

SalesPredictionModel::SalesPredictionModel() {
    lstm1 = register_module("lstm1", torch::nn::LSTM(torch::nn::LSTMOptions(input_size, hidden_size).num_layers(1).batch_first(true)));
    lstm2 = register_module("lstm2", torch::nn::LSTM(torch::nn::LSTMOptions(hidden_size, hidden_size).num_layers(1).batch_first(true)));
    linear2 = register_module("linear2", torch::nn::Linear(hidden_size, output_size));
}

torch::Tensor SalesPredictionModel::forward(torch::Tensor id_tensor, torch::Tensor days_tensor, torch::Tensor months_tensor) {
    auto input_tensor = torch::stack({ days_tensor, months_tensor }).unsqueeze(0).transpose(1, 2);

    auto lstm_output1 = lstm1->forward(input_tensor);
    auto lstm_out1 = std::get<0>(lstm_output1).squeeze(0);

 \

    return linear2->forward(lstm_out1.unsqueeze(0));
}

void Plot::updateNN(DB* dbase1, QValueAxis* axisY, QDateTimeAxis* axisX, QSpinBox* lePickedProd, QDateEdit* leFirstDate, QDateEdit* leSecondDate) {

    serNN = new QSplineSeries();
    mapNN = new QMap<QDateTime, float>;
    QList<QDate> endDate = { leFirstDate->date(), leSecondDate->date() };
    QList<QDateTime> dates;
    QList<float> values;
    qint32 pickedprod = lePickedProd->value();
    *mapNN = prediction(&endDate, &pickedprod);

    for (QMap<QDateTime, float>::const_iterator i = mapNN->begin(); i != mapNN->end(); i++) {
        values.push_back(i.value());
        qDebug() << i.value();
        qDebug() << i.key();
        dates.push_back(i.key());
    }

    chartnn->removeAllSeries();
    chartnn->series().clear();
    *slider_rangeNN = dates.size();
    for (int i = 0; i < dates.size(); ++i) {
        serNN->append(dates[i].toMSecsSinceEpoch(), values[i]);
    }

    chartnn->addSeries(serNN);
    sliderNN->setRange(0, *slider_rangeNN);
    qint32 a = lePickedProd->value();
    axisY->setTitleText("продажи \"" + dbase1->getProductName(&a) + "\"");
    axisX->setRange(dates.first(), dates.last());
    axisY->setRange(*std::min_element(values.begin(), values.end()), *std::max_element(values.begin(), values.end()));
}
