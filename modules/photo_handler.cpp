//
// Created by developer on 2026-05-22.
//
#include <iostream>
#include <thread>
#include <filesystem>
#include "file_utils.h"
#include "opencv_utils.h"
#include "haru_random.h"
#include "haru_ffmpeg.h"
#include "haru_yaml.h"
#include "web_models.h"

namespace haru {
    const double HARU_SCALE = 0.4;
    const int HARU_FRAME_WIDTH = 1512;
    const int HARU_FRAME_HEIGHT = 1024;
    const cv::Size HARU_IMAGE_SIZE = cv::Size(HARU_FRAME_WIDTH,HARU_FRAME_HEIGHT);
    const int HARU_CODEC = cv::VideoWriter::fourcc('a', 'v', 'c', '1');
    const int HARU_FPS = 5;

    void flip_frame_horizentally(cv::VideoWriter &output,cv::Mat &frame,int repeat) {
        cv::Mat flippedFrame;
        // ... assume frame is populated ...
        for (int i = 0; i < repeat; i++) {
            cv::flip(frame, flippedFrame, 1); // 1 means flipping horizontally
            output << flippedFrame;
        }
    }
    void flip_frame_vertically(cv::VideoWriter &output,cv::Mat &frame,int repeat) {
        cv::Mat flippedFrame;
        // ... assume frame is populated ...
        for (int i = 0; i < repeat; i++) {
            cv::flip(frame, flippedFrame, 0); // 1 means flipping horizontally
            output << flippedFrame;
        }
    }
    void grey_frame(cv::VideoWriter &output,cv::Mat &frame,int repeat) {
        cv::Mat *frame_target = grey(frame);
        for (int i = 0; i < repeat; i++) {
            output << *frame_target;
        }
        delete frame_target;
    }
    void contrast_frame(cv::VideoWriter &output,cv::Mat &frame,int repeat) {
        double beta = 0 - (repeat-1);
        for (int i = 0; i < repeat; i++) {
            cv::Mat *frame_target = contrast(frame,1.0,beta);
            output << *frame_target;
            delete frame_target;
            beta = beta + i;
        }
    }
    void blur_frame(cv::VideoWriter &output,cv::Mat &frame,int repeat) {
        cv::Mat mat;
        int initial_value = 5;
        for (int i = repeat; i > 0; i--) {
            cv::blur(frame, mat, cv::Size(i+5, i+5));
            output << mat;
        }
    }
    void repeat_frame(cv::VideoWriter &output,cv::Mat &frame,int repeat) {
        for (int i = 0; i < repeat; i++) {
            output << frame;
        }
    }
    void output_shift_frame_vertically(cv::VideoWriter &output,cv::Mat &frame,double shift) {
        cv::Mat *shiftedImage = shift_frame_vertically(frame,shift);
        output << *shiftedImage;
        delete shiftedImage;
    }
    void output_shift_frame_horizentally(cv::VideoWriter &output,cv::Mat &frame,double shift) {
        cv::Mat *shiftedImage = shift_frame_horizentally(frame,shift);
        output << *shiftedImage;
        delete shiftedImage;
    }
    /*
    ffmpeg -i video.mp4 -i audio1.mp3 -i audio2.mp3 -filter_complex \
    "[1:a][2:a]concat=n=2:v=0:a=1[seq]; \
    [seq]aloop=loop=-1:size=2e+09[looped]; \
    [looped]amix=inputs=1:duration=shortest[a]" \
    -shortest \
    -map 0:v -map "[a]" -c:v copy -shortest output.mp4
     */
    void rotate_frame(cv::VideoWriter &output,cv::Mat &frame,double angle,int repeat) {
        // cv::Mat dst;
        // cv::bitwise_not(frame, dst);
        // cv::Mat *frame1 = styleImage(frame);
        cv::Mat *frame_target = rotation(frame,angle);
        // cv::Mat *frame_target1 = contrast(*frame_target,2,1.0);
        for (int i = 0; i < repeat; i++) {
            output << *frame_target;
        }
        // delete frame1;
        delete frame_target;
    }
    void putText(cv::Mat frame,std::string text) {
        cv::Point org(50, 250);             // Bottom-left corner of the text string
        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        double fontScale = 1.0;
        cv::Scalar color(0, 255, 0);        // Green color in BGR format
        int thickness = 2;
        int lineType = cv::LINE_AA;         // Anti-aliased line for smoother rendering
        // 3. Apply the text to the Mat
        cv::putText(frame, text, org, fontFace, fontScale, color, thickness, lineType);
    }
    void process_photoes(std::string path,std::string videoFile){
        std::cout << "process_photoes::path => " << path << std::endl;
        std::cout << "process_photoes::videoFile => " << videoFile << std::endl;
        HaruRandom haru_random(1,9);
        std::vector<std::string> files = getFiles(path,".jpeg");
        std::cout << "Total jpeg files to be handled => " << files.size() << std::endl;
        cv::VideoWriter output(videoFile, HARU_CODEC, HARU_FPS, HARU_IMAGE_SIZE);
        for (std::string file : files) {
            cv::Mat frame = cv::imread(file);
            putText(frame,path);
            // std::cout << "Handle file=> " << file << ",size=" << frame.size << std::endl;
            // cv::Mat resized_img;
            // Resize to 300x200 (Width x Height)
            // cv::resize(frame, resized_img, HARU_IMAGE_SIZE);
            cv::Mat *resized_img = resize_image(frame,HARU_SCALE,HARU_FRAME_WIDTH,HARU_FRAME_HEIGHT);
            // for (int i = 20; i >= 0; i=i-10) {
            //     rotate_frame(output,resized_img,i,1);
            // }
            // flip_frame_horizentally(output,resized_img,2);
            // for (int i = -20; i < 0; i=i+10) {
            //     rotate_frame(output,resized_img,i,1);
            // }
            int my_rd = haru_random.getRandom();
            // std::cout << "Random Number => " << my_rd << std::endl;
            switch (my_rd) {
                case 1:
                    repeat_frame(output,*resized_img,1);
                    for (double i = 0; i <= 400.0; i=i+20.0) {
                        output_shift_frame_horizentally(output,*resized_img,i);
                    }
                    repeat_frame(output,*resized_img,5);
                    break;
                case 2:
                    repeat_frame(output,*resized_img,1);
                    for (double i = 400; i > 0; i=i-20.0) {
                        output_shift_frame_horizentally(output,*resized_img,i);
                    }
                    repeat_frame(output,*resized_img,5);
                    break;
                case 3:
                    repeat_frame(output,*resized_img,1);
                    for (double i = 0; i <= 190.0; i=i+10.0) {
                        output_shift_frame_vertically(output,*resized_img,i);
                        // skin_smooth(frame,resized_img);
                        // output << resized_img;
                    }
                    repeat_frame(output,*resized_img,5);
                    break;
                case 4:
                    contrast_frame(output,*resized_img,20);
                    repeat_frame(output,*resized_img,5);
                    break;
                case 5:
                    contrast_frame(output,*resized_img,20);
                    repeat_frame(output,*resized_img,5);
                    break;
                    // case 5:
                //     repeat_frame(output,*resized_img,20);
                //     break;
                case 6:
                    for (int i = 90; i >= 0; i=i-10) {
                        rotate_frame(output,*resized_img,i,1);
                    }
                    repeat_frame(output,*resized_img,20);
                    break;
                case 7:
                    for (int i = -90; i < 0; i=i+10) {
                        rotate_frame(output,*resized_img,i,1);
                    }
                    repeat_frame(output,*resized_img,20);
                    break;
                case 8:
                    blur_frame(output,*resized_img,10);
                    repeat_frame(output,*resized_img,20);
                    break;
                case 9:
                    blur_frame(output,*resized_img,10);
                    repeat_frame(output,*resized_img,20);
                    break;
            }
            delete resized_img;
        }
        output.release();
        cv::destroyAllWindows();
        std::cout << "process_photoes::Write video to  => " << videoFile << std::endl;
    }
    void process_photoes_v1(VideoProcessRequest &request){
        std::vector<std::string> files = request.files;
        std::string videoFile = request.videoFile;
        std::cout << "Total jpeg files to be handled => " << files.size() << std::endl;
        cv::VideoWriter output(videoFile, HARU_CODEC, request.fps, cv::Size(request.width,request.height));
        HaruRandom haru_random(1,5);
        for (std::string file : files) {
            cv::Mat frame = cv::imread(file);
            cv::Mat *resized_img = resize_image(frame,request.scale,request.width,request.height);
            int my_rd = haru_random.getRandom();
            switch (my_rd) {
                case 1:
                    repeat_frame(output,*resized_img,1);
                    for (double i = 0; i <= 400.0; i=i+20.0) {
                        output_shift_frame_horizentally(output,*resized_img,i);
                    }
                    repeat_frame(output,*resized_img,5);
                    break;
                case 2:
                    repeat_frame(output,*resized_img,1);
                    for (double i = 400; i > 0; i=i-20.0) {
                        output_shift_frame_horizentally(output,*resized_img,i);
                    }
                    repeat_frame(output,*resized_img,5);
                    break;
                case 3:
                    repeat_frame(output,*resized_img,1);
                    for (double i = 0; i <= 190.0; i=i+10.0) {
                        output_shift_frame_vertically(output,*resized_img,i);
                        // skin_smooth(frame,resized_img);
                        // output << resized_img;
                    }
                    repeat_frame(output,*resized_img,5);
                    break;
                case 4:
                    contrast_frame(output,*resized_img,20);
                    repeat_frame(output,*resized_img,5);
                    break;
                case 5:
                    contrast_frame(output,*resized_img,20);
                    repeat_frame(output,*resized_img,5);
                    break;
                    // case 5:
                //     repeat_frame(output,*resized_img,20);
                //     break;
                case 6:
                    for (int i = 90; i >= 0; i=i-10) {
                        rotate_frame(output,*resized_img,i,1);
                    }
                    repeat_frame(output,*resized_img,20);
                    break;
                case 7:
                    for (int i = -90; i < 0; i=i+10) {
                        rotate_frame(output,*resized_img,i,1);
                    }
                    repeat_frame(output,*resized_img,20);
                    break;
                case 8:
                    blur_frame(output,*resized_img,10);
                    repeat_frame(output,*resized_img,20);
                    break;
                case 9:
                    blur_frame(output,*resized_img,10);
                    repeat_frame(output,*resized_img,20);
                    break;
            }
            delete resized_img;
        }
        output.release();
        cv::destroyAllWindows();
        std::cout << "process_photoes::Write video to  => " << videoFile << std::endl;
    }

