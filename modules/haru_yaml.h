//
// Created by developer on 2026-05-27.
//

#ifndef HARU_OPENCV_WEB_HARU_YAML_H
#define HARU_OPENCV_WEB_HARU_YAML_H
#include <filesystem>
#include <iostream>
#include <yaml-cpp/yaml.h>

#include "simplelogger/simple_logger.hpp"

namespace haru {
    struct YamlConfig {
        std::string yaml_path;
        std::string host;
        int port;
        std::string static_path;
        std::string prefix;

        std::string media_audio_path;
        std::string media_audio_workspace;
        std::string media_video_export;
        std::string media_video_capture;
        std::string upload_target_path;
        int upload_max_size;
        std::string get_path(std::string&& path) {
            SimpleLogger &logger = SimpleLogger::getInstance();
            std::filesystem::path file_path = path;
            logger.info("haru_yaml.h::get_path::path",path);
            if (file_path.is_absolute()) {
                return path;
            } else {
                try {
                    // Fetch the current working directory
                    std::filesystem::path current_path = std::filesystem::current_path();
                    std::filesystem::path fullPath = current_path / path;
                    std::filesystem::create_directories(fullPath);
                    logger.info("haru_yaml.h::get_path::fullPath",fullPath);
                    return fullPath;
                } catch (const std::filesystem::filesystem_error& e) {
                    std::cerr << "Error detecting directory: " << e.what() << std::endl;
                }
                return path;
            }
        }
        void load_yaml() {
            try {
                // Load the file into a Node object
                YAML::Node config = YAML::LoadFile(yaml_path);

                if (config["web"]) {
                    host = config["web"]["host"].as<std::string>();
                    static_path = get_path(config["web"]["static_path"].as<std::string>());
                    port = config["web"]["port"].as<int>();
                    prefix = config["web"]["prefix"].as<std::string>();
                }
                if (config["media"]) {
                    media_audio_path = get_path(config["media"]["audio_path"].as<std::string>());
                    media_audio_workspace = get_path(config["media"]["audio_workspace"].as<std::string>());
                    media_video_export = get_path(config["media"]["video_export"].as<std::string>());
                    media_video_capture = get_path(config["media"]["video_capture"].as<std::string>());
                }
                if (config["upload"]) {
                    upload_target_path = get_path(config["upload"]["target_path"].as<std::string>());
                    upload_max_size = config["upload"]["max_size"].as<int>();
                }
            } catch (const YAML::Exception& e) {
                std::cerr << "YAML Error: " << e.what() << std::endl;
            }
        }
    };
}
#endif //HARU_OPENCV_WEB_HARU_YAML_H
