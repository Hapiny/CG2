Image bmp2image(BMP * image) //BMP -> Image
{
	int height = image->TellHeight(); 
	int width = image->TellWidth();
	RGBApixel pixel;
	Image result = Image(height, width);
	for (int i = 0; i < height; i++) 
	{
		for (int j = 0; j < width; j++) 
		{
			pixel = image->GetPixel(j, i);
			result(i, j) = std::make_tuple(pixel.Red, pixel.Green, pixel.Blue);
		}
	}
	return result;
}


Matrix<float> grayscale(const Image & image)
{
	Matrix<float> result(image.n_rows,image.n_cols);
	int red, green, blue;
	for(uint i = 0; i < image.n_rows; i++)
	{
		for(uint j = 0; j < image.n_cols; j++)
		{
			std::tie(red, green, blue) = image(i,j);
			result(i,j) = 0.299 * red + 0.587 * green + 0.114 * blue;
		}
	}
	return result;
}

Matrix<float> horizontal_sobel(const Matrix<float> & source)
{ 	
	// -1 0 1
	int height = source.n_rows, width = source.n_cols;
	Matrix<float> result(height, width);
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
				if (j == 0)
				{	
					result(i,j) = source(i,1) - source(i,width-1);
				}	
				else if (j == (width - 1))
				{
					result(i,j) = source(i,0) - source(i,width-2);
				}
				else
				{	
					result(i,j) = source(i,j+1) - source(i,j-1); 
				}
		}
	}
	return result;
}

Matrix<float> vertical_sobel(const Matrix<float> & source)
{	
	// 1
	// 0
	//-1
	int height = source.n_rows, width = source.n_cols;
	Matrix<float> result(height, width);
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
				if (i == 0) 
				{
					result(i,j) = (1.) * source(height-1,j) + (-1.) * source(1,j);
				}	
				else if (i == (height-1))
				{
					result(i,j) = (1.) * source(height-2,j) + (-1.) * source(0,j);
				}
				else 
				{
					result(i,j) = (-1.) * source(i-1,j) + (1.) * source(i+1,j); 
				}
			}
	}
	return result;
}
Matrix<float> mod_gradient(const Matrix<float> & I_x,const Matrix<float> & I_y)
{
	int height, width;
	height = I_x.n_rows;
	width = I_x.n_cols;

	Matrix<float> result(height,width);
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			result(i,j) = sqrt(pow(I_x(i,j),2) + pow(I_y(i,j),2));
		}
	}
	return result;
}

Matrix<float> ang_gradient(const Matrix<float> & I_x,const Matrix<float> & I_y)
{
	int height = I_x.n_rows, width = I_y.n_cols;

	Matrix<float> result(height,width);
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			result(i,j) = atan2(I_y(i,j), I_x(i,j));
		}
	}
	return result;
}

void normalize(std::vector<float> &histogram)
{
	float norm = 0;
	for(uint i = 0; i < histogram.size(); i++)
		norm += pow(histogram[i],2);
	norm = sqrt(norm);
	if (norm <= 0) return;
	for(uint i = 0; i < histogram.size(); i++)
		histogram[i] /= norm;
	return;

}

std::vector<float> histogram(const Matrix<float> &cell, const Matrix<float> &mod, const Matrix<float> &ang)
{
	std::vector<float> result(hist_segment_num, 0);
	uint idx;
	for(uint i = 0; i < cell.n_rows; i++)
	{
		for(uint j = 0; j < cell.n_cols; j++)
		{
			idx = (ang(i,j) + PI) * hist_segment_num / (2 * PI);
			result[idx] += mod(i,j);
		}
	}
	normalize(result);
	return result;
}

std::vector<float> HOG(const Matrix<float> &image)
{
	//Функция будет вызвана для изображения после обработки функцией grayscale()
	Matrix<float> horizontal = horizontal_sobel(image);
	Matrix<float> vertical = vertical_sobel(image);
	Matrix<float> module = mod_gradient(vertical, horizontal);
	Matrix<float> angle = ang_gradient(vertical, horizontal);

	//Вычисляем размеры клетов с учетом кол-ва горизонтальных и вертикальных клеток,
	//с учетом размера изображения
	int h_step = round((1. * image.n_rows) / num_of_vertical_cells);
	int w_step = round((1. * image.n_cols) / num_of_horizontal_cells);
//	std::cout << h_step << " " << w_step << std::endl;
	std::vector<float> result;
	int height = image.n_rows, width = image.n_cols;
	int prev_h_step, prev_w_step;

//	int count = 1;

	int x_end = 0, y_end = 0;
	for(int i = 0; i <= height; i = i + h_step)
	{
		x_end = i + h_step;
		prev_h_step = h_step;

		if (x_end > height) h_step -= (x_end - height);

//		std::cout << count << ") " << "i = " << i << std::endl;
		for(int j = 0; j <= width; j = j + w_step)
		{
			y_end = j + w_step;
			prev_w_step = w_step;

			if (y_end > width) w_step -= (y_end - width);
				
			auto cell_hist = histogram(image.submatrix(i, j, h_step, w_step),  //клетка из матрицы grayscale-изображения
									   module.submatrix(i, j, h_step, w_step), //клетка из матрицы модуля градиента
									   angle.submatrix(i, j, h_step, w_step)); //клетка из матрицы направление градиента

//			std::cout << "j = "<< j << " "; 
			w_step = prev_w_step;
			result.insert(result.end(), cell_hist.begin(), cell_hist.end());
		}
//		std::cout << std::endl;
//		count++;
		h_step = prev_h_step;
	}
//	std::cout << std::endl;
	return result;
}