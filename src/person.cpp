#include "person.h"

Person::Person(cv::Rect _box)
{
    disp = true;
    remove = false;
    red = false;
    foundThisTurn = true;

    displayTimer = 0;
    displayMaxTimer = 4;

    mainBox = _box;

}

Person::~Person()
{
    //dtor
}

bool Person::update(cv::Rect otherBox)
{
    red = !red;
    bool same = false;
    if((mainBox & otherBox).area() > 50)
    {
        mainBox = otherBox;
        same = true;
    }


    displayTimer ++;
    if(displayTimer > displayMaxTimer) disp = false;

    return same;
}

void Person::display(cv::Mat &im)
{
    if(disp)
    {

        cv::line(im, cv::Point(mainBox.x + (mainBox.width/2), 0), cv::Point(mainBox.x + (mainBox.width/2),im.size().height), cvScalar(0, 0, 255));
        cv::line(im, cv::Point(0, mainBox.y + (mainBox.height/2)), cv::Point(im.size().width, mainBox.y + (mainBox.height/2)), cvScalar(0, 0, 255));
        if(red)
        {
            cv::rectangle(im, mainBox, cv::Scalar(0, 0, 255) );
        }
        else
        {
            cv::rectangle(im, mainBox, cv::Scalar(100, 100, 255) );

        }

    }
}
