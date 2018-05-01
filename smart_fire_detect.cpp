#include "smart_fire_detect.h"

smart_fire_detect::smart_fire_detect(const char * file_name, int len)
{          
    strncpy(m_input_file_name, file_name, len);
    m_input_file_name[len] = '\0';

	m_rgb_img = imread(m_input_file_name);
	resize(m_rgb_img, m_rgb_img, cv::Size(400, 400));
	cvtColor(m_rgb_img, m_hsv_img, COLOR_BGR2HSV);
	cvtColor(m_rgb_img, m_ycrcb_img, COLOR_BGR2YCrCb);
	split(m_hsv_img, split_hsv_img);
	split(m_rgb_img, split_rgb_img);
	split(m_ycrcb_img, split_ycrcb_img);
	m_step_value = (m_rgb_img.cols > m_rgb_img.rows ? m_rgb_img.cols : m_rgb_img.rows) / 40;

}
smart_fire_detect::smart_fire_detect(Mat  src_img)
{

	m_rgb_img = src_img;
	resize(m_rgb_img, m_rgb_img, cv::Size(400 ,400));
	cvtColor(m_rgb_img, m_hsv_img, COLOR_BGR2HSV);
	cvtColor(m_rgb_img, m_ycrcb_img, COLOR_BGR2YCrCb);
	split(m_hsv_img, split_hsv_img);
	split(m_rgb_img, split_rgb_img);
	split(m_ycrcb_img, split_ycrcb_img);
	m_step_value = (m_rgb_img.cols > m_rgb_img.rows ? m_rgb_img.cols : m_rgb_img.rows) / 40;
}

smart_fire_detect::smart_fire_detect(smart_fire_detect & src_img)
{

}
smart_fire_detect::~smart_fire_detect()
{

}
int smart_fire_detect::show_img_(char* windows_name, Mat show_img)
{
	namedWindow(windows_name, WINDOW_NORMAL);
	imshow(windows_name, show_img);
	return 0;
}

Mat smart_fire_detect::get_v_mask_Img()
{
	if (v_mask_img.empty())
	{
		Mat new_Image(split_hsv_img[2].rows, split_hsv_img[2].cols, CV_8UC1);
		v_mask_img = new_Image;
		threshold(split_hsv_img[2], v_mask_img, V_THRESHOLD_VALUE, 255, THRESH_BINARY);
		return v_mask_img;
	}
	else
		return v_mask_img;
}
Mat smart_fire_detect::get_binary_img()
{
	if (binary_img.empty())
	{
		Mat fire_center_img = get_fire_center_img();
		Mat fire_edge_img = get_fire_edge_img();	

		Mat binary_img_(fire_center_img.rows, fire_center_img.cols, CV_8UC1, Scalar(1));
		//cout << binary_img_ << endl;
		for (int i = 0; i < fire_edge_img.cols - m_step_value ? 1 : (i > fire_edge_img.cols - m_step_value && i < fire_edge_img.cols); i = i + m_step_value/2)
		{
			for (int j = 0; j < fire_edge_img.rows - m_step_value ? 1 : (j > fire_edge_img.rows - m_step_value && j < fire_edge_img.rows); j = j + m_step_value/2)
			{
				int real_x = 0, real_y = 0;
				real_x = i;
				real_y = j;
				if (j > fire_edge_img.rows - m_step_value)
				{
					real_y = j - m_step_value;
				}
				if (i > fire_edge_img.cols - m_step_value)
				{
					real_x = i - m_step_value;
				}

				Mat edge_split_image = fire_edge_img(cv::Rect(real_x, real_y, m_step_value, m_step_value));
				Mat center_split_image = fire_center_img(cv::Rect(real_x, real_y, m_step_value, m_step_value));
				Mat rgb_split_image = m_rgb_img(cv::Rect(real_x, real_y, m_step_value, m_step_value));
				
				Mat current_binary_img;
				get_current_binary(current_binary_img, edge_split_image, center_split_image, rgb_split_image);

				current_binary_img.copyTo(binary_img_(Rect(real_x, real_y, m_step_value, m_step_value)));
			}
		}
		is_fire_flag = fire_probability > FIRE_DETECT_THRSH;
		binary_img = binary_img_;
		return binary_img;
	}
	else
		return binary_img;
}
Mat smart_fire_detect::get_result_img(detect_config param_strut)
{
	result_img = m_rgb_img.clone();
	for (int i = 0; i < m_rgb_img.cols - m_step_value ? 1 : (i > m_rgb_img.cols - m_step_value && i < m_rgb_img.cols); i = i + m_step_value)
	{
		for (int j = 0; j < m_rgb_img.rows - m_step_value ? 1 : (j > m_rgb_img.rows - m_step_value && j < m_rgb_img.rows); j = j + m_step_value )
		{
			int real_x = 0, real_y = 0;
			real_x = i;
			real_y = j;
			if (j > m_rgb_img.rows - m_step_value)
			{
				real_y = j - m_step_value;
			}
			if (i > m_rgb_img.cols - m_step_value)
			{
				real_x = i - m_step_value;
			}

			Mat binary_split_img = binary_img(cv::Rect(real_x, real_y, m_step_value, m_step_value));
			if (binary_img.empty())
				return binary_img;
			int fire_pixs_num = 0;
			for (int j = 0; j < binary_split_img.rows; j++)
			{
				for (int i = 0; i < binary_split_img.cols; i++)
				{
					if (binary_split_img.at<uchar>(j, i) == 255)
						fire_pixs_num++;
				}
			}
			if (fire_pixs_num > binary_split_img.rows * binary_split_img.cols * 0.003)
				rectangle(result_img, cv::Rect(i, j, m_step_value, m_step_value), cvScalar(0, 0, 255), 5);


		}
	}
	return result_img;
}
Mat smart_fire_detect::get_fire_center_img()
{
	Mat fire_center_img(m_rgb_img.rows, m_rgb_img.cols, CV_8UC1);
	for (int j = 0; j < m_rgb_img.rows; j++)
	{
		for (int i = 0; i < m_rgb_img.cols; i++)
		{
			if (m_rgb_img.at<Vec3b>(j, i)[0] < 199  && m_rgb_img.at<Vec3b>(j, i)[1]  < m_rgb_img.at<Vec3b>(j, i)[2]  && m_rgb_img.at<Vec3b>(j, i)[1] > 201 && m_rgb_img.at<Vec3b>(j, i)[2] > 240)
			{
				fire_center_img.at<uchar>(j, i) = 255;
			}
			else
			{
				fire_center_img.at<uchar>(j, i) = 0;
			}
		}
	}
	return fire_center_img;
}
Mat smart_fire_detect::get_fire_edge_img()
{
	Mat fire_edge_img(m_rgb_img.rows, m_rgb_img.cols, CV_8UC1);
	for (int j = 0; j < m_rgb_img.rows; j++)
	{
		for (int i = 0; i < m_rgb_img.cols; i++)
		{
			if (m_rgb_img.at<Vec3b>(j, i)[0] < 125 && m_rgb_img.at<Vec3b>(j, i)[1]  < m_rgb_img.at<Vec3b>(j, i)[2] && m_rgb_img.at<Vec3b>(j, i)[1] < 201 && m_rgb_img.at<Vec3b>(j, i)[1] > 100 && m_rgb_img.at<Vec3b>(j, i)[2] > 200)
			{
				fire_edge_img.at<uchar>(j, i) = 255;
			}
			else
			{
				fire_edge_img.at<uchar>(j, i) = 0;
			}
		}
	}
	return fire_edge_img;

}


