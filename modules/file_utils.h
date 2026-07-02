//
// Created by developer on 2026-05-19.
//
#include <vector>
#include <chrono>
#include "../include/json.hpp"
#ifndef HARU_OPENCV_WEB_FILE_UTILS_H
#define HARU_OPENCV_WEB_FILE_UTILS_H
namespace haru {
    struct HaruFileEntry {
        std::string name;
        std::string path;
        std::string parent_path;
        std::string kind;
        unsigned long size;
        std::string last_update;
    };
    struct HaruFolder {
        std::string parent;
        std::string name;
        std::vector<HaruFileEntry> files;
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HaruFileEntry, name,path,kind,parent_path,size,last_update)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(HaruFolder, parent,name,files)
    int scan_directory(const std::string path,std::vector<std::string> &files,const std::string extension);
    std::vector<std::string> create_textfile_for_merge(std::vector<std::string> &files);
    int process_filesystem(std::string path,std::string destination);
    std::vector<std::string> getFiles(std::string path,std::string extension);
    void find_image_directory(std::string path,std::vector<std::string> &files);
    void clear_folder(std::string path);
    std::string create_folder_under(std::string pathname,std::string name);
    HaruFolder get_folder(std::string path);
    std::string get_folder_as_json(std::string path);
    std::string get_audio_as_json(std::string path);
    std::string get_video_as_json(std::string path);
    std::string get_image_as_json(std::string path);
    std::string read_binary_file(const std::string& file_path);
    std::string get_content_type(std::string &path);
    void create_folder(std::string &name,std::string &path);
    void delete_file(std::string &path);
    void delete_folder(std::string &path);
    void move(std::string &path,std::string &target);
    void copy(std::string &path,std::string &target);
    void scanFiles(std::string &path,std::vector<std::string> &files);
    void rename(std::string &file1,std::string &file2);
    std::string getLastWriteTimeStr(const std::filesystem::path& filePath);
    void write_file(std::string &path,std::string &content);
    std::string read_file(std::string &path);
}
#endif //HARU_OPENCV_WEB_FILE_UTILS_H
