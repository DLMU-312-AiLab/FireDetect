
#include "common.h"
#include <algorithm>
class smart_fire_detect
{
public:
    smart_fire_detect(const char * file_name, int len);
	smart_fire_detect(Mat  src_img);
	smart_fire_detect(smart_fire_detect & src_img);

	~smart_fire_detect();

	/*
	@brief ：获取检测结果
	@param [out] 输出信息 msg为结构体指针，可能包含检测结果描述等，具体结构体未定
	@param [in]  配置信息，param_strut,可能包含是否保存等字段。未定
 	@return：疑似有火返回true，否则返回false
	*/
	bool get_result(string &msg);

	Mat get_result_img(detect_config param_strut);

	/*设置步长*/
    uint32_t set_step_value(uint32_t step_value){ m_step_value = step_value; }

	/*设置文件保存路径*/
    uint32_t set_output_file_addr(char * out_file_addr){ strncpy(m_output_file_name, out_file_addr, strlen(out_file_addr)); }

	/*获取当前的步长*/
    uint32_t  get_step_value() { return m_step_value; }

	/*显示图像*/
	int show_img_(char* windows_name, Mat img);
	/*
	@brief:通过RGB之间的关系找到临近火焰中心亮黄色的区域，将训练好的数值直接在此做阈值
	@ B通道数值小于199
	@ G通道数值大于201
	@ R通道数值大于240
	@ R通道数值大于G通道数值
	*/
	Mat get_fire_center_img();
	/*
	@brief:通过RGB之间的关系找到远离火焰中心的火焰边缘区域，将训练好的数值直接在此做阈值
	@ B通道数值小于125
	@ G通道数值大于100小于201
	@ R通道数值大于200
	@ R通道数值大于G通道数值
	*/
	Mat get_fire_edge_img();


	Mat get_dim_yellow_fire_img();


	char m_input_file_name[MAX_FILE_SIZE];			/* 传入 文件路径 */
	char m_output_file_name[MAX_FILE_SIZE];			/* 保存 文件路径 */
	int m_step_value{50};
public:
	Mat m_rgb_img;
	Mat m_hsv_img;
	Mat m_ycrcb_img;
	Mat result_img;


	vector<Mat> split_hsv_img; //存储H、S、V通道的单通道图像
	vector<Mat> split_rgb_img; //存储B、G、R通道的单通道图像
	vector<Mat> split_ycrcb_img; //存储B、G、R通道的单通道图像
	Mat get_v_mask_Img();
	Mat get_binary_img();
	int fire_probability;

	bool is_fire_flag{ false };
	Mat get_current_binary(Mat &result_img,Mat center_img,Mat edge_img,Mat rgb_img);
private:
	Mat binary_img;
	Mat h_mask_img;
	Mat v_mask_img;

};
