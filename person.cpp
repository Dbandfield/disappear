#include "person.h"

Person::Person(cv::Rect _box)
{
    disp = true;
    remove = false;
    red = false;

    displayTimer = 0;
    displayMaxTimer = 4;

    mainBox = _box
    calcBoxes();
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
        calcBoxes();
        same = true;
    }


    displayTimer ++;
    if(displayTimer > displayTimrerMax) disp = false;

    return same;
}

void Person::display(cv::Mat &im)
{
    bool tmpRed = red;
    if(disp)
    {
        for(int i = 0; i < boxes.size(); i ++)
        {

            if(tmpRed)
            {
                cv::rectangle(im, boxes[i], cv::Scalar(0, 0, 255) );
            }
            else
            {
                cv::rectangle(im, boxes[i], cv::Scalar(50, 50, 255) );

            }
            tmpRed = !tmpRed;
        }
    }
}

void Person::calcBoxes()
{
    boxes.clear();
    int inc = 3;
    cv::Rect tmpBox = cv::Rect(mainBox.x, mainBox.y, mainBox.width, mainBox.height);
    while(tmpBox. width > 0 && tmpBox.height > 0)
    {
        tmpBox.x += inc;
        tmpBox.y += inc;
        tmpBox.width -= (inc * 2);
        tmpBox.height -= (inc * 2);
        boxes.push_back(tmpBox);
    }
}
