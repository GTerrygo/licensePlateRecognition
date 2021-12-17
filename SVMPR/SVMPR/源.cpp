#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/ml.hpp"
#include <iostream>


using namespace std;
using namespace cv;

int get_data(Mat& trainingImages, vector<int>& trainingLabels,int buf);
int get_tdata(Mat& trainingImages, vector<int>& trainingLabels, int buf);
string train_path = ".\\data\\data_char\\train\\";
string test_path = ".\\data\\data_char\\test\\";
string output_name = "char_svm2.xml";
int type = 0; //0:train char, 1:train plate
void exactfeature(Mat &src, Mat &feature);

int main()
{
	//获取训练数据
	cout << "——————————正在获取训练数据——————————" << endl;
	Mat classes;
	Mat trainingData;
	Mat trainingImages;
	vector<int> trainingLabels;
	int length = type == 0 ? 34 : 2;
	for (int i = 0; i < length; i++)
	{
		int flag = get_data(trainingImages, trainingLabels, i);
	}


	Mat(trainingImages).copyTo(trainingData);
	trainingData.convertTo(trainingData, CV_32FC1);
	Mat(trainingLabels).copyTo(classes);
	classes.convertTo(classes, CV_32SC1);

	//配置SVM训练器参数
	cout << "——————————正在训练数据——————————" << endl;
	Ptr<ml::SVM> svm = ml::SVM::create();
	svm->setType(ml::SVM::C_SVC);     //SVM类型
	svm->setKernel(ml::SVM::RBF);  //核函数，这里使用线性核
		//svm->setGamma(1);
		//svm->setC(10);
		//svm->setDegree(0);
	   //svm->setCoef0(0);
	   //svm->setNu(0);
	   //svm->setP(0);
	   //svm->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER,5000,0.01));
	   // train operation
	   //svm->train(trainingData, ml::SampleTypes::ROW_SAMPLE, classes);
	Ptr<ml::TrainData> td = ml::TrainData::create(trainingData, ml::SampleTypes::ROW_SAMPLE, classes);
	svm->trainAuto(td, 10);
	svm->save(output_name);
	cout << "——————————训练完毕，保存xml文件——————————" << endl;
	//获取测试 数据
	cout << "——————————正在获取测试数据——————————" << endl;
	Mat testingData;
	Mat testingImages;
	vector<int> testingLabels;
	for (int i = 0; i < length; i++)
	{
		int flag = get_tdata(testingImages, testingLabels, i);
	}
	Mat(testingImages).copyTo(testingData);
	testingData.convertTo(testingData, CV_32FC1);



	//读取训练参数
	cout << "——————————正在获取SVM训练数据——————————" << endl;
	//Ptr<cv::ml::SVM> svm;
	svm = ml::SVM::load(output_name);
	cout << "——————————SVM训练数据获取成功——————————" << endl << endl;

	//开始预测
	cout << "——————————开始预测——————————" << endl;
	float count=0;
	int totalCount = testingData.rows;
	for (int i = 0; i < totalCount; ++i) {
		Mat sample = testingData.row(i);
		float response = svm->predict(sample);
		cout << response << "  " << testingLabels[i] << endl;
		if (response == testingLabels[i])
		{
			count++; 
		}

	}
	cout << "正确的识别个数 count = " << count << endl;
	cout << "错误率为..." << (totalCount - count + 0.0) / 5200 * 100.0 << "%....\n";
	int a;
	cin >> a;
	
	

}




int get_data(Mat& trainingImages, vector<int>& trainingLabels,int buf)
{

	string imgs_path, str;
	ostringstream   ostr;
	ostr << train_path <<buf;
	str = ostr.str();
	imgs_path += str;

	vector<String> imgs_file;
	glob(imgs_path, imgs_file);

	if (imgs_file.size() == 0) {
		cout << "No Images" << std::endl;
		return 0;
	}

	for (int i = 0; i < imgs_file.size(); i++)
	{
		Mat  SrcImage = imread(imgs_file[i]);
		Mat feature;
		exactfeature(SrcImage, feature);
		feature = feature.reshape(1, 1);
		trainingImages.push_back(feature);
		trainingLabels.push_back(buf);
	}
	return 1;
}

int get_tdata(Mat& trainingImages, vector<int>& trainingLabels, int buf)
{

	string imgs_path, str;
	ostringstream   ostr;
	ostr << test_path << buf;
	str = ostr.str();
	imgs_path += str;

	vector<String> imgs_file;
	glob(imgs_path, imgs_file);

	if (imgs_file.size() == 0) {
		cout << "No Images" << std::endl;
		return 0;
	}

	for (int i = 0; i < imgs_file.size(); i++)
	{
		Mat  SrcImage = imread(imgs_file[i]);
		Mat feature;
		exactfeature(SrcImage, feature);
		feature = feature.reshape(1, 1);
		trainingImages.push_back(feature);
		trainingLabels.push_back(buf);
	}
	return 1;
}

void exactfeature(Mat &src,Mat &feature)
{
	Mat dst;
	cvtColor(src, dst, COLOR_BGR2GRAY);
	//GaussianBlur(dst, dst, Size(3, 3), 0, 0);
	threshold(dst, dst, 0, 1, THRESH_BINARY | THRESH_OTSU);
	//水平投影特征
	Mat horizonf=Mat::zeros(1, dst.rows,CV_8UC1);
	for (int i = 0; i < dst.rows; i++)
	{
		for (int j = 0; j < dst.cols; j++)
		{
		horizonf.at<uchar>(0,i) = horizonf.at<uchar>(0, i) + dst.at<uchar>(i, j);
		}
	}
	//垂直投影特征
	Mat verticalf = Mat::zeros(1, dst.cols, CV_8UC1);
	for (int j = 0; j < dst.cols; j++)
	{
		for (int i = 0; i < dst.rows; i++)
		{
			verticalf.at<uchar>(0, j) = verticalf.at<uchar>(0, j) + dst.at<uchar>(i, j);
		}
	}
	//将垂直和水平投影相连
	hconcat(horizonf, verticalf, feature);
}