#include"file.h"
#include <QApplication>

\
#include <torch/script.h>
const int input_size = 3;
const int output_size = 1;
const int hidden_size =1000;
const int num_epochs = 100;
const double learning_rate =1;





std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor> normalize_data(
    const std::vector<std::tuple<int, int, int, float>>& data) {
    std::vector<int> ids, days, months;
    std::vector<float> sales;

    std::transform(data.begin(), data.end(), std::back_inserter(ids), [](const auto& tup) { return std::get<0>(tup); });
    std::transform(data.begin(), data.end(), std::back_inserter(days), [](const auto& tup) { return std::get<1>(tup); });
    std::transform(data.begin(), data.end(), std::back_inserter(months), [](const auto& tup) { return std::get<2>(tup); });
    std::transform(data.begin(), data.end(), std::back_inserter(sales), [](const auto& tup) { return std::get<3>(tup); });

    torch::Tensor ids_tensor = torch::tensor(ids, torch::kFloat32).sub_(torch::mean(torch::tensor(ids, torch::kFloat32))).div_(torch::std(torch::tensor(ids, torch::kFloat32)));
    torch::Tensor days_tensor = torch::tensor(days, torch::kFloat32).sub_(torch::mean(torch::tensor(days, torch::kFloat32))).div_(torch::std(torch::tensor(days, torch::kFloat32)));
    torch::Tensor months_tensor = torch::tensor(months, torch::kFloat32).sub_(torch::mean(torch::tensor(months, torch::kFloat32))).div_(torch::std(torch::tensor(months, torch::kFloat32)));
    torch::Tensor sales_tensor = torch::tensor(sales, torch::kFloat32).sub_(torch::mean(torch::tensor(sales, torch::kFloat32))).div_(torch::std(torch::tensor(sales, torch::kFloat32)));

    return std::make_tuple(ids_tensor, days_tensor, months_tensor, sales_tensor);
}





class SalesPredictionModel : public torch::nn::Module {


public:
    SalesPredictionModel() {
        lstm1 = register_module("lstm1", torch::nn::LSTM(torch::nn::LSTMOptions(input_size, hidden_size).num_layers(1).batch_first(true)));
        linear = register_module("linear", torch::nn::Linear(hidden_size, output_size));
    }

    torch::Tensor forward(torch::Tensor id_tensor, torch::Tensor days_tensor, torch::Tensor months_tensor) {
        auto input_tensor = torch::stack({id_tensor, days_tensor, months_tensor}).unsqueeze(0).transpose(1, 2);

        auto lstm_output = lstm1->forward(input_tensor);
        auto lstm_out = std::get<0>(lstm_output).squeeze(0);
        return linear->forward(lstm_out.unsqueeze(0));
    }






    torch::nn::LSTM lstm1 = nullptr;
    torch::nn::Linear linear = nullptr;

\
};







int main(int argc, char* argv[]) {

  QApplication a(argc, argv);
  std::vector<std::tuple<int, int, int, float>> data =  getfile();


    auto [id, days, months, sales] = normalize_data(data);
    auto dataset = torch::data::datasets::TensorDataset({id, days, months, sales});
    auto data_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(
        std::move(dataset), torch::data::DataLoaderOptions().batch_size(64));


    SalesPredictionModel model;

    torch::optim::Adam optimizer(model.parameters(), torch::optim::AdamOptions(learning_rate));
    torch::nn::MSELoss loss;



    std::cout<<"load model?(1,0)"<<std::endl;
    bool f;
    std::cin>>f;
    if (f==1) {
        torch::serialize::InputArchive archive;
            archive.load_from("model.pt");
            model.load(archive);
    }


        std::cout<<"continue learning?(1,0)"<<std::endl;
        bool g;
        std::cin>>g;

        if (g==1) {

            for (int epoch = 0; epoch < num_epochs; ++epoch) {


                for (auto& batch : *data_loader) {

                    auto id_batch = batch.data()[0];
                    auto days_batch = batch.data()[1];
                    auto months_batch = batch.data()[2];

                    auto sales_batch1 = batch.data()[3];
                    auto sales_batch = torch::stack({sales_batch1}).unsqueeze(2);


                    optimizer.zero_grad();


                    auto predictions = model.forward(id_batch, days_batch, months_batch);
                    auto l = loss(predictions, sales_batch);


                    l.backward();
                    optimizer.step();



                    std::cout << "epoch: " << epoch<<"  loss:"<< l.item<float>() << std::endl;

                }

            }
            std::cout<<"trening finished, save?(1,0)"<<std::endl;
            bool h;
            std::cin>>h;
            if (h ==1) {
                torch::serialize::OutputArchive archive;
                    model.save(archive);
                    archive.save_to("model.pt");
            }

        }




    model.eval();

    for (int i = 3; i <= 5; i++) {
            for (int j = 1; j <= 30; j++) {
                torch::Tensor test_id = torch::tensor({8}, torch::kFloat32);
                torch::Tensor test_day = torch::tensor({j}, torch::kFloat32);
                torch::Tensor test_month = torch::tensor({i}, torch::kFloat32);
                torch::Tensor predicted_output = model.forward(test_id, test_day, test_month);
                writeFile(8, j, i,predicted_output.item<float>() );

                std::cout << "Output: " << predicted_output.item<float>() << std::endl;
            }

    }
    return 0;

}


/*
 *
 *     for (int i = 1; i <= 12; i++) {
        for (int j = 1; j <= 28; j++) {
            test_inputs.push_back({8, j, i});
        }
    }

 for (const auto& test_input : test_inputs) {
    torch::Tensor input_tensor = torch::tensor({std::get<0>(test_input), std::get<1>(test_input), std::get<2>(test_input)}, torch::kFloat32);
    torch::Tensor predicted_demand = test_net.forward(input_tensor);
    writeFile(std::get<0>(test_input), std::get<1>(test_input), std::get<2>(test_input), predicted_demand.item<float>()*100);
    std::cout << "For input: {" << std::get<0>(test_input) << ", " << std::get<1>(test_input) << ", " << std::get<2>(test_input)
              << "}, Predicted Demand: " << predicted_demand.item<float>() << std::endl;
}
*/
