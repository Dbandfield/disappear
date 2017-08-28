#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include <vector>
#include <iostream>
#include <string>

#include "person.h"
#include "textcontrol.h"

typedef std::vector<std::vector<cv::Point> > Contours;
typedef std::vector<std::vector<cv::Point> >::iterator  ContoursItr;
typedef std::vector<cv::Rect> Faces;
typedef std::vector<cv::Rect>::iterator  FacesItr;
typedef std::vector<Person> People;
typedef std::vector<Person>::iterator PeopleItr;

// Images
cv::Mat frame;
cv::Mat frameGrey;
cv::Mat background;
cv::Mat processed;
cv::Mat contourIm;
cv::Mat textIm;
cv::Mat erodeElem;
cv::Mat dilateElem;
cv::Mat extBackground;
cv::Mat extFrame;

// Contours
Contours contours;
Faces faces;

// Face Detection
cv::CascadeClassifier cascade;

// Background Subtraction
cv::Ptr<cv::BackgroundSubtractor> sub;

// My objects
People ppl;
//TextControl textControl;

// Video
cv::VideoCapture cap;

// Dimensions
cv::Size monitorSz;
cv::Size camSize;
cv::Size textSize;

// Window
char windowName[] = "main";
bool testWnd = false;


int main()
{
	//load cascade
	cascade.load("/home/odroid/Documents/disappear/data/cascade.xml");


	// Background Sub
	sub = cv::createBackgroundSubtractorMOG2(500, 16, false);
    erodeElem = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));
	dilateElem = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));

	// Window
	cv::namedWindow(windowName, CV_WINDOW_NORMAL);
	if(!testWnd) cv::setWindowProperty(windowName, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

	// video
	cap.open(0);

	// get background
	for(int i =0; i < 100; i ++)
	{
		cap >> background;
	}
    monitorSz = cv::Size(1920, 1080);
	float a = background.size().width / monitorSz.height;
	float b = background.size().height * a;
	camSize = cv::Size(monitorSz.height, b);
	textSize = cv::Size(monitorSz.height, monitorSz.width - camSize.width);

    bool cont = true;
	while(cont)
	{
		// get frsme
		cap >> frame;

		cv::cvtColor(frame, frameGrey, CV_RGB2GRAY);

		// get moving things
		sub->apply(frame, processed, 0.001);
        if(testWnd) cv::imshow("preproc", processed);

		// tidy up
		medianBlur(processed, processed, 9);
		cv::erode(processed, processed, erodeElem);
		cv::dilate(processed, processed, dilateElem);
		cv::threshold(processed, processed, 100, 255, CV_THRESH_BINARY);
		processed.convertTo(processed, CV_8UC1);

        if(testWnd) cv::imshow("proc", processed);
		// Get contours
		cv::findContours(processed, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1);
		contourIm = cv::Mat::zeros(frame.size(), CV_8UC1);


		// Find faces
		cascade.detectMultiScale(frameGrey, faces, 1.1);

		// reset pp
		for(PeopleItr it = ppl.begin(); it != ppl.end(); it ++)
		{
            it->reset();
		}


        for(FacesItr it = faces.begin(); it != faces.end(); it ++)
        {
            bool found = false;
            for(PeopleItr it2 = ppl.begin(); it2 != ppl.end(); it2 ++)
            {
                found = it2->update(*it);
                if(found)
                {
                    it2->setFound();
                    break;
                }
            }

            if(!found)
            {
                ppl.push_back(Person(*it));
            }
        }

		for(int i = ppl.size() -1; i >= 0; i --)
		{
            if(!ppl[i].getFoundThisTurn())
            {
                ppl.erase(ppl.begin() + i);
            }
		}


		for (int i = contours.size() - 1; i >= 0; i--)
		{
			bool remove = true;
			for (FacesItr it = faces.begin(); it != faces.end(); it++)
			{
				cv::Point pt(it->x + (it->width / 2), it->y + (it->height / 2)); // rect centre

				if (cv::pointPolygonTest(contours[i], pt, false) >= 0)
				{
					remove = false;
					break;
				}
			}

			if(remove) contours.erase(contours.begin() + i);
		}

		if (contours.size() > 0)
		{
			cv::drawContours(contourIm, contours, -1, cv::Scalar(255, 255, 255), CV_FILLED);
		}
        if(testWnd) cv::imshow("contour im", contourIm);
		//cv::threshold(processed, processed, 100, 255, CV_THRESH_BINARY);

		// extract background
		extBackground = cv::Mat::zeros(frame.size(), frame.type());
		background.copyTo(extBackground, contourIm);

		cv::bitwise_not(contourIm, contourIm);
		extFrame = cv::Mat::zeros(frame.size(), frame.type());
		frame.copyTo(extFrame, contourIm);

		cv::bitwise_or(extFrame, extBackground, frame);

		for(PeopleItr it = ppl.begin(); it != ppl.end(); it ++)
		{
            it->display(frame);
		}


		cv::imshow(windowName, frame);
		char key = cv::waitKey(10);
		switch (key)
		{
		case 27://esc
			cont = false;
			break;
		}


    }

}




