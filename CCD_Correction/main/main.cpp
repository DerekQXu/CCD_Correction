#include <iostream>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\opencv.hpp>
#include <cstdlib>
#include <cmath>
#include <vector>
#define inputExposTime 0.01

using namespace cv;
using namespace std;

const int NO_OF_IMAGES = 6;
const int NO_OF_EXPOS = 8;
const int NO_OF_TAKEN_IMAGES = 9;
float EXPOSURE_TIMES[] = { 0.001, 0.01, 0.1, 1, 5, 10, 50, 100 };

//uchar = 0 is dark
//uchar = 255 is bright
int main() {
	//corrects exposure times
	Mat buffer = imread("rawImage.png", 0);
	double correctedExpos = log(inputExposTime);
	for (int i = 0; i < NO_OF_EXPOS; ++i)
		EXPOSURE_TIMES[i] = log(EXPOSURE_TIMES[i]);

	//reads raw image
	Mat rawImg = imread("rawImage.png", 0);
	if (rawImg.empty())
		cerr << "whaa raw img cannot be loaded!" << endl;

	//finds sizes of images being input
	const int IMAGE_WIDTH = rawImg.rows;
	const int IMAGE_HEIGHT = rawImg.cols;

	double sum;
	cout << "Program start" << endl;
	//create vector for all calibration images
	vector <Mat> inputImages;
	//create a vector for calibration images without noise
	vector <Mat> reducedNoiseImages;
	for (int i = 0; i < NO_OF_TAKEN_IMAGES; ++i)
	{
		//input calibration images into a vector
		for (int j = 0; j < NO_OF_IMAGES; ++j) {
			string name = format("%d.png", i*NO_OF_IMAGES+j);
			Mat img = imread(name, 0); // pgm implies grayscale, maybe even: imread(name,0); to return CV_8U
			if (img.empty())
				cerr << "whaa " << name << " can't be loaded!" << endl;
			inputImages.push_back(img);
		}

		//creates average of all calibration images
		for (int i = 0; i < IMAGE_WIDTH; ++i)
			for (int j = 0; j < IMAGE_HEIGHT; ++j) {
				sum = 0;
				for (int n = 0; n < NO_OF_IMAGES; ++n)
					sum += inputImages[n].at<uchar>(i, j);
				sum /= NO_OF_IMAGES;
				buffer.at<uchar>(i, j) = (int) sum;
			}

		//adds noise-removed image into a vector of noise-removed calibration images
		cout << "Noise removed from image " << i << endl;
		reducedNoiseImages.push_back(inputImages[0]);

		//clears original calibraiton image vector
		inputImages.clear();

		//assures that noise removal was successful
		waitKey();
	}

	//biasImage is an image of zero ~or~ close to zero exposure
	Mat bias = reducedNoiseImages[0];
	cout << "found bias" << endl;
#if 0
	int output = bias.at<uchar>(0, 0);
	cout << output << endl;
#endif


	//finds darkCurrent function
	Mat darkCurrent;
	buffer.convertTo(darkCurrent, CV_32F);

	float buff = 0;
	for (int i = 0; i < IMAGE_WIDTH; ++i)
		for (int j = 0; j < IMAGE_HEIGHT; ++j){
			buff = 0;
			for (int n = 0; n < reducedNoiseImages.size()-1; ++n)
				buff += reducedNoiseImages[n].at<uchar>(i, j) / EXPOSURE_TIMES[n];
			buff /= NO_OF_EXPOS;
			darkCurrent.at<float>(i, j) = buff;
		}
	cout << "found dark current" << endl;
#if 0
	output = darkCurrent.at<float>(0, 0);
	cout << output << endl;
#endif
	//suggestion if fail: convert back into type that can be multiplied


	Mat cCorrected = imread("rawImage.png", 0);
	Mat sensitivity;
	buffer.convertTo(sensitivity, CV_32F);
	for (int i = 0; i < IMAGE_WIDTH; ++i)
		for (int j = 0; j < IMAGE_WIDTH; ++j)
			sensitivity.at<float>(i, j) = 1;
	//corrects white poster board picture
	for (int i = 0; i < IMAGE_WIDTH; ++i)
		for (int j = 0; j < IMAGE_HEIGHT; ++j) {
			buff = reducedNoiseImages[reducedNoiseImages.size() - 1].at<uchar>(i, j)
				- bias.at<uchar>(i, j)
				- (unsigned char)(darkCurrent.at<float>(i, j) * correctedExpos);
			if (buff < 0)
				buff = 0;
			if (buff > 255)
				buff = 255;
			cCorrected.at<uchar>(i, j) = buff;
		}
	cout << "found cCorrected" << endl;

	double max_bright = 254;
	//finds sensitivity
	for (int i = 0; i < IMAGE_WIDTH; ++i)
		for (int j = 0; j < IMAGE_HEIGHT; ++j)
			sensitivity.at<float>(i, j) = max_bright / cCorrected.at<uchar>(i,j);
	cout << "found sensitivity" << endl;

	Mat brokenImg = imread("rawImage.png", 0);
	for (int i = 0; i < IMAGE_WIDTH; ++i)
		for (int j = 0; j < IMAGE_HEIGHT; ++j) {
			buff = sensitivity.at<float>(i, j) *
				(rawImg.at<uchar>(i, j)
					- bias.at<uchar>(i, j)
					- (unsigned char)(darkCurrent.at<float>(i, j) * correctedExpos));
#if 0
			if (buff < 0)
				buff = 0;
			if (buff > 255)
				buff = 255;
#endif
			brokenImg.at<uchar>(i, j) = buff;
		}

	Mat correctImg = imread("rawImage.png", 0);
	for (int i = 0; i < IMAGE_WIDTH; ++i)
		for (int j = 0; j < IMAGE_HEIGHT; ++j) {
			buff = sensitivity.at<float>(i, j) *
				(brokenImg.at<uchar>(i, j)
					+ bias.at<uchar>(i, j)
					+ (unsigned char)(darkCurrent.at<float>(i, j) * correctedExpos));
			if (buff < 0)
				buff = 0;
			if (buff > 255)
				buff = 255;
			correctImg.at<uchar>(i, j) = buff;
		}


	//output raw image
	namedWindow("Picture1", CV_WINDOW_FREERATIO);
	imshow("Picture1", brokenImg);

	namedWindow("Picture2", CV_WINDOW_FREERATIO);
	imshow("Picture2", correctImg);
	waitKey(0);
}