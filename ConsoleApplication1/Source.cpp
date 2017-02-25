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
		cvScalarAll(20), // ����������� ��������
		cvScalarAll(30), // ������������ ��������
		&comp,
		CV_FLOODFILL_FIXED_RANGE + 8,
		0);

	// ������� ������� �������
	printf("[filled area] %.2f\n", comp.area);

	return comp.area;

}

int main(int argc, char* argv[])
{
	// ��� ����� ������� ������ ����������
	char* filename = "video.avi";

	printf("[i] file: %s\n", filename);

	// ���� ��� ����������� ��������
	cvNamedWindow("original", CV_WINDOW_AUTOSIZE);

	// �������� ���������� � �����-�����
	CvCapture* capture = cvCreateFileCapture(filename);

	f.open("1.txt");

	while (1) {
		// �������� ��������� ����
		frame = cvQueryFrame(capture);
		if (!frame) {
			break;
		}

		// ����� ����� ��������
		// ��������� ���������
		cvSetImageROI(frame, cvRect(210, 479, 210, 120));
		cvAddS(frame, cvScalar(200), frame);
		cvResetImageROI(frame);


		f << fill(frame, cvPoint(320, 479), CV_RGB(250, 0, 0))   << sizeof(double) << endl;
		//fwrite(&fill, sizeof(fill), 1, f); 


		// ���������� ����
		cvShowImage("original", frame);

		char c = cvWaitKey(0);
		if (c == 27) { // ���� ������ ESC - �������
			break;
		}
	}
	f.close();

	// ����������� �������
	cvReleaseCapture(&capture);
	
	// ������� ����
	cvDestroyWindow("original");
	return 0; 
}
