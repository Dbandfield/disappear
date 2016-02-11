#include "textcontrol.h"

TextControl::TextControl()
{
    maxItems = 20;
}

TextControl::~TextControl()
{
    //dtor
}

void TextControl::addItem(std::string _str, cv::Scalar _col)
{
    textItems.push_back(TextItem(_str, _col));
    if(textItems.size() >maxItems) textItems.pop_front();
}

void TextControl::display(cv::Mat &_im)
{
    int xPos = _im.size().width/4;
    int yPos = _im.size().height/4;
    int inc = 32;

    cv::putText(_im, "---------------------------------",
                cv::Point(xPos, yPos), CV_FONT_HERSHEY_PLAIN,
                1, cv::Scalar(255, 255, 255));

    yPos += inc;

    for(std::list<TextItem>::iterator it = textItems.begin(); it != textItems.end(); it ++)
    {
        cv::putText(_im, it->str, cv::Point(xPos, yPos),
                    CV_FONT_HERSHEY_PLAIN, 1, it->col);
        yPos += inc;
    }
    cv::putText(_im, "---------------------------------",
                cv::Point(xPos, yPos), CV_FONT_HERSHEY_PLAIN,
                1, cv::Scalar(255, 255, 255));
}
