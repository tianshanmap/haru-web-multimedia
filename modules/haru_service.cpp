#include <iostream>
#include "./opencv_utils.h"
#include "photo_handler.h"
#include "haru_ffmpeg.h"
#include "web_models.h"
#include "../include/json.hpp"

using json = nlohmann::json;
namespace harusvc {
    std::string create_video_v1(haru::VideoCreateRequestV1 &video_create_request) {
        std::cout << "--create_video :: video_create_request" << video_create_request.video_name << std::endl;
        haru::VideoProcessRequest request;
        request.fps = video_create_request.fps;
        request.scale = video_create_request.scale;
        request.width = video_create_request.width;
        request.height = video_create_request.height;
        request.files = video_create_request.image_files;
        request.videoFile = video_create_request.video_name;
        request.haruCodec[0] = video_create_request.haruCodec[0];
        haru::process_photoes_v1(request);
        std::cout << "--create_video :: add_mp3_to_mp4-started" << std::endl;
        haru::add_mp3_to_mp4(video_create_request.audio_name,video_create_request.video_name);
        std::cout << "--create_video :: create result" << std::endl;
        json result;
        result["file"] = video_create_request.video_name;
        result["status"] = "success";
        std::cout << "--create_video :: result-created" << std::endl;
        std::string response = result.dump();
        std::cout << "--create_video :: result-created" << response <<std::endl;
        return response;
    }
    std::string create_video(std::string &image_path,std::string audio_path,std::string &video_path) {
        std::cout << "create_video :: audio_path=" << audio_path << std::endl;
        std::cout << "create_video :: video_path=" << video_path << std::endl;
        std::cout << "create_video :: process_photoes-started" << std::endl;
        haru::process_photoes(image_path,video_path);
        std::cout << "create_video :: add_mp3_to_mp4-started" << std::endl;
        haru::add_mp3_to_mp4(audio_path,video_path);
        std::cout << "create_video :: create result" << std::endl;
        json result;
        result["file"] = video_path;
        result["status"] = "success";
        std::cout << "create_video :: result-created" << std::endl;
        return result.dump();
    }
}
