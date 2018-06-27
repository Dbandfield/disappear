#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <list>
#include <string>

struct TextItem
{
    std::string str;
    cv::Scalar  col;
    TextItem(std::string _str, cv::Scalar _col)
    {
        str = _str;
        col = _col;
    }
};

class TextControl
{
    public:
        TextControl();
        virtual ~TextControl();

        void addItem(std::string _str, cv::Scalar _col);

        void display(cv::Mat &_im);
    protected:
        std::list<TextItem> textItems;
        int maxItems;
    private:
};
