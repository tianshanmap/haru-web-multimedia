//
// Created by developer on 2026-06-08.
//
#include <iostream>
#ifndef HARU_OPENCV_WEB_HARU_SERVICE_H
#define HARU_OPENCV_WEB_HARU_SERVICE_H
namespace harusvc {
    std::string create_video(std::string &image_path,std::string audio_path,std::string &video_path);
    std::string create_video_v1(haru::VideoCreateRequestV1 &video_create_request);
}
#endif //HARU_OPENCV_WEB_HARU_SERVICE_H
