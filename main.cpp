#include <string>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/mat.hpp>

#include "modules/haru_yaml.h"
#include "modules/haru_httpserver.h"

using namespace haru;

int greyscale(cv::Mat& src, cv::Mat& dst) {

    if (src.empty()) {
        return -1;
    }

    dst.create(src.rows, src.cols, src.type());

    for (int i = 0; i < src.rows; i++) {
        cv::Vec3b* srcPtr = src.ptr<cv::Vec3b>(i);
        cv::Vec3b* dstPtr = dst.ptr<cv::Vec3b>(i);

        for (int j = 0; j < src.cols; j++) {
            uchar blue = srcPtr[j][0];
            uchar green = srcPtr[j][1];
            uchar red = srcPtr[j][2];

            // uchar grey = (uchar)(0.2 * blue + 0.187 * green + 0.89 * red);
            uchar grey = (uchar)(0.114 * blue + 0.587 * green + 0.299 * red);

            dstPtr[j][0] = grey;
            dstPtr[j][1] = grey;
            dstPtr[j][2] = grey;
        }
    }
    return(0);
}
int test() {
    // 1. Load the original 3-channel BGR image
    cv::Mat src = cv::imread("/Users/developer/T9/travels/processed/s25-queenstown-new-zealand/jpeg/20260214_145449.jpeg");
    if (src.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }
    cv::Mat dst;
    greyscale(src, dst);
    cv::imshow("GreyScaled Photo", dst);
    cv::waitKey(0);

    //
    // cv::Mat smoothed;
    //
    // // 2. Apply Bilateral Filter for Skin Smoothing
    // // d: diameter of pixel neighborhood
    // // sigmaColor: filter sigma in color space (larger = more colors blended)
    // // sigmaSpace: filter sigma in coordinate space
    // bilateralFilter(src, smoothed, 15, 80, 80);
    //
    // // 3. (Optional) Alpha Blending to restore some natural texture
    // // Result = smoothed * 0.5 + original * 0.5
    // cv::Mat beautified;
    // addWeighted(smoothed, 1.0, src, 0.5, 0.5, beautified);
    //
    // // 4. Save and display
    // cv::imwrite("/Users/developer/T9/travels/processed/s25-queenstown-new-zealand/jpeg/output_beautified.jpg", beautified);
    // cv::imshow("Beautified Photo", beautified);
    // cv::waitKey(0);
}

int main1() {
    // 1. Read the input image
    cv::Mat src = cv::imread("/Users/developer/T9/travels/processed/s25-queenstown-new-zealand/jpeg/20260214_145449.jpeg");
    if (src.empty()) {
        std::cout << "Could not open or find the image!" << std::endl;
        return -1;
    }

    // 2. Convert the image to grayscale (edges depend on intensity changes, not color)
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    // 3. Reduce noise using a 3x3 Gaussian Blur to prevent false edges
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(3, 3), 3,0);

    // 4. Apply Canny Edge Detection
    cv::Mat edges;
    double lowThreshold = 25;
    double highThreshold = 75; // Generally recommended to be 2x to 3x the lowThreshold
    int kernelSize = 3;         // Aperture size for the Sobel operator

    cv::Canny(blurred, edges, lowThreshold, highThreshold, kernelSize);
    cv::Mat se1 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat img_dilate;
    dilate(edges, img_dilate, se1);
    // 5. Display the results
    cv::imshow("Original Image", src);
    cv::imshow("Canny Edges", edges);
    cv::imshow("Dilate", img_dilate);

    // Wait for a keystroke in the window
    cv::waitKey(0);
    return 0;
}
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " config_file_path" << std::endl;
        return 1;
    }
    // YamlConfig config{.yaml_path="../config/application.yaml"};
    YamlConfig config{.yaml_path=argv[1]};
    config.load_yaml();
    SimpleLogger &logger = SimpleLogger::getInstance();
    logger.configure("../log/log.txt",LogLevel::INFO);
    logger.debug("debug");
    logger.info("info");
    logger.warning("warning");
    logger.error("error");

    webMain(config);
}