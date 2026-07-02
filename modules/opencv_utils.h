//
// Created by developer on 2026-05-19.
//
#include <filesystem>
#ifndef HARU_OPENCV_WEB_OPENCV_UTILS_H
#define HARU_OPENCV_WEB_OPENCV_UTILS_H
#include <opencv2/opencv.hpp>
namespace haru {
    struct Matrix33
    {
        float vertical_fk[3][3];
    };
    std::string getVideo();
    std::vector<std::string> split(const std::string &s, char delim);
    std::vector<float> convert(const std::vector<std::string> &input);
    Matrix33 convertMatrix(const std::string r[], int size);

    void convertImage(cv::Mat &frame, std::vector<uchar> &buffer);
    void save_image(std::string filename, cv::Mat &frame);

    cv::Mat *rotation(cv::Mat &frame, double angle);
    cv::Mat *image_filter(cv::Mat &frame, float vertical_fk[3][3]);
    cv::Mat *grey(cv::Mat &frame);
    cv::Mat *blurImage(cv::Mat &frame);
    cv::Mat *gaussinblurImage(cv::Mat &frame);
    cv::Mat *styleImage(cv::Mat &frame);
    void sketch(cv::Mat &frame, cv::Mat &greyFrame, cv::Mat &colorFrame, float sigma_s, float sigma_r, float shade_factor);
    cv::Mat *sketchImage(cv::Mat &frame);
    cv::Mat *sketchColorImage(cv::Mat &frame);
    cv::Mat *detailEnhanceImage(cv::Mat &frame, float sigma_s, float sigma_r);
    cv::Mat *medianblurImage(cv::Mat &frame);
    cv::Mat *bilateralImage(cv::Mat &frame);
    cv::Mat *flipImage(cv::Mat &frame, int direction);
    cv::Mat *normalizeImage(cv::Mat &frame);
    void contrastImage(cv::Mat &frame, std::vector<uchar> &buffer, double alpha = 2.2, int beta = 50);
    cv::Mat *edgePreservingImage(cv::Mat &frame,double sigma_s,double sigma_r);
    cv::Mat *readVideo(int index, std::string &filename, std::vector<uchar> &buffer);
    void read_frame(int index, std::string &filename, cv::Mat &frame);
    int save_video(std::string &in_file, std::string &out_file);
    cv::Mat *shift_frame_vertically(cv::Mat &frame,double shift);
    cv::Mat *shift_frame_horizentally(cv::Mat &frame,double shift);
    cv::Mat *contrast(cv::Mat &frame, double alpha, int beta);
    void skin_smooth(cv::Mat &src,cv::Mat &smoothedImage);
    void load_image();
    int play_image_with_background(std::string &input_image_path,int bgWidth,int bgHeight);
    cv::Mat *resize_image(cv::Mat src,double scale,int bgWidth,int bgHeight);
    void load_video(std::string filename,std::filesystem::path parent_path);
    void combine_image(std::string &filename1,std::string &filename2);
    void combine_overlay(std::string &filename1,std::string &filename2);
}
#endif //HARU_OPENCV_WEB_OPENCV_UTILS_H
