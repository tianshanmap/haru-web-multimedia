//
// Created by developer on 2026-05-27.
//

#ifndef HARU_OPENCV_WEB_HARU_YAML_H
#define HARU_OPENCV_WEB_HARU_YAML_H
#include <iostream>
#include <yaml-cpp/yaml.h>
namespace haru {
    struct YamlConfig {
        std::string yaml_path;
        std::string source;
        std::string export_path;
        std::string media_mp3_path;
        std::string media_mp3_path_export;
        std::string final_photoes_export;
        std::string host;
        int port;
        std::string static_path;
        std::string media_audio_path;
        std::string media_audio_workspace;
        std::string media_video_export;
        std::string upload_target_path;
        int upload_max_size;
        void load_yaml() {
            try {
                // Load the file into a Node object
                YAML::Node config = YAML::LoadFile(yaml_path);

                // 2. Read nested maps
                if (config["photoes"]) {
                    source = config["photoes"]["source"].as<std::string>();
                    export_path = config["photoes"]["export_path"].as<std::string>();
                    media_mp3_path = config["photoes"]["media_mp3_path"].as<std::string>();
                    media_mp3_path_export = config["photoes"]["media_mp3_path_export"].as<std::string>();
                    final_photoes_export = config["photoes"]["final_photoes_export"].as<std::string>();
                }
                if (config["media"]) {
                    media_audio_path = config["media"]["audio_path"].as<std::string>();
                    media_audio_workspace = config["media"]["audio_workspace"].as<std::string>();
                    media_video_export = config["media"]["video_export"].as<std::string>();
                }
                if (config["web"]) {
                    host = config["web"]["host"].as<std::string>();
                    static_path = config["web"]["static_path"].as<std::string>();
                    port = config["web"]["port"].as<int>();
                }
                if (config["upload"]) {
                    upload_target_path = config["upload"]["target_path"].as<std::string>();
                    upload_max_size = config["upload"]["max_size"].as<int>();
                }
            } catch (const YAML::Exception& e) {
                std::cerr << "YAML Error: " << e.what() << std::endl;
            }
        }
    };
}
#endif //HARU_OPENCV_WEB_HARU_YAML_H
