#include <iostream>  
#include "EPR1.h"
#include "stdafx.h"

using namespace std;
using namespace cv;

EPR::EPR(Mat &src)
{
	if ((src.cols > 1000) | (src.rows > 1000))
	{
		cout << "����ͼƬ̫����С��800*800��" << endl << endl;
		srcImage = Mat::zeros(300, 399, src.type());
		resize(src, srcImage, srcImage.size());
		tempdstImage = Mat::zeros(srcImage.size(), src.type());
		dstImage = Mat::zeros(srcImage.size(), src.type());
		boxImage = Mat::zeros(srcImage.size(), src.type());
		Plate.clear();
		charROI.clear();
	}
	else
	{
		srcImage = src.clone();
		tempdstImage = Mat::zeros(srcImage.size(), src.type());
		dstImage = Mat::zeros(srcImage.size(), src.type());
		boxImage = Mat::zeros(srcImage.size(), src.type());
		Plate.clear();
		charROI.clear();
	}
}

void EPR::setParameters(Size element3, Size element4, double whRatiotlow, double whRatiotup, double boxangle1, double rect_degree)
{
	g_rect_degree = rect_degree;
	g_whRatiotlow = whRatiotlow;
	g_whRatiotup = whRatiotup;
	boxangle = boxangle1;
	element3size = element3;
	element4size = element4;
	times = 0;
	mode = 1;
}

void EPR::preprocess(int gaussian_size, int elementclose_width, int elementclose_height, int elementtop_size)
{
	;//[1]��ȡԭͼ�񣬲�תΪ�Ҷ�ͼ
	cvtColor(srcImage, tempdstImage, COLOR_BGR2GRAY);

	//[2]��˹�˲���ֱ��ͼ����
	GaussianBlur(tempdstImage, tempdstImage, Size(gaussian_size, gaussian_size), 0, 0);

	//[3]��ñ�任���ñ�任
	Mat tempdstImage_t, tempdstImage_b;
	Mat element = getStructuringElement(MORPH_RECT, Size(elementtop_size, elementtop_size));
	morphologyEx(tempdstImage, tempdstImage_t, MORPH_TOPHAT, element);
	morphologyEx(tempdstImage, tempdstImage_b, MORPH_TOPHAT, element);
	add(tempdstImage, tempdstImage_t, tempdstImage);
	subtract(tempdstImage, tempdstImage_b, tempdstImage);
	//imshow("1", tempdstImage);  

	//[4]��sobel���Ӽ�ⴹֱ��Ե
	Mat dx;
	Sobel(tempdstImage, dx, CV_16S, 1, 0);
	convertScaleAbs(dx, tempdstImage);

	//[5]��ֵ��
	threshold(tempdstImage, tempdstImage, 0, 255, THRESH_BINARY | THRESH_OTSU);

	//[6]������
	Mat element2 = getStructuringElement(MORPH_RECT, Size(elementclose_width, elementclose_height));
	morphologyEx(tempdstImage, tempdstImage, MORPH_CLOSE, element2);
	//imshow("1", tempdstImage);
}