    std::vector<std::string> make_video_from_photoes(std::string source,std::string export_path) {
        std::vector<std::string> export_mp4_files;
        // std::string source = "/Users/developer/T9/travels/processed";
        // std::string export_path = "/Users/developer/T9/travels/export";
        std::vector<std::string> files;
        find_image_directory(source,files);
        std::vector<std::thread> thread_pool;
        for (auto file : files) {
            std::cout << "Process directory => " << file << std::endl;
            // std::string export_path = create_folder_under(file,"export");
            // std::cout << "Process export directory => " << export_path << std::endl;
            std::filesystem::path filepath(file);
            std::cout << "Process filepath => " << filepath.filename() << std::endl;
            std::string export_mp4_file = export_path + "/" + filepath.parent_path().filename().string() + ".mp4";
            export_mp4_files.push_back(export_mp4_file);
            std::thread th(process_photoes,file,export_mp4_file);
            thread_pool.push_back(std::move(th));
        }
        for (auto &th : thread_pool) {
            th.join();
        }
        return export_mp4_files;
    }
    void create_video(YamlConfig &config) {
        std::string source = config.source;
        std::string export_path = config.export_path;
        std::string export_path_mp3_mp4 = config.final_photoes_export;
        std::string media_mp3_path = config.media_mp3_path;
        std::string media_mp3_processed_path = config.media_mp3_path_export;
        std::cout << "create_video::" << source << " to " << export_path << std::endl;
        std::vector<std::string> export_mp4_files = make_video_from_photoes(source,export_path);
        std::vector<std::string> mp3_files = getFiles(media_mp3_path,".mp3");
        // std::string merged_mp3_output = "/Users/developer/T9/travels/play-music-export";
        HaruMp3Files mp3_files_obj{.mp3_path = media_mp3_processed_path,.files = mp3_files};
        for (std::string video_file : export_mp4_files) {
            std::cout << "video => " << video_file << std::endl;
            std::filesystem::path filePath(video_file);
            std::string filename = export_path_mp3_mp4 + "/" + filePath.filename().string();
            std::cout << "video with mp3 => " << filename << std::endl;
            std::string mp3_filename = mp3_files_obj.merge_mp3();
            std::cout << "merged mp3 => " << mp3_filename << std::endl;
            add_mp3_to_mp4(mp3_filename,video_file,filename);
        }
    }

}