Mat smart_fire_detect::get_current_binary(Mat &current_binary_img, Mat edge_img, Mat center_img, Mat rgb_img)
{
	//edge_img.copyTo(current_binary_img);
	current_binary_img.create(edge_img.rows, edge_img.cols, CV_8UC1);
	int center_pixs_num = 0;
	int edge_pixs_num = 0;
	for (int j = 0; j < center_img.rows; j++)
	{
		for (int i = 0; i < center_img.cols; i++)
		{
			current_binary_img.at<uchar>(j, i) = 0;
			if (center_img.at<uchar>(j, i) == 255)
				center_pixs_num++;
			if (edge_img.at<uchar>(j, i) == 255)
				edge_pixs_num++;		
		}
	}
//	cout << "center_pixs_num : " << center_pixs_num << "  edge_pixs_num: " << edge_pixs_num << endl;
	//如果同时存在火芯和火苗，则认为疑似火焰

	if (center_pixs_num > center_img.rows * center_img.cols * 0.003 && edge_pixs_num > center_img.rows * center_img.cols * 0.003)
	{

		fire_probability++;
		//如果存在高亮部分可能是火中心（中心多亮度特别高，色彩不明显），将其填充进去
		for (int j = 0; j < rgb_img.rows; j++)
		{
			for (int i = 0; i < rgb_img.cols; i++)
			{
				//将火芯和火苗部分叠加
				if (edge_img.at<uchar>(j, i) == 255 || center_img.at<uchar>(j, i) == 255)
				{
					current_binary_img.at<uchar>(j, i) = 255;
				}
				int R = rgb_img.at<Vec3b>(j, i)[2];
				int G = rgb_img.at<Vec3b>(j, i)[1];
				int B = rgb_img.at<Vec3b>(j, i)[0];

				if (R > 230 && G > 230 && B > 230 )
				{
					current_binary_img.at<uchar>(j, i) = 255;
				}

			}
		}
	}
	//测试部分
	/*
	for (int j = 0; j < rgb_img.rows; j++)
	{
		for (int i = 0; i < rgb_img.cols; i++)
		{
			int R = rgb_img.at<Vec3b>(j, i)[2];
			int G = rgb_img.at<Vec3b>(j, i)[1];
			int B = rgb_img.at<Vec3b>(j, i)[0];
			if (max(max(R, G), B)> 230 && edge_img.at<uchar>(j, i) == 255)
			{
				current_binary_img.at<uchar>(j, i) = 255;
			}
		}
	}
	*/
	return current_binary_img;
}
Mat smart_fire_detect::get_dim_yellow_fire_img()
{
	Mat yellow_field_img(m_rgb_img.rows, m_rgb_img.cols, CV_8UC1);

	for (int j = 0; j < m_rgb_img.rows; j++)
	{
		for (int i = 0; i < m_rgb_img.cols; i++)
		{
			int R = m_rgb_img.at<Vec3b>(j, i)[2];
			int G = m_rgb_img.at<Vec3b>(j, i)[1];
			int B = m_rgb_img.at<Vec3b>(j, i)[0];

			if (R > 1.5 * G && (R -G ) > 1.2 *(G -B) && R > 140 && G > 50 && R -G < 110)
			{
				yellow_field_img.at<uchar>(j, i) = 255;
			}
			else
			{
				yellow_field_img.at<uchar>(j, i) = 0;
			}
		}
	}
	return yellow_field_img;

}
