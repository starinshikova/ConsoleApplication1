/**
* @file videocapture_starter.cpp
* @brief A starter sample for using OpenCV VideoCapture with capture devices, video files or image sequences
* easy as CV_PI right?
*
*  Created on: Nov 23, 2010
*      Author: Ethan Rublee
*
*  Modified on: April 17, 2013
*      Author: Kevin Hughes
*/

#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <time.h>  
#include <istream>
#include <iostream>
#include <stdio.h>

using namespace cv; 
using namespace std;

IplImage* frame = 0;
IplImage* dst = 0;

double fill(IplImage* src, CvPoint seed, CvScalar color = CV_RGB(255, 0, 0))
{
	assert(src);

	CvConnectedComp comp;

	cvFloodFill(src, seed, color,
		cvScalarAll(30), // минимальная разность
		cvScalarAll(30), // максимальная разность
		&comp,
		CV_FLOODFILL_FIXED_RANGE + 8,
		0);

	// покажем площадь заливки
  	//printf("[filled area] %.2f\n", comp.area);

   	return comp.area;

}

inline bool isBlack(int x, uint8_t *row)
{
	int r = row[3 * x + 2];
	int g = row[3 * x + 1];
	int b = row[3 * x + 0];

	return ((b*0.0722 + g*0.7152 + r*0.2126) < 60);
}

int center_line(IplImage* src, int x, int y, int width) 
{
	assert(src);
	int blDotX3 = 0, blDotX4 = 0, XROI = 0, Xco2 = 0, count = 0;
	uint8_t *row2;
	Mat orig;
	//dst = cvCloneImage(frame);

	//set the ROI in copy image (like temp image for finding center of black line)
	//int yt = 639 - heightROI;
	cvSetImageROI(src, cvRect(x, y, width, 2));

	//convert image to matrix
	orig = cvarrToMat(src);

	//save the row from matrix with x=44
	row2 = (uint8_t*)orig.ptr<uint8_t>(1);

	//set the starting point of searching black point in bottom line of ROI
	blDotX3 = 0;
	blDotX4 = width-1;

	//finding black point from the left (from 0 to ..+)
	while (!isBlack(blDotX3, row2))
	{
		blDotX3++;
	}

	//finding black point from the right (from 200 to ...-)
	while (!isBlack(blDotX4, row2))
	{
		blDotX4--;
	}

	// center of black line
	return x+(blDotX3 + blDotX4) / 2;

}


int main(int argc, char* argv[])
{
	// имя файла задаётся первым параметром
	char* filename = "p12.avi";

	printf("[i] file: %s\n", filename);

	// окно для отображения картинки
	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);

	// получаем информацию о видео-файле
	CvCapture* capture = cvCreateFileCapture(filename);

	//f.open("1.txt");


	int x = argc >= 3 ? atoi(argv[2]) : 170;
	int y = argc >= 4 ? atoi(argv[3]) : 420;
	int widthROI = argc >= 5 ? atoi(argv[4]) : 260;
	int heightROI = argc >= 6 ? atoi(argv[5]) : 40;
	//int add = argc >= 7 ? atoi(argv[6]) : 400;
	double area1=0, area2=0, dif=0, proc=0, area2l=0, area2r=0;
	int XROI = 0, Xco2 = 0, count = 0;
	clock_t start, finish;
	double  duration;

	while (1) {
		// получаем следующий кадр
		frame = cvQueryFrame(capture);
		if (!frame) {
			break;
		}

		// здесь можно вставить
		// процедуру обработки

		start = clock();

		XROI = center_line(frame, x, y+heightROI, widthROI) - widthROI/2;
		//XROI = x + Xco2 - widthROI / 2;

		if (XROI > 639-widthROI) {
			XROI = 639 - widthROI;
		}

		//ustanovim ROI
		cvSetImageROI(frame, cvRect(XROI, y, widthROI, heightROI));
		//color ROI in blue
		//cvAddS(frame, cvScalar(200), frame);

		//calculate and fill with red area in black line from center of black line
		CvConnectedComp comp;

		cvFloodFill(frame, cvPoint(10, heightROI-1), CV_RGB(255, 0, 0),
			cvScalarAll(50), // минимальная разность
			cvScalarAll(50), // максимальная разность
			&comp,
			CV_FLOODFILL_FIXED_RANGE + 8,
			0);
		area2l = comp.area;

		cvFloodFill(frame, cvPoint(widthROI-10, heightROI - 1), CV_RGB(0, 255, 0),
			cvScalarAll(50), // минимальная разность
			cvScalarAll(50), // максимальная разность
			&comp,
			CV_FLOODFILL_FIXED_RANGE + 8,
			0);
		//reset ROI
		cvResetImageROI(frame);
		area2r = comp.area;

		area2 = area2l + area2r;

		//if we have first frame of video
		if (count == 0) {
			area1 = area2; count++;
		}

		//calculating diffarance
		proc = fabs(area2 - area1) / area2 * 100;

		area1 = area2;
		x = XROI;

		finish = clock();
		duration = (double)(finish - start) / CLOCKS_PER_SEC; 
		printf("%2.3f seconds\n", duration);

		// показываем кадр
		cvShowImage("original", frame);

		if (proc > 3) {
			printf("STOP");
			char c = cvWaitKey(0);

		}

		char c = cvWaitKey(33);
		if (c == 27) { // если нажата ESC - выходим
			break;
		}
	}
	
	// освобождаем ресурсы
	cvReleaseCapture(&capture);
	
	// удаляем окно
	cvDestroyWindow("original");
	return 0; 
}