void EPR::searchPlate(Ptr<cv::ml::SVM> &svm)
{
	//��ʼ����������
	vector<vector<Point>> counters;//��������
	vector<Vec4i> hierarchy;

	float contour_Area;//�������
	float rect_degree;//�������ζ�
	float wh_ratio;//���������
	float longside, shortside;//���ο򳤱ߺͶ̱�
	float angle;//���ο�Ƕ�

	//����������
	Mat element3 = getStructuringElement(MORPH_RECT, element3size);
	Mat element4 = getStructuringElement(MORPH_RECT, element4size);
	morphologyEx(tempdstImage, dstImage, MORPH_OPEN, element4);
	morphologyEx(dstImage, dstImage, MORPH_OPEN, element3);
	//imshow("2", dstImage);

	//��ȡ����
	findContours(dstImage, counters, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	srcImage.copyTo(boxImage);
	ROI.clear();
	for (int i = 0, j = 0; i < counters.size(); i++)
	{
		RotatedRect tempRect = minAreaRect(Mat(counters[i]));
		contour_Area = float(fabs(contourArea(counters[i])));

		//���ζȼ���
		rect_degree = contour_Area / ((tempRect.size.height)*(tempRect.size.width));
		//����ȼ���
		longside = (tempRect.size.width > tempRect.size.height) ? tempRect.size.width : tempRect.size.height;
		shortside = (tempRect.size.width < tempRect.size.height) ? tempRect.size.width : tempRect.size.height;
		wh_ratio = longside / shortside;
		//�Ƕȼ��� �ж���б�ǶȻ�����б�Ƕ�
		if (tempRect.size.width > tempRect.size.height)
			angle = fabs(tempRect.angle);//��б
		else
			angle = 90 + tempRect.angle;//��б

		float flag = 0;
		//�������ϼ�������ɸѡ��������
		if ((rect_degree > g_rect_degree) & (wh_ratio > g_whRatiotlow) & (wh_ratio < g_whRatiotup)&(angle < boxangle))
		{
			ROIExtract(tempRect, svm, flag);
			if (flag == 1)
			{
				drawbox(tempRect);
				cout << "��ѡ����" << endl;
				cout << "���ζ�=" << rect_degree << "  " << "���ο򳤿��=" << wh_ratio << " " << "���ο�Ƕ�=" << tempRect.angle << endl << endl;
			}
		}
	}

	if ((ROI.data() == 0)& (mode == 1))
	{
		changeElementSize(times++);
		if (times == 8)
			mode = 0;
		searchPlate(svm);

	}

}


//������������������������������������ROI��ȡ����תУ������������������������������������������
void EPR::ROIExtract(RotatedRect &box, Ptr<cv::ml::SVM> &svm, float flag)
{
	Mat dst, dst2, dst3, rot_map;
	int width, height;//width��box�ĳ��ߣ�height�Ƕ̱�
	Rect tempbox = box.boundingRect();
	boxsafe(tempbox);
	int rotmatsize = (tempbox.width > tempbox.height) ? tempbox.width : tempbox.height;
	dst = Mat::zeros(2 * rotmatsize, 2 * rotmatsize, srcImage.type());

	//��б���ο�����ĵ�ӳ�䵽dst����
	int a = int(box.center.x) - tempbox.tl().x;
	int b = int(box.center.y) - tempbox.tl().y;
	a = rotmatsize - a;
	b = rotmatsize - b;
	if (a < 0)
		a = 0;
	if (b < 0)
		b = 0;
	dst(Rect(a, b, tempbox.width, tempbox.height)) = srcImage(tempbox) + 0.0;
	;
	//��תROIͼ��
	Point newcenter(rotmatsize, rotmatsize);
	//�ж���б������б
	if (box.size.width > box.size.height)
	{
		rot_map = getRotationMatrix2D(newcenter, box.angle, 1);//��б
		width = int(box.size.width);
		height = int(box.size.height);
	}
	else
	{
		rot_map = getRotationMatrix2D(newcenter, (90 + box.angle), 1);//��б
		width = int(box.size.height);
		height = int(box.size.width);
	}
	warpAffine(dst, dst, rot_map, dst.size());
	dst2 = dst(Rect(rotmatsize - width / 2, rotmatsize - height / 2, width, height));
	dst3 = Mat::zeros(36, 136, srcImage.type());
	resize(dst2, dst3, dst3.size());
	Mat feature;
	extractfeature(dst3, feature);
	flag = svm->predict(feature);
	if (flag == 1)
	{
		ROI.push_back(dst3);
	}
}

//���������������������������������������������ο򡪡�����������������������������������������
void EPR::drawbox(RotatedRect &box)
{
	Point2f vertex[4];
	box.points(vertex);
	for (int i = 0; i < 4; i++)
		line(boxImage, vertex[i], vertex[(i + 1) % 4], Scalar(100, 220, 211), 2, 8);
}

//��������������������������������������������ԭͼ�Ƿ���硪������������������������������
void EPR::boxsafe(Rect &box)
{
	int width = srcImage.cols;
	int height = srcImage.rows;
	int tl_new_x, tl_new_y;
	tl_new_x = box.tl().x;
	tl_new_y = box.tl().y;
	if (box.tl().x > width)
		tl_new_x = width;
	if (box.tl().x < 0)
		tl_new_x = 0;
	if (box.tl().y > height)
		tl_new_y = height;
	if (box.tl().y < 0)
		tl_new_y = 0;

	if (box.br().x > width)
		tl_new_x = tl_new_x - (box.br().x - width);
	if (box.br().y > height)
		tl_new_y = tl_new_y - (box.br().y - height);

	Rect newbox(tl_new_x, tl_new_y, box.width, box.height);
	box = newbox;
}

//����������������������������������ȡˮƽͶӰ�ʹ�ֱͶӰ�����㡪������������������������������
void EPR::extractfeature(Mat &src, Mat &feature)
{
	Mat dst;
	if (src.type() == 0)
		dst = src.clone();
	else
		cvtColor(src, dst, COLOR_BGR2GRAY);
	threshold(dst, dst, 0, 1, THRESH_BINARY | THRESH_OTSU);
	//ˮƽͶӰ����
	Mat horizonf = Mat::zeros(1, dst.rows, CV_8UC1);
	for (int i = 0; i < dst.rows; i++)
	{
		for (int j = 0; j < dst.cols; j++)
		{
			horizonf.at<uchar>(0, i) = horizonf.at<uchar>(0, i) + dst.at<uchar>(i, j);
		}
	}
	//��ֱͶӰ����
	Mat verticalf = Mat::zeros(1, dst.cols, CV_8UC1);
	for (int j = 0; j < dst.cols; j++)
	{
		for (int i = 0; i < dst.rows; i++)
		{
			verticalf.at<uchar>(0, j) = verticalf.at<uchar>(0, j) + dst.at<uchar>(i, j);
		}
	}
	//����ֱ��ˮƽͶӰ����
	hconcat(horizonf, verticalf, feature);
	feature.convertTo(feature, CV_32FC1);
}

//�����������������������������������Ľṹ��Ԫ�ش�С��������������������������������
void EPR::changeElementSize(int times)
{
	if (times < 5)
	{
		if (times < 4)
			element3size.height = element3size.height + 4;
		else
		{
			element3size.height = 13;
			element3size.height = element3size.height - 4;
		}
	}
	else
	{
		element3size.height = 11;
		if (times < 7)
			element4size.width = element4size.width + 4;
		else
		{
			element4size.width = 15;
			element4size.width = element4size.width - 4;
		}
	}

}
//����������������������������������ʾ����ROI���򡪡�����������������������������
void EPR::ROIShow()
{
	if (ROI.size() == 0)
		cout << "δ��⵽��������" << endl;
	else
	{
		Mat allROI = Mat::zeros(36, int(136 * ROI.size()), srcImage.type());
		for (int i = 0; i < ROI.size(); i++)
		{
			ROI[i].copyTo(allROI(Rect(i * 136, 0, 136, 36)));
		}
		imshow("����", allROI);
	}
}

//����������������������������������ȡ����ROI������ַ���������������������������������
void EPR::charObtain(Ptr<cv::ml::SVM> &svm_char)
{
	for (int ROInumber = 0; ROInumber < ROI.size(); ROInumber++)
	{
		//��������Ԥ����
		Mat intputROIGray;
		cvtColor(ROI[ROInumber], intputROIGray, COLOR_BGR2GRAY);//ת�Ҷ�
		GaussianBlur(intputROIGray, intputROIGray, Size(3, 3), 0, 0);//ģ��
		Mat element = getStructuringElement(MORPH_RECT, Size(23, 3));
		morphologyEx(intputROIGray, intputROIGray, MORPH_TOPHAT, element);//��ñ�任
		threshold(intputROIGray, intputROIGray, 0, 255, THRESH_BINARY | THRESH_OTSU);//��ֵ��

		//���Ʒָ�
		//Ѱ����������ȥ��С������
		vector<vector<Point>> contours;//��������
		vector<Vec4i> hierarchy;
		vector<Rect> boundRect_all;
		vector<Rect> boundRect(7);
		findContours(intputROIGray, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		for (int i = 0, j = 0; i < contours.size(); i++)
		{
			Rect tempRect = boundingRect(Mat(contours[i]));
			if ((tempRect.height > 15) & (tempRect.width > 3))
			{
				boundRect_all.push_back(tempRect);
			}
		}
		vector<Rect> boundRect_all_sort(boundRect_all.size());
		vector<Mat> tempcharROI;

		//���������6�����ϣ���Ϊ����������ĸ��
		if (boundRect_all.size() > 5)
		{
			sortRect(boundRect_all, boundRect_all_sort);//������������x�����С
			charExtract(intputROIGray, boundRect_all_sort, tempcharROI, boundRect);//��ȡ�����ַ�ͼ��

			//��ʼԤ�������ַ�ͼ��
			String tempPlate;
			tempPlate.push_back(63);
			for (int i = 1; i < tempcharROI.size(); i++)
			{
				Mat feature;
				extractfeature(tempcharROI[i], feature);
				float response = svm_char->predict(feature);
				//��int��ת���ɶ�Ӧ�����ֺ���ĸ
				if (response < 10)
					response = response + 48;
				else
				{
					if (response < 18)
						response = response + 55;
					else
					{
						if (response < 23)
							response = response + 56;
						else
							response = response + 57;
					}
				}
				tempPlate = tempPlate + char(response);
				//rectangle(ROI[ROInumber], boundRect[i].tl(), boundRect[i].br(), Scalar(222, 222, 100));
			}
			Plate.push_back(tempPlate);
			charROI.push_back(tempcharROI);
		}
	}
}

//���������������������������������������ο����򡪡�������������������������������������
void EPR::sortRect(vector<Rect> &inputRect, vector<Rect> &outputRect)
{
	int rectnumber = int(inputRect.size());
	vector<int> sortRectTlx(rectnumber);
	vector<int> sortRectNumber(rectnumber);
	for (int i = 0; i < rectnumber; i++)
	{
		sortRectTlx[i] = inputRect[i].tl().x;
		sortRectNumber[i] = i;
	}

	//ð������
	int temp;
	for (int i = 1; i < rectnumber; i++)
	{
		for (int j = rectnumber - 1; j >= i; j--)
		{
			if (sortRectTlx[j] < sortRectTlx[j - 1])
			{
				temp = sortRectTlx[j - 1];
				sortRectTlx[j - 1] = sortRectTlx[j];
				sortRectTlx[j] = temp;

				temp = sortRectNumber[j - 1];
				sortRectNumber[j - 1] = sortRectNumber[j];
				sortRectNumber[j] = temp;
			}
		}
	}
	//�����������
	for (int i = 0; i < rectnumber; i++)
	{
		outputRect[i] = inputRect[sortRectNumber[i]];
	}
}

//�����������������������������������������ο����������ת����MATͼ�񡪡�������������������������������������
void EPR::charExtract(Mat &inputImage, vector<Rect> &inputRect, vector<Mat> &outputcharROI, vector<Rect> &outputRect)
{

	//�ҵ��ڶ����ַ�
	int rectnumber = int(inputRect.size());
	int count;
	for (int i = 0; i < rectnumber; i++)
	{
		if ((inputRect[i].tl().x < 50) & (inputRect[i].tl().x > 10))
		{
			outputRect[1] = inputRect[i];
			count = i;
			break;
		}
	}

	//�ҵ���һ���ַ�
	int x = outputRect[1].tl().x - 18;
	if (x < 0)
		x = 0;
	outputRect[0] = Rect(x, outputRect[1].tl().y, outputRect[1].width, outputRect[1].height);

	//�ҵ�ʣ���ַ�
	int width_new, tl_new_x;
	for (int i = 2; i < 7; i++)
	{
		if (inputRect[count + i - 1].width < outputRect[1].width / 2)
		{
			tl_new_x = inputRect[count + i - 1].tl().x - (outputRect[1].width - inputRect[count + i - 1].width) / 2;
			if ((tl_new_x + outputRect[1].width) > inputImage.cols)
				width_new = inputImage.cols - tl_new_x;
			else
				width_new = outputRect[1].width;
			Rect newbox(tl_new_x, inputRect[count + i - 1].tl().y, width_new, inputRect[count + i - 1].height);
			outputRect[i] = newbox;
		}
		else
			outputRect[i] = inputRect[count + i - 1];
	}

	//�����ο�����򵥶����������
	//��ֹ���
	int count2;
	if (inputRect.size() - count > 6)
		count2 = 7;
	else
		count2 = inputRect.size() - count + 1;

	for (int i = 0; i < count2; i++)
	{
		Mat tempROI = Mat::zeros(20, 20, inputImage.type());
		Mat tempROI2 = Mat::zeros(20, 10, inputImage.type());
		Mat tempROI3 = inputImage(outputRect[i]);
		resize(tempROI3, tempROI2, tempROI2.size());
		tempROI(Rect(5, 0, 10, 20)) = tempROI2 + 0;
		outputcharROI.push_back(tempROI);
	}
}