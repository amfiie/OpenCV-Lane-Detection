#include "line.hpp"

Line::Line(const int height) {
    for (int i = 0; i < height; i++) {
        all_y.push_back(i);
    }
}

void Line::detect(const cv::Mat& src, const bool is_left) {
    const int width = src.size().width;
    const int height = src.size().height;

    cv::Mat histogram;
    int offset = 0;
    if (is_left) {
        histogram = src(cv::Rect(0, 3 * height / 4, width / 2, height / 4));
    } else {
        histogram = src(cv::Rect(width / 2, 3 * height / 4, width / 2, height / 4));
        offset = width / 2;
    }

    std::vector<cv::Point> nonzero;
    cv::findNonZero(histogram, nonzero);
    
    if (nonzero.empty()) {
        detected = false;
        return;
    }
    
    int base_x = offset + cv::mean(nonzero)[0];
    const int margin = 60;
    int found_pixels = 0;

    for (int h = 0; h < src.size().height; h++) {
        cv::Mat row;
        for (int w = -margin; w <=margin; w++) {
            int x = base_x + w;
            if (x >= 0 && x < src.size().width) {
                cv::Vec3b clr = src.at<cv::Vec3b>(h, x);
                if (clr[0] != 0 || clr[1] != 0 || clr[2] != 0) {
                    row.push_back(x);
                    found_pixels++;
                }
            }
        }
        if (!row.empty()) {
            base_x = cv::mean(row)[0];
            all_x.at<int>(h) = base_x;
        }
    }
    detected = found_pixels > 50;
}

void Line::update(const cv::Mat& src) {
    const int margin = 60;
    int found_pixels = 0;
    for (int h = 0; h < src.size().height; h++) {
        int base_x = all_x.at<int>(h);
        cv::Mat row;
        for (int w = -margin; w <= margin; w++) {
            int x = base_x + w;
            if (x >= 0 && x < src.size().width) {
                cv::Vec3b clr = src.at<cv::Vec3b>(h, x);
                if (clr[0] != 0 || clr[1] != 0 || clr[2] != 0) {
                    row.push_back(x);
                    found_pixels++;
                }
            }     
        }
        if (!row.empty()) {
            all_x.at<int>(h) = cv::mean(row)[0];
        }
    }
    detected = found_pixels > 50;
}

void Line::draw (
    const cv::Mat& frame, 
    const cv::Scalar& color, 
    const int line_width, 
    const bool average
) const {
    cv::Mat line_left_side = all_x - line_width / 2;
    cv::Mat line_right_side = all_x + line_width / 2;
    std::vector<cv::Point> pts = zip(line_left_side, all_y);
    std::vector<cv::Point> pts_right = zip(line_right_side, all_y);
    std::reverse(pts_right.begin(), pts_right.end());
    pts.insert(pts.end(), pts_right.begin(), pts_right.end());
    cv::fillPoly(frame, pts, color);
}

std::vector<cv::Point> Line::get_points() {
    return zip(all_x, all_y);
}

std::vector<cv::Point> zip(
    const std::vector<int>& xs,
    const std::vector<int>& ys
) {
    std::vector<cv::Point> pts;
    pts.reserve(xs.size());
    for (int i = 0; i < xs.size(); i++) {
        pts.push_back(cv::Point(xs[i], ys[i]));
    }
    return pts;
}