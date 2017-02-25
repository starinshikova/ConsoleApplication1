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

IplImage* frame = 0;

void fill(IplImage* src, CvPoint seed, CvScalar color = CV_RGB(255, 0, 0))
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
	//fprintf()
	FILE* f;
	if ((f = fopen("1.txt", "wb+")) == NULL) {
		printf("ne otkrivaetcya");
		exit(1);
	}
	fwrite(&comp.area, sizeof(comp.area), 1, f); 
	fclose(f);
	f = NULL;
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
		fill(frame, cvPoint(320, 479), CV_RGB(250, 0, 0));


		// показываем кадр
		cvShowImage("original", frame);

		char c = cvWaitKey(0);
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
