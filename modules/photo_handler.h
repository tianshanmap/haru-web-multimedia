//
// Created by developer on 2026-05-22.
//
#include "haru_yaml.h"
#include "web_models.h"
#ifndef HARU_OPENCV_WEB_PHOTO_HANDLER_H
#define HARU_OPENCV_WEB_PHOTO_HANDLER_H
namespace haru {
    void process_photoes(std::string path,std::string videoFile);
    void process_photoes_v1(VideoProcessRequest &request);
    void create_video(YamlConfig &config);
    std::vector<std::string> make_video_from_photoes(std::string source,std::string export_path);
}
#endif //HARU_OPENCV_WEB_PHOTO_HANDLER_H
