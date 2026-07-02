//
// Created by developer on 2026-06-08.
//
#include <vector>
#ifndef HARU_OPENCV_WEB_WEB_MODELS_H
#define HARU_OPENCV_WEB_WEB_MODELS_H
#include <string>
namespace haru {
    struct AudioCreateRequest {
        std::vector<std::string> audio_files;
    };
    struct AudioCreateResponse {
        std::string name;
    };
    struct VideoCreateRequest {
        std::string video_name;
        std::string image_path;
        std::vector<std::string> audio_files;
    };
    struct VideoCreateRequestV1 : public VideoCreateRequest {
        int fps;
        char haruCodec[4];
        int width;
        int height;
        double scale;
        std::string audio_name;
        std::string video_name;
        std::vector<std::string> image_files;
    };
    struct TextSaveRequest {
        std::string file_path;
        std::string content;
        std::string created_by;
        std::string parent_path;
    };
    struct TextLoadResponse {
        std::string name;
        std::string content;
    };
    struct VideoCreateResponse {
        std::string name;
    };
    struct VideoProcessRequest {
        int fps;
        char haruCodec[4];
        int width;
        int height;
        double scale;
        std::string videoFile;
        std::vector<std::string> files;
    };
}
#endif //HARU_OPENCV_WEB_WEB_MODELS_H
