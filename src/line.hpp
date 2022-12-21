#pragma once
#include <opencv2/opencv.hpp>

struct Line {
    Line(const int height);

    void Line::detect(const cv::Mat& src, const bool is_left);

    void Line::update(const cv::Mat& src);

    void draw(
        const cv::Mat& frame, 
        const cv::Scalar& color = cv::Scalar(255, 0, 0), 
        const int line_width = 50, 
        const bool average = false
    ) const;

    std::vector<cv::Point> get_points();

    cv::Mat all_x, all_y;
    bool detected = false;
};

std::vector<cv::Point> zip(
    const std::vector<int>& xs,
    const std::vector<int>& ys
);

void detect_lanes(
    const cv::Mat src,
    cv::mat dst,
    Line& line_lt,
    Line& line_rt,
    const bool verbose=false
);
