#ifndef FILE_H
#define FILE_H
#include <torch/torch.h>
#include <iostream>
#include <vector>
#include <QFileDialog>
#include <QTextStream>
#include <QString>
#include <QDebug>
#include <QApplication>
#include <random>
#include <QDate>
#include <algorithm>
#include <unordered_map>



void writeFile(int prod, int day, int mou, float dem);

 std::vector<std::tuple<int, int, int, float>> getfile(int *prod);

#endif // FILE_H
