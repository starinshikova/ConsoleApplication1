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

using namespace std;

IplImage* frame = 0;
ofstream f;

double fill(IplImage* src, CvPoint seed, CvScalar color = CV_RGB(255, 0, 0))
{
	assert(src);

	CvConnectedComp comp;

	cvFloodFill(src, seed, color,
		cvScalarAll(20), // минимальная разность
		cvScalarAll(30), // максимальная разность
		&comp,
		CV_FLOODFILL_FIXED_RANGE + 8,
		0);

	// покажем площадь заливки
	printf("[filled area] %.2f\n", comp.area);

	return comp.area;

}

int main(int argc, char* argv[])
{
	// имя файла задаётся первым параметром
	char* filename = "video.avi";

	printf("[i] file: %s\n", filename);

	// окно для отображения картинки
	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);

	// получаем информацию о видео-файле
	CvCapture* capture = cvCreateFileCapture(filename);

	f.open("1.txt");

	while (1) {
		// получаем следующий кадр
		frame = cvQueryFrame(capture);
		if (!frame) {
			break;
		}

		// здесь можно вставить
		// процедуру обработки
		cvSetImageROI(frame, cvRect(210, 479, 210, 120));
		cvAddS(frame, cvScalar(200), frame);
		cvResetImageROI(frame);


		f << fill(frame, cvPoint(320, 479), CV_RGB(250, 0, 0))   << sizeof(double) << endl;
		//fwrite(&fill, sizeof(fill), 1, f); 


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
