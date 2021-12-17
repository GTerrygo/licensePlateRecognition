#pragma once
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp> 
#include"opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml.hpp"



class EPR
{
public:
	// ��Ա����
	double g_rect_degree;
	double g_whRatiotlow;
	double g_whRatiotup;
	double boxangle;
	cv::Size element3size;//������1��ȥ�����С������
	cv::Size element4size;//������2�� ȥ������С������
	cv::Mat srcImage, tempdstImage, dstImage, boxImage;
	std::vector<cv::Mat> ROI;//��������ROI����
	int mode, times;//modeΪ0����������modeΪ1��ȷ������
	std::vector<cv::String> Plate; //���ƺ�������ĸ����
	std::vector<std::vector<cv::Mat>> charROI;//�����ַ�ROI����

	//��Ա����
	EPR() { ; };
	EPR(cv::Mat &src);
	void setParameters(cv::Size element3, cv::Size element4, double whRatiotlow = 2.7, double whRatiotup = 5.2, double boxangle1 = 50, double Rect_degree = 0.7);
	void preprocess(int gaussian_size = 5, int elementclose_width = 17, int elementclose_height = 5, int elementtop_size = 7);
	void searchPlate(cv::Ptr<cv::ml::SVM> &svm);
	void ROIExtract(cv::RotatedRect &box, cv::Ptr<cv::ml::SVM> &svm, float flag);//ROI��ȡ����תУ��
	void drawbox(cv::RotatedRect &box);//�����ο�
	void boxsafe(cv::Rect &box);//��ֹ���ο����
	void extractfeature(cv::Mat &src, cv:: Mat &feature);
	void changeElementSize(int times);
	void ROIShow();
	void charObtain(cv::Ptr<cv::ml::SVM> &svm_char);
	void sortRect(std::vector<cv::Rect> &inputRect, std::vector<cv::Rect> &outputRect);
	void charExtract(cv::Mat &inputImage, std::vector<cv::Rect> &inputRect, std::vector<cv::Mat> &outputcharROI, std::vector<cv::Rect> &outputRect);
};