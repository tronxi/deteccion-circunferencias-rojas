// ejemplo1.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include<cstdio>
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<iostream>
#include<string>

using namespace std;
using namespace cv;

int main()
{	
	VideoCapture cap(0);

	Scalar min1(0, 100, 100);
	Scalar max1(10, 255, 255);

	Scalar min2(160, 100, 100);
	Scalar max2(179, 255, 255);


	vector<vector<Point> > contornos;
	vector<Vec4i> jerarquia;

	if (!cap.isOpened())
	{
		cout << "Error al abrir la camara ";
	}
	else
	{
		while (true)
		{
			int64 start = cv::getTickCount();
			Mat frame, gray, hsv, mask1, mask2, maskT;
			
			int x = 25, y = 25;

			cap.read(frame);

			cv::GaussianBlur(frame, frame, cv::Size(9, 9), 2, 2);

			cv::cvtColor(frame, hsv, CV_BGR2HSV);


			cv::inRange(hsv, min1, max1, mask1);
			cv::inRange(hsv, min2, max2, mask2);
			maskT = mask1 | mask2;

			cv::findContours(maskT, contornos, jerarquia, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
			for (size_t idx = 0; idx < contornos.size(); idx++) 
			{
				vector<Point> contorno = contornos[idx];
				double area = cv::contourArea(contorno);
				if (area >= 3000)
				{
					cv::drawContours(frame, contornos, idx, Scalar(255, 255, 0));
				}
			}

			cv::bitwise_and(frame, frame, gray, maskT);
			cv::cvtColor(gray, gray, CV_BGR2GRAY);
			cv::GaussianBlur(gray, gray, cv::Size(9, 9), 2, 2);

			vector<Vec3f> circulos;
			cv::HoughCircles(gray, circulos, CV_HOUGH_GRADIENT, 1, gray.rows / 16, 100, 30, 0, 100000);
			for (size_t i = 0; i < circulos.size(); i++)
			{
				y += 50;
				Point center(cvRound(circulos[i][0]), cvRound(circulos[i][1]));
				int radius = cvRound(circulos[i][2]);

				circle(frame, center, 3, Scalar(0, 255, 0), 3);
				circle(frame, center, radius, Scalar(255, 0, 0), 3);

				string text = std::to_string(cvRound(circulos[i][0])) + " " + std::to_string(cvRound(circulos[i][1]));
				putText(frame, text, Point(x, y), CV_FONT_NORMAL, 1, CV_RGB(0, 0, 255), 1);
			}

			double fps = cv::getTickFrequency() / (cv::getTickCount() - start);
			putText(frame, std::to_string(fps), Point(25, 25), CV_FONT_NORMAL, 1, CV_RGB(0, 0, 255), 1);

			//imshow("gray", gray);
			imshow("frame", frame);
			imshow("mask", maskT);

			if (waitKey(30) >= 0)
			{
				break;
			}
		}
	}
    return 0;
}