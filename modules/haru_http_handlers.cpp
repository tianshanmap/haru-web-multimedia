#include <stdexcept> // Required for standard exceptions

#include "haru_yaml.h"
#include "web_models.h"
#include "../include/json.hpp"
#include "../include/httplib.hpp"

using json = nlohmann::json;
namespace haru {
    VideoCreateRequest get_video_create_request(const httplib::Request &req) {
        json incoming_json = json::parse(req.body);
        VideoCreateRequest request;
        request.image_path = incoming_json["image_path"];
        request.video_name = incoming_json["video_name"];
        request.audio_files = incoming_json["audio_files"];
        return request;
    }
    VideoCreateRequestV1 get_video_create_request_v1(const httplib::Request &req) {
        json incoming_json = json::parse(req.body);
        VideoCreateRequestV1 request;
        try {
            request.image_path = incoming_json["image_path"];
            request.audio_name = incoming_json["audio_name"];
            request.video_name = incoming_json["video_name"];
            request.image_files = incoming_json["image_files"];
            request.fps = incoming_json["fps"];
            request.scale = incoming_json["scale"];
            request.width = incoming_json["width"];
            request.height = incoming_json["height"];
            std::string codec = incoming_json["codec"];
            request.haruCodec[0] = codec[0];
            request.haruCodec[1] = codec[1];
            request.haruCodec[2] = codec[2];
            request.haruCodec[3] = codec[3];
        }
        catch (const std::exception& e) {
            std::cout << "Error*****" << std::endl;   // Code to handle the specific exception
            std::cout << e.what() << std::endl;   // Code to handle the specific exception
        }
        return request;
    }
    AudioCreateRequest get_audio_create_request(const httplib::Request &req) {
        json incoming_json = json::parse(req.body);
        AudioCreateRequest request;
        request.audio_files = incoming_json["audio_files"];
        return request;
    }
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AudioCreateResponse, name);
    std::string get_audio_create_response(AudioCreateResponse &response) {
        nlohmann::json j = response;
        return j.dump();
    }
    std::string get_upload_target_path(const YamlConfig &config) {
        nlohmann::json response = {
            {"path", config.upload_target_path},
            {"max_size", config.upload_max_size},
          };
        return response.dump();
    }
    TextSaveRequest get_text_save_request(const httplib::Request &req) {
        json incoming_json = json::parse(req.body);
        TextSaveRequest request;
        request.file_path = incoming_json["file_path"];
        request.content = incoming_json["content"];
        request.created_by = incoming_json["created_by"];
        std::filesystem::path path = request.file_path;
        request.parent_path = path.parent_path();
        return request;
    }
    std::string get_text_load_response(const TextLoadResponse &res){
        nlohmann::json response = {
            {"name", res.name},
            {"content", res.content},
          };
        return response.dump();
    }
}