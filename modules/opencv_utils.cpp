#include <iostream> // Header for input and output
#include <filesystem>
#include <opencv2/imgproc/types_c.h>
#include "file_utils.h"
#include "opencv_utils.h"
#include "haru_random.h"
#include <cmath>

int g_slider_position = 0;
int g_run = 1, g_dontset = 0; //start out in single step mode
cv::VideoCapture g_cap;
namespace haru {
    /*To convert mp4 to webm(50% compression rate)
     *ffmpeg -i x002.mp4 -c:v libvpx-vp9 -crf 30 -b:v 0 -c:a libopus output.webm
     */
    const cv::Scalar BACKGROUND_COLOR = cv::Scalar(135, 206, 235);
    HaruRandom haru_random(1,5);
    std::string getVideo()
    {
        return "/Users/developer/T9/document/seijin/Unfaithful-things.mp4";
        // return "/Users/developer/T9/document/movie/猩疯血雨.mp4";
    }

    std::vector<std::string> split(const std::string &s, char delim)
    {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim))
        {
            result.push_back(item);
        }
        return result;
    }

    std::vector<float> convert(const std::vector<std::string> &input)
    {
        std::vector<float> result;
        for (const auto &val : input)
        {
            result.push_back(std::stof(val));
        }
        return result;
    }

    Matrix33 convertMatrix(const std::string r[], int size)
    {
        Matrix33 maxtrix;
        for (int i = 0; i < size; i++)
        {
            std::vector<std::string> rv = split(r[i], ',');
            std::vector<float> rf = convert(rv);
            for (int j = 0; j < rf.size(); j++)
            {
                maxtrix.vertical_fk[i][j] = rf[j];
            }
        }
        return maxtrix;
    }

    void convertImage(cv::Mat &frame, std::vector<uchar> &buffer)
    {
        std::cout << "saveImage..." << std::endl;
        std::vector<int> param = std::vector<int>(2);
        param[0] = cv::IMWRITE_JPEG_QUALITY;
        param[1] = 95; // Set quality to 95%

        // Encode Mat to JPG buffer
        std::cout << "Encode Mat to JPG buffer" << std::endl;
        cv::imencode(".jpg", frame, buffer, param);
    }
    void save_image(std::string filename, cv::Mat &frame)
    {
        std::cout << "saveImage..." << std::endl;
        std::vector<uchar> buffer;
        convertImage(frame, buffer);
        FILE *fp = fopen(filename.c_str(), "wb");
        fwrite(buffer.data(), 1, buffer.size(), fp);
        fclose(fp);
        std::cout << "saveImage done." << std::endl;
    }

    cv::Mat *rotation(cv::Mat &frame, double angle)
    {

        cv::Point2f center((frame.cols - 1) / 2.0, (frame.rows - 1) / 2.0);
        // using getRotationMatrix2D() to get the rotation matrix
        cv::Mat rotation_matix = getRotationMatrix2D(center, angle, 1.0);

        // we will save the resulting image in rotated_image matrix
        cv::Mat *rotated_image = new cv::Mat();
        // rotate the image using warpAffine
        warpAffine(frame, *rotated_image, rotation_matix, frame.size());
        return rotated_image;
    }
    cv::Mat *image_filter(cv::Mat &frame, float vertical_fk[3][3])
    {
        // float vertical_fk[5][5] = {{0,0,0,0,0}, {0,0,0,0,0}, {-1,-2,6,-2,-1}, {0,0,0,0,0}, {0,0,0,0,0}};
        // float horizontal_fk[5][5] = {{0,0,-1,0,0}, {0,0,-2,0,0}, {0,0,6,0,0}, {0,0,-2,0,0}, {0,0,-1,0,0}};
        // float vertical_fk[5][5] = {{1,0,0,0,0}, {1,0,0,0,0}, {1,1,1,1,1}, {1,0,0,0,0}, {1,0,0,0,0}};
        // Embossed : 压花
        // float vertical_fk[3][3] = {
        //     {0,-3,-3},
        //     {3,0,-3},
        //     {3,3,0}};
        // Outline
        // float horizontal_fk[5][5] = {{0,0,-1,0,0}, {0,0,-1,0,0}, {0,0,1,0,0}, {0,0,-1,0,0}, {0,0,-1,0,0}};
        cv::Mat filter_kernel = cv::Mat(3, 3, CV_32FC1, vertical_fk);
        cv::Mat *img_filtered = new cv::Mat();
        filter2D(frame, *img_filtered, -1, filter_kernel);
        std::cout << "frame size => cols : " << frame.cols << ", rows : " << frame.rows << std::endl;
        std::cout << "img_filtered size => cols : " << img_filtered->cols << ", rows : " << img_filtered->rows << std::endl;
        return img_filtered;
    }

    cv::Mat *grey(cv::Mat &frame)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat *image = new cv::Mat();
        // rotate the image using warpAffine
        cv::cvtColor(frame, *image, CV_BGR2GRAY);
        float vertical_fk[3][3] = {
            {-1, -1, -1},
            {-1, 9, -1},
            {-1, -1, -1}};

        return image_filter(*image, vertical_fk);
    }

    cv::Mat *blurImage(cv::Mat &frame)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat *image = new cv::Mat();
        // rotate the image using warpAffine
        cv::blur(frame, *image, cv::Size(3, 3));
        return image;
    }
    cv::Mat *gaussinblurImage(cv::Mat &frame)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat *image = new cv::Mat();
        cv::GaussianBlur(frame, *image, cv::Size(5, 5), 0, 0);
        return image;
    }

    /*

        stylization(Mat src, Mat dst, float sigma_s=60, float sigma_r=0.45f)
        Stylization aims to produce digital imagery with a wide variety of effects not focused on photorealism. Edge-aware filters are ideal for stylization, as they can abstract regions of low contrast while preserving, or enhancing, high-contrast features.

        Parameters
        src	Input 8-bit 3-channel image.
        dst	Output image with the same size and type as src.
        sigma_s	Range between 0 to 200.
        sigma_r	Range between 0 to 1.
    */
    cv::Mat *styleImage(cv::Mat &frame)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat image;
        cv::Mat *style_image = new cv::Mat();
        cv::GaussianBlur(frame, image, cv::Size(5, 5), 0, 0);
        cv::stylization(image, *style_image, 5, 0.1);
        return style_image;
    }

    /*
        Pencil-like non-photorealistic line drawing.

        Parameters
        src	Input 8-bit 3-channel image.
        dst1	Output 8-bit 1-channel image.
        dst2	Output image with the same size and type as src.
        sigma_s	Range between 0 to 200.
        sigma_r	Range between 0 to 1.
        shade_factor	Range between 0 to 0.1.
    */
    void sketch(cv::Mat &frame, cv::Mat &greyFrame, cv::Mat &colorFrame, float sigma_s, float sigma_r, float shade_factor)
    {
        // we will save the resulting image in rotated_image matrix
        cv::pencilSketch(frame, greyFrame, colorFrame, sigma_s, sigma_r, shade_factor);
    }
    cv::Mat *sketchImage(cv::Mat &frame)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat image;
        cv::Mat *style_image = new cv::Mat();
        cv::Mat image1;
        cv::pencilSketch(frame, *style_image, image1);
        return style_image;
    }
    cv::Mat *sketchColorImage(cv::Mat &frame)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat image;
        cv::Mat *style_image = new cv::Mat();
        cv::Mat image1;
        cv::pencilSketch(frame, image1, *style_image);
        return style_image;
    }

    /*
        detailEnhance(Mat src, Mat dst, float sigma_s=10, float sigma_r=0.15f)
        This filter enhances the details of a particular image.
        Parameters
        src	Input 8-bit 3-channel image.
        dst	Output image with the same size and type as src.
        sigma_s	Range between 0 to 200.
        sigma_r	Range between 0 to 1.
    */
    cv::Mat *detailEnhanceImage(cv::Mat &frame, float sigma_s, float sigma_r)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat image;
        cv::Mat *style_image = new cv::Mat();
        cv::Mat image1;
        cv::detailEnhance(frame, *style_image, sigma_s, sigma_r);
        cv::putText(*style_image, "Detail Enhanced", cv::Point(50, 45), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255), 1);

        return style_image;
    }
    cv::Mat *medianblurImage(cv::Mat &frame)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat *image = new cv::Mat();
        cv::medianBlur(frame, *image, 5);
        return image;
    }
    cv::Mat *bilateralImage(cv::Mat &frame)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat *image = new cv::Mat();
        cv::bilateralFilter(frame, *image, 9, 75, 75);
        return image;
    }

    cv::Mat *flipImage(cv::Mat &frame, int direction)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat *image = new cv::Mat();
        // rotate the image using warpAffine
        cv::flip(frame, *image, direction);
        return image;
    }

    cv::Mat *normalizeImage(cv::Mat &frame)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat *image = new cv::Mat();
        // rotate the image using warpAffine
        cv::normalize(frame, *image, 20, 50, cv::NORM_MINMAX, -1, cv::noArray());
        return image;
    }
    /*
     *
     *\(g(x)=\alpha \cdot f(x)+\beta \)\
     *  (f(x)\): The original pixel value.\
     *  (g(x)\): The resulting pixel value after adjustment.
     *  Alpha (\(\alpha \)) — Contrast ControlOften referred to as the gain parameter,
     *  alpha scales the pixel values.
     *  - High Contrast (\(\alpha > 1\)): Increasing alpha expands the difference between light and dark areas,
     *  making the image appear more vivid.
     *  - Low Contrast (\(0 < \alpha < 1\)): Decreasing alpha compresses the pixel values, making the image look flatter or more gray.
     *  No Change (\(\alpha = 1\)): The contrast remains as it was in the original image.Beta
     *  (\(\beta \)) — Brightness ControlOften referred to as the bias parameter,
     *  beta adds or subtracts a constant value to every pixel.
     *  Increase Brightness (\(\beta > 0\)): Shifts all pixel values toward white.
     */
    void contrastImage(cv::Mat &frame, std::vector<uchar> &buffer, double alpha, int beta)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat image;
        frame.convertTo(image, -1, alpha, beta);
        convertImage(image, buffer);
    }

    /*
     * alpha : contrast
     * beta : brightness, bigger
     * g(x) = alpha * f(x) + beta
     * Beta - Lightness/Brightness: Controls the overall additive brightness (lightness) of the image.
     *      A positive value makes the image lighter, while a negative value makes it darker.
     * Alpha - Contrast: Controls the contrast. Values (1.0) keep the image as-is,
     *      values (>1.0) increase contrast (making bright pixels brighter and dark pixels darker),
     *      and values (<1.0) reduce contrast (making the image look flatter).
     */
    cv::Mat *contrast(cv::Mat &frame, double alpha, int beta)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat *image = new cv::Mat();
        frame.convertTo(*image, -1, alpha, beta);
        return image;
    }

    /*

        edgePreservingFilter(Mat src, Mat dst, int flags=1, float sigma_s=60, float sigma_r=0.4f)

        Filtering is the fundamental operation in image and video processing. Edge-preserving smoothing filters are used in many different applications [102] .

        Parameters
        src	Input 8-bit 3-channel image.
        dst	Output 8-bit 3-channel image.
        flags	Edge preserving filters: cv::RECURS_FILTER or cv::NORMCONV_FILTER
        sigma_s	Range between 0 to 200.
        sigma_r	Range between 0 to 1.
    */
    cv::Mat *edgePreservingImage(cv::Mat &frame,double sigma_s,double sigma_r)
    {

        // we will save the resulting image in rotated_image matrix
        cv::Mat image;
        cv::Mat *style_image = new cv::Mat();
        cv::Mat image1;
        // cv::edgePreservingFilter(frame, *style_image, 1, 100, 0.25);
        cv::edgePreservingFilter(frame, *style_image, 1, sigma_s,sigma_r);
        return style_image;
    }

    cv::Mat *readVideo(int index, std::string &filename, std::vector<uchar> &buffer)
    {
        // 1. Create a VideoCapture object
        // Pass the file path string for a video file, or 0 for the default webcam
        cv::VideoCapture cap(filename);

        // 2. Check if the video opened successfully
        if (!cap.isOpened())
        {
            std::cout << "Error: Could not open video." << std::endl;
            return nullptr;
        }

        int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
        std::cout << "Total Frames: " << totalFrames << std::endl;

        if (index < totalFrames)
        {
            cap.set(cv::CAP_PROP_POS_FRAMES, index);
            std::cout << "Moved to frame: " << index << std::endl;
        }

        cv::Mat *frame = new cv::Mat();

        // 3. Read a new frame from the video
        if (!cap.read(*frame))
        {
            std::cout << "End of video stream." << std::endl;
            return nullptr;
        }
        convertImage(*frame, buffer);

        // 6. Release resources
        cap.release();
        cv::destroyAllWindows();

        return frame;
    }

    void read_frame(int index, std::string &filename, cv::Mat &frame)
    {
        // 1. Create a VideoCapture object
        // Pass the file path string for a video file, or 0 for the default webcam
        cv::VideoCapture cap(filename);

        // 2. Check if the video opened successfully
        if (!cap.isOpened())
        {
            std::cout << "Error: Could not open video." << std::endl;
        }

        int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
        std::cout << "Total Frames: " << totalFrames << std::endl;

        if (index < totalFrames)
        {
            cap.set(cv::CAP_PROP_POS_FRAMES, index);
            std::cout << "Moved to frame: " << index << std::endl;
        }

        // 3. Read a new frame from the video
        if (!cap.read(frame))
        {
            std::cout << "End of video stream." << std::endl;
        }

        // 6. Release resources
        cap.release();
        cv::destroyAllWindows();
    }

    int save_video(std::string &in_file, std::string &out_file)
    {
        std::cout << "save_video::save the video" << std::endl;
        // 1. Create a VideoCapture object
        // Pass the file path string for a video file, or 0 for the default webcam
        cv::VideoCapture cap(in_file);

        // 2. Check if the video opened successfully
        if (!cap.isOpened())
        {
            std::cout << "Error: Could not open video." << std::endl;
            return -1;
        }
        std::cout << "save_video::save the video,001" << std::endl;
        cv::Size S = cv::Size((int)cap.get(cv::CAP_PROP_FRAME_WIDTH), (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT));

        /*
            Codec Options :
                DIVX: Known for producing smaller file sizes while maintaining decent quality, making it suitable for web videos.
                XVID: Another popular codec, often used in video compression for distribution due to its balance of quality and file size.
                MJPG: This codec compresses each frame as a JPEG image, which is simple and fast but can lead to larger files.
                H264: Widely used and highly efficient, providing excellent compression with high-quality output, commonly used in .mp4 files.
        */
        cv::VideoWriter output(out_file, cv::VideoWriter::fourcc('D', 'I', 'V', 'X'), 20, S);

        cv::Mat frame;
        cv::Mat frame_target;
        double alpha = 2.2; // Contrast control [1.0-3.0]
        int beta = 50;      // Brightness control [0-100]
        int count = 0;
        int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
        std::cout << "total count=>" << totalFrames << std::endl;
        while (true)
        {
            // 3. Read a new frame from the video
            if (!cap.read(frame))
            {
                std::cout << "End of video stream." << std::endl;
                break;
            }
            count++;
            std::cout << "count=>" << count << std::endl;
            frame.convertTo(frame_target, -1, alpha, beta);
            output << frame_target;
            // output << frame;
        }
        std::cout << "save_video::save the video,002" << std::endl;

        // 6. Release resources
        cap.release();
        cv::destroyAllWindows();

        return 0;
    }
    cv::Mat *shift_frame(cv::Mat &frame,double tx,double ty) {
        // std::cout << "shift_frame::tx=" << tx << ",ty=" << ty << std::endl;
        // 2. Define the horizontal shift distance (tx)
        // Positive values shift right, negative values shift left
        // double tx = 50.0;
        // double ty = 0.0;  // No vertical shift

        // 3. Create the 2x3 affine transformation matrix
        cv::Mat M = (cv::Mat_<double>(2, 3) << 1, 0, tx, 0, 1, ty);
        // 4. Create an output matrix with the same size and type as the source
        cv::Mat *shiftedImage = new cv::Mat();
        // 5. Apply the affine transformation
        // Example: Bright Green is Blue=0, Green=255, Red=0
        cv::Scalar backgroundColor(255, 255, 255);
        // cv::warpAffine(frame, *shiftedImage, M, frame.size());
        cv::warpAffine(
            frame,
            *shiftedImage,
            M,
            frame.size(),
            cv::INTER_LINEAR,
            cv::BORDER_CONSTANT, // Tells OpenCV to use a constant color for borders
            backgroundColor      // Passes your custom color scalar
        );

        return shiftedImage;
    }
    cv::Mat *shift_frame_vertically(cv::Mat &frame,double shift) {
        return shift_frame(frame,0,shift);
    }
    cv::Mat *shift_frame_horizentally(cv::Mat &frame,double shift) {
        return shift_frame(frame,shift,0);
    }
    void skin_smooth(cv::Mat &src,cv::Mat &smoothedImage) {

        // 2. Apply Bilateral Filter for Skin Smoothing
        // Parameters to tweak:
        // d: Neighborhood diameter (larger means more smoothing)
        // sigmaColor: Filter sigma in color space (larger values mix more colors together)
        // sigmaSpace: Filter sigma in coordinate space (larger values mean farther pixels influence each other)
        int d = 9;
        double sigmaColor = 75;
        double sigmaSpace = 75;
        cv::bilateralFilter(src, smoothedImage, d, sigmaColor, sigmaSpace);
    }
    void load_image() {
        std::cout << "OpenCV version: " << CV_VERSION << std::endl;
        // Load the image
        cv::Mat src = cv::imread("IMG_2333.jpeg");
        if (src.empty()) {
            std::cout << "Could not open or find the image" << std::endl;
            return;
        }

        // Load pre-trained Haar Cascade for Face Detection
        cv::CascadeClassifier face_cascade;
        std::cout << "Loading face cascade..." << std::endl;
        if (!face_cascade.load("haarcascade_frontalface_alt.xml")) {
            std::cout << "--(!)Error loading face cascade" << std::endl;
            return;
        }

        std::cout << "Detect faces..." << std::endl;
        // Detect faces
        std::vector<cv::Rect> faces;
        cv::Mat gray;
        cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        equalizeHist(gray, gray);
        face_cascade.detectMultiScale(gray, faces, 1.1, 4, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

        // Create a copy to hold our beautified result
        cv::Mat result = src.clone();

        // Process each detected face
        std::cout << "Beautifying faces...faces.size()=" << faces.size() << std::endl;
        for (size_t i = 0; i < faces.size(); i++) {
            cv::Rect face_rect = faces[i];
            cv::Mat faceROI = result(face_rect);

            // Apply Bilateral Filter for skin smoothing
            // Parameters: input, output, diameter of pixel neighborhood, sigmaColor, sigmaSpace
            cv::Mat smoothed_face;
            bilateralFilter(faceROI, smoothed_face, 9, 85, 85);

            // Place the smoothed face back into the image
            smoothed_face.copyTo(result(face_rect));
        }
        // Save and display the result
        cv::Mat blur_image;
        // cv::blur(result, blur_image, cv::Size(50, 50));
        // cv::blur(blur_image, result, cv::Size(2, 2));
        for (int i=1; i<51; i=i+2)
        {
            // smooth the image in the "src" and save it to "dst"
            // blur(src, dst, Size(i,i));

            // Gaussian smoothing
            GaussianBlur( src, blur_image, cv::Size( i, i ), 0, 0 );

            // Median smoothing
            // medianBlur( result, blur_image, i );

            // show the blurred image with the text
            cv::imshow( "Median filter", blur_image );

            // wait for 5 seconds
            cv::waitKey(5000);
        }
        // cv::GaussianBlur(result, blur_image, cv::Size(5, 5), 0, 0);
        // cv::addWeighted(blur_image, 2.0, result, -1.0, 0);
        // cv::addWeighted(blur_image, 2.0, result, -0.5, 0, result);
        // imwrite("beautified_face.jpg", result);
        // imshow("Beautified Face", result);
        // cv::waitKey(0);
    }

    cv::Scalar getMatColor(cv::Mat &src) {
        int my_rd = haru_random.getRandom();
        cv::Vec3b intensity = src.at<cv::Vec3b>(10*my_rd, 10*my_rd);
        uchar blue = intensity.val[0];
        uchar green = intensity.val[1];
        uchar red = intensity.val[2];
        // return cv::Scalar(blue, green, red);
        return cv::Scalar(blue, green, red);
    }
    cv::Mat *resize_image(cv::Mat src,double scale,int bgWidth,int bgHeight) {
        // 1. Define the size of the background image and create it (e.g., black background)
        // cv::Mat background = cv::Mat::zeros(bgHeight, bgWidth, CV_8UC3);
        // cv::Mat *background = new cv::Mat(cv::Mat(bgHeight, bgWidth, CV_8UC3, BACKGROUND_COLOR));
        int source_height = src.rows; // Height of the image
        int source_width = src.cols;
        double _scale = static_cast<double>(bgWidth) / src.cols;
        int targetHeight = cvRound(src.rows * _scale);
        cv::Mat *background = new cv::Mat(cv::Mat(bgHeight, bgWidth, CV_8UC3, getMatColor(src)));
        // 2. Load the image you want to place
        cv::Mat scaledImage;
        cv::resize(src, scaledImage, cv::Size(bgWidth,targetHeight), 0,0, cv::INTER_CUBIC);
        // 3. Define where you want to place the image (top-left coordinates)

        int scaledImage_width = scaledImage.cols;
        int x_offset = bgWidth/2 - scaledImage_width/2;
        if (x_offset < 0) {
            x_offset = 0;
        }
        int y_offset = bgHeight/2 - scaledImage.rows/2;
        int y_offset_origin = y_offset;
        if (y_offset < 0) {
            y_offset = 0;
        }
        // std::cout << "x_offset=" << x_offset << ",y_offset=" << y_offset << std::endl;
        // 4. Create a rectangle for the ROI ensuring it does not exceed background boundaries
        int width = std::min(scaledImage.cols, (*background).cols - x_offset);
        int height = std::min(scaledImage.rows, (*background).rows - y_offset);

        if (width > 0 && height > 0) {
            // 5. Define the ROI on the background and copy the overlay image into it
            // cv::Mat roi = (*background)(cv::Rect(0, 0, width, height));
            // overlayImage(cv::Rect(0, 0, width, height)).copyTo(roi);
            cv::Mat roi1 = (*background)(cv::Rect(x_offset, y_offset, width, height));
            if (y_offset_origin < 0) {
                scaledImage(cv::Rect(0, std::abs(y_offset_origin), width, height)).copyTo(roi1);
            } else {
                scaledImage(cv::Rect(0, 0, width, height)).copyTo(roi1);
            }
        }
        return background;
    }
    cv::Mat *place_image_with_background(cv::Mat src,double scale,int bgWidth,int bgHeight) {
        // 1. Define the size of the background image and create it (e.g., black background)
        // cv::Mat background = cv::Mat::zeros(bgHeight, bgWidth, CV_8UC3);
        cv::Mat *background = new cv::Mat(cv::Mat(bgHeight, bgWidth, CV_8UC3, BACKGROUND_COLOR));
        // 2. Load the image you want to place
        cv::Mat overlayImage;
        cv::resize(src, overlayImage, cv::Size(), scale, scale, cv::INTER_LINEAR);
        // 3. Define where you want to place the image (top-left coordinates)

        int overlayImage_width = overlayImage.cols;
        int x_offset = bgWidth/2 - overlayImage_width/2;
        if (x_offset < 0) {
            x_offset = 0;
        }
        int y_offset = bgHeight/2 - overlayImage.rows/2;
        if (y_offset < 0) {
            y_offset = 0;
        }
        std::cout << "x_offset=" << x_offset << ",y_offset=" << y_offset << std::endl;
        // 4. Create a rectangle for the ROI ensuring it does not exceed background boundaries
        int width = std::min(overlayImage.cols, (*background).cols - x_offset);
        int height = std::min(overlayImage.rows, (*background).rows - y_offset);

        if (width > 0 && height > 0) {
            // 5. Define the ROI on the background and copy the overlay image into it
            cv::Mat roi = (*background)(cv::Rect(x_offset, y_offset, width, height));
            overlayImage(cv::Rect(0, 0, width, height)).copyTo(roi);
        }
        return background;
    }
    int play_image_with_background(std::string &input_image_path,int bgWidth,int bgHeight) {
        // 2. Load the image you want to place
        cv::Mat src = cv::imread(input_image_path);
        if (src.empty()) {
            std::cout << "Could not open or find the image!" << std::endl;
            return -1;
        }
        std::cout << "Image loaded successfully!" << std::endl;
        std::cout << "Image loaded size=(w,h)=(" << src.cols << "," << src.rows << ")" << std::endl;
        cv::Mat *frame = place_image_with_background(src,0.5,bgWidth,bgHeight);
        // 6. Display the result
        cv::imshow("Result", *frame);
        cv::waitKey(0);
        delete frame;
        return 0;
    }
    void onTrackbarSlide( int pos, void *) {
        g_cap.set( cv::CAP_PROP_POS_FRAMES, pos );
        if( !g_dontset )
            g_run = 1;
        g_dontset = 0;
    }
    void save_image(cv::Mat& frame,std::string filename) {
        cv::imwrite(filename, frame);
    }
    void load_video(std::string filename,std::filesystem::path parent_path) {
        cv::namedWindow( "Example2_4", cv::WINDOW_AUTOSIZE );
        cv::VideoCapture g_cap;
        g_cap.open(filename);
        int frames = (int) g_cap.get(cv::CAP_PROP_FRAME_COUNT);
        int tmpw = (int) g_cap.get(cv::CAP_PROP_FRAME_WIDTH);
        int tmph = (int) g_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        std::cout << "Video has " << frames << " frames of dimensions("
        << tmpw << ", " << tmph << ")." << std::endl;
        cv::createTrackbar("Position", "Example2_4", &g_slider_position, frames,
        onTrackbarSlide);
        cv::Mat frame;
        for(;;) {
            if( g_run != 0 ) {
                g_cap >> frame; if(frame.empty()) break;
                int current_pos = (int)g_cap.get(cv::CAP_PROP_POS_FRAMES);
                g_dontset = 1;
                cv::setTrackbarPos("Position", "Example2_4", current_pos);
                cv::imshow( "Example2_4", frame );
                g_run-=1;
            }
            char c = (char) cv::waitKey(10);
            //single mode
            if( c == 's' ){
                g_run = 1;
                std::cout << "Single step, run = " << g_run << std::endl;
            }
            //run mode
            if( c == 'r' ) {
                g_run = -1;
                std::cout << "Run mode, run = " << g_run << std::endl;
            }
            //save the image to the file system
            if( c == 'w' ) {
                std::string filename = parent_path.string() + "/frame_" + std::to_string(g_slider_position) + ".jpg";
                save_image(frame,filename);
                std::cout << "Save mode, run = " << g_run << std::endl;
                cv::namedWindow( "Example2_5-in", cv::WINDOW_AUTOSIZE );
                cv::namedWindow( "Example2_5-out", cv::WINDOW_AUTOSIZE );
                cv::Mat out;
                cv::GaussianBlur( frame, out, cv::Size(5,5), 3, 3);
                cv::GaussianBlur( out, out, cv::Size(5,5), 3, 3);
                cv::imshow( "Example2_5-in", frame );
                cv::imshow( "Example2_5-out", out );
            }
            if( c == 27 )
                break;
        }
    }
    void combine_image(cv::Mat &img1,cv::Mat &img2) {
        // Force matching heights for horizontal stacking if necessary
        if (img1.rows != img2.rows) {
            cv::resize(img2, img2, cv::Size(img2.cols * img1.rows / img2.rows, img1.rows));
        }

        // Combine horizontally
        cv::Mat combined_horizontal;
        cv::hconcat(img1, img2, combined_horizontal);

        // Save and display
        cv::imwrite("combined_side_by_side.jpg", combined_horizontal);
        cv::imshow("Horizontal Merge", combined_horizontal);
        cv::waitKey(0);
    }
    void combine_image(std::string &filename1,std::string &filename2) {
        cv::Mat img1 = cv::imread(filename1);
        cv::Mat img2 = cv::imread(filename2);
        if (img1.empty() || img2.empty()) {
            std::cout << "Could not open or find the images!" << std::endl;
            return;
        }
        combine_image(img1,img2);
    }
    void combine_overlay(cv::Mat &img1,cv::Mat &img2) {
        // Images must be identical size for blending
        cv::resize(img2, img2, img1.size());

        double alpha = 0.3; // Weight of the first image (0.0 to 1.0)
        double beta = 0.7;  // Weight of the second image (1.0 - alpha)
        double gamma = 0.2; // Scalar added to each sum
        cv::Mat blended_image;

        // Blend the images
        cv::addWeighted(img1, alpha, img2, beta, gamma, blended_image);

        cv::imwrite("blended_output.jpg", blended_image);
        cv::imshow("Blended Merge", blended_image);
        cv::waitKey(0);
    }
    void combine_overlay(std::string &filename1,std::string &filename2) {
        cv::Mat img1 = cv::imread(filename1);
        cv::Mat img2 = cv::imread(filename2);
        if (img1.empty() || img2.empty()) {
            std::cout << "Could not open or find the images!" << std::endl;
            return;
        }
        combine_overlay(img1,img2);
    }
}
//
// Created by developer on 2026-05-19.
//
