#ifndef _COMMON_H_ 
#define _COMMON_H_
#include <iostream>
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <stdint.h>
#include <string.h>

using namespace cv;
using namespace std;

int threshold_by_fixed_value(Mat input_img, Mat &output_img);
int threshold_by_H_value(Mat input_img, Mat &output_img);
struct detect_config
{
	bool  save_flag;//表示是否需要保存
	string save_addr;//若发现明火，保存路径
};
bool checkDrive(string drives, string userPath);
#define MAX_FILE_SIZE 1024  //最大文件路径长度
#define H_THRESHOLD_VALUE 15
#define V_THRESHOLD_VALUE 240
#define FIRE_DETECT_THRSH 2
#endif
