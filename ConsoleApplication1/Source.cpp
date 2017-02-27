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

#include <istream>
#include <iostream>
#include <stdio.h>

using namespace cv; 
using namespace std;

IplImage* frame = 0;
ofstream f;
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

int main(int argc, char* argv[])
{
	// имя файла задаётся первым параметром
	char* filename = "n14.avi";

	printf("[i] file: %s\n", filename);

	// окно для отображения картинки
	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);

	// получаем информацию о видео-файле
	CvCapture* capture = cvCreateFileCapture(filename);

	f.open("1.txt");


	int x = argc >= 3 ? atoi(argv[2]) : 200;
	int y = argc >= 4 ? atoi(argv[3]) : 434;
	int width = argc >= 5 ? atoi(argv[4]) : 300;
	int height = argc >= 6 ? atoi(argv[5]) : 45;
	int add = argc >= 7 ? atoi(argv[6]) : 400;
	double area1=0, area2=0, dif=0, proc=0;
	Mat orig;
	int blDotX3 = 0, blDotX4 = 0, Xco = 0, Xco2 = 0, count = 0;
	uint8_t *row2;

	while (1) {
		// получаем следующий кадр
		frame = cvQueryFrame(capture);
		if (!frame) {
			break;
		}

		// здесь можно вставить
		// процедуру обработки
		dst = cvCloneImage(frame);

		//set the ROI in copy image (like temp image for finding center of black line)
		cvSetImageROI(dst, cvRect(x, y, width, height));

		//convert image to matrix
		orig = cvarrToMat(dst);

		//save the row from matrix with x=44
		row2 = (uint8_t*)orig.ptr<uint8_t>(44);
		
		//set the starting point of searching black point in bottom line of ROI
		blDotX3 = 0;
		blDotX4 = 200;

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
		Xco2 = (blDotX3 + blDotX4) / 2;

		//printf("[x center] %.2u\n", Xco2);
		if (count == 0) {
			x += (int)Xco2 - 57; count++;
		}

		//set image ROI
		cvSetImageROI(frame, cvRect(x + Xco2-width/2, 435, 200, 45));

		//color ROI in blue
     	cvAddS(frame, cvScalar(200), frame);

		//reset ROI
		cvResetImageROI(frame);

		//calculate and fill with red area in black line from center of black line
		area2 = fill(frame, cvPoint(x + Xco2, 479), CV_RGB(250, 0, 0));

		//if we have first frame of video
		if (count == 1) {
			area1=area2; count++;
		}

		//calculating diffarance
		dif = fabs(area2 - area1);
		proc = dif / area2 * 100;

		//write in file f
		f << dif  << sizeof(double) << endl;
		f << area1 << sizeof(double) << endl;
		f << area2 << sizeof(double) << endl;
		f << proc << sizeof(double) << endl;

		printf("[area1] %.2f\n", area1);
		printf("[area2] %.2f\n", area2);
		printf("[differance area] %.2f\n", dif);
		printf("[differance area in proc] %.2f\n", proc);

		area1 = area2;
		x = x + Xco2 - width / 2;

		// показываем кадр
		cvShowImage("original", frame);

		char c = cvWaitKey(0);
		if (c == 27) { // если нажата ESC - выходим
			break;
		}
	}
	f.close();

	// освобождаем ресурсы
	cvReleaseCapture(&capture);
	
	// удаляем окно
	cvDestroyWindow("original");
	return 0; 
}
