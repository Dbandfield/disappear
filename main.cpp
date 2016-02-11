#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include "person.h"
#include "textcontrol.h"

typedef std::vector<std::vector<cv::Point> > Contours;
typedef std::vector<std::vector<cv::Point> >::iterator  ContoursItr;
typedef std::vector<cv::Rect> Faces;
typedef std::vector<cv::Rect>::iterator  FacesItr;
typedef std::vector<Person> People;
typedef std::vector<Person>::iterator PeopleItr;

cv::RNG ran;
int ranNum;

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
cv::Mat combined;

// Contours
Contours contours;
Faces faces;

// Face Detection
cv::CascadeClassifier cascade;

// Background Subtraction
cv::Ptr<cv::BackgroundSubtractor> sub;

// My objects
People ppl;
TextControl textControl;

int frameCount;

// Video
cv::VideoCapture cap;

// Dimensions
cv::Size monitorSz;
cv::Size camSize;
cv::Size textSize;
cv::Size combinedSz;

// Window
char windowName[] = "main";
bool testWnd = false;

std::vector<std::string> waitingStrings;
std::vector<std::string> foundStrings;
std::vector<std::string> removingStrings;


int main()
{

    frameCount = 0;
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
	for(int i =0; i < 50; i ++)
	{
		cap >> background;
	}
    monitorSz = cv::Size(1920, 1080);
    combinedSz = cv::Size(monitorSz.height, monitorSz.width);
	float a = (float)monitorSz.height/(float)background.size().width;
	float b = (float)background.size().height * a;
	camSize = cv::Size(monitorSz.height, b);
	textSize = cv::Size(monitorSz.height, monitorSz.width - camSize.height);
	combined = cv::Mat(combinedSz, background.type());

    bool cont = true;

    // open files
    std::ifstream file;
    std::string line;
    // waiting
	file.open("/home/odroid/Documents/disappear/data/waiting.txt");
    if(file)
    {
        while(std::getline(file, line))
        {
            std::cout << " Reading: " << line << std::endl;
            waitingStrings.push_back(line);
        }
        file.close();
	}
	else
	{
        std::cout <<"error opening file" << std::endl;
        return 1;
	}

	// found
    file.open("/home/odroid/Documents/disappear/data/found.txt");
    if(file)
    {
        while(std::getline(file, line))
        {
                    std::cout << " Reading: " << line << std::endl;

            foundStrings.push_back(line);
        }
        file.close();
	}
	else
	{
        std::cout <<"error opening file" << std::endl;
        return 1;
	}

    // removing
    file.open("/home/odroid/Documents/disappear/data/removing.txt");
    if(file)
    {
        while(std::getline(file, line))
        {
                    std::cout << " Reading: " << line << std::endl;

            removingStrings.push_back(line);
        }
        file.close();
	}
	else
	{
        std::cout <<"error opening file" << std::endl;
        return 1;
	}

	while(cont)
	{
        if(frameCount %6 == 0)
        {

            ran = cv::RNG(cv::getTickCount());
            ranNum = ran.uniform(0, waitingStrings.size());
            std::cout << ranNum << std::endl;
            textControl.addItem(waitingStrings[ranNum], cv::Scalar(255, 255, 255));
        }

		// get frame
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

            ran = cv::RNG(cv::getTickCount());
            ranNum = ran.uniform(0, foundStrings.size());
            std::cout << ranNum << std::endl;
            textControl.addItem(foundStrings[ranNum], cv::Scalar(0, 0, 255));


            ran = cv::RNG(cv::getTickCount());
            ranNum = ran.uniform(0, removingStrings.size());
            std::cout << ranNum << std::endl;
            textControl.addItem(removingStrings[ranNum], cv::Scalar(255, 128, 0));




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

		// text
		textIm = cv::Mat::zeros(textSize, frame.type());
		textControl.display(textIm);
				cv::resize(frame, frame, camSize);

		std::cout << "Text: " << textIm.size() << " " << textIm.type() << std::endl;
		std::cout << "Frame: " <<frame.size() << " " << frame.type() << std::endl;
		std::cout << "Combined: " << combined.size() << " " << combined.type() << std::endl;
				cv::vconcat(frame, textIm, combined);
		cv::rotate(combined, combined, cv::ROTATE_90_CLOCKWISE);

		cv::imshow(windowName, combined);
		char key = cv::waitKey(10);
		switch (key)
		{
            case 27://esc
                cont = false;
                break;
		}

		frameCount ++;
    }
}




