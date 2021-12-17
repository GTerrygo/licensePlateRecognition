#pragma once
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp> 
#include"opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml.hpp"



class EPR
{
public:
	// 成员变量
	double g_rect_degree;
	double g_whRatiotlow;
	double g_whRatiotup;
	double boxangle;
	cv::Size element3size;//开运算1，去除宽很小的物体
	cv::Size element4size;//开运算2， 去除长很小的物体
	cv::Mat srcImage, tempdstImage, dstImage, boxImage;
	std::vector<cv::Mat> ROI;//车牌区域ROI容器
	int mode, times;//mode为0快速搜索，mode为1精确搜索。
	std::vector<cv::String> Plate; //车牌号码与字母容器
	std::vector<std::vector<cv::Mat>> charROI;//车牌字符ROI容器

	//成员函数
	EPR() { ; };
	EPR(cv::Mat &src);
	void setParameters(cv::Size element3, cv::Size element4, double whRatiotlow = 2.7, double whRatiotup = 5.2, double boxangle1 = 50, double Rect_degree = 0.7);
	void preprocess(int gaussian_size = 5, int elementclose_width = 17, int elementclose_height = 5, int elementtop_size = 7);
	void searchPlate(cv::Ptr<cv::ml::SVM> &svm);
	void ROIExtract(cv::RotatedRect &box, cv::Ptr<cv::ml::SVM> &svm, float flag);//ROI提取并旋转校正
	void drawbox(cv::RotatedRect &box);//画矩形框
	void boxsafe(cv::Rect &box);//防止矩形框出界
	void extractfeature(cv::Mat &src, cv:: Mat &feature);
	void changeElementSize(int times);
	void ROIShow();
	void charObtain(cv::Ptr<cv::ml::SVM> &svm_char);
	void sortRect(std::vector<cv::Rect> &inputRect, std::vector<cv::Rect> &outputRect);
	void charExtract(cv::Mat &inputImage, std::vector<cv::Rect> &inputRect, std::vector<cv::Mat> &outputcharROI, std::vector<cv::Rect> &outputRect);
};