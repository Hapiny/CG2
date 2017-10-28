#pragma once
#include <tuple>
#include <cmath>
#include <vector>
#include "matrix.h"

#define PI 3.141592653589793

//количество сегментов разбиения отрезка [-PI,PI]
int hist_segment_num = 32;

//количество клеток по горизонтали и по вертикали
int num_of_horizontal_cells = 10;
int num_of_vertical_cells = 8;

typedef Matrix<std::tuple<int, int, int>> Image;


//Из объекта типа BMP получаем Image, аналогично, как это было сделано в первом задании 
Image bmp2image(BMP *);

//Получаем матрицу яркости пикселей
Matrix<float> grayscale(const Image &);

//Получаем I_x и I_y
Matrix<float> horizontal_sobel(const Matrix<float> &);
Matrix<float> vertical_sobel(const Matrix<float> &);

//mod_grad = sqrt((I_x)^2 + (I_y)^2) 
Matrix<float> mod_gradient(const Matrix<float> &, const Matrix<float> &);

//angel_grad = atan2(I_y/I_x)
Matrix<float> ang_gradient(const Matrix<float> &, const Matrix<float> &); 

//Евклидова норма для вектора
void normalize(std::vector<float> &);

//для каждой клетки вычисляем гистрограмму
std::vector<float> histogram(const Matrix<float> &, const Matrix<float> &, const Matrix<float> &);

std::vector<float> HOG(const Matrix<float> &);

#include "HOG.hpp"
