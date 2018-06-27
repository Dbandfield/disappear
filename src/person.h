#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/bgsegm.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>

#include <vector>
#include <iostream>
#include <string>

class Person
{
    public:
        Person(cv::Rect _box);
        virtual ~Person();
        bool update(cv::Rect otherBox);
        void display(cv::Mat &im);

        void reset(){foundThisTurn = false;}
        void setFound(){foundThisTurn = true;}

        bool getRemove(){return remove;}
        bool getFoundThisTurn(){return foundThisTurn;}
    protected:
        cv::Rect mainBox;
        std::vector<cv::Rect> boxes;
        bool remove;
        bool disp;
        bool red;

        bool foundThisTurn;

        int displayTimer;
        int displayMaxTimer;
    private:
};
