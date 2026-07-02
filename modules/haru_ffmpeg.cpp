extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}
#include <iostream>
#include <filesystem>
#include <vector>
#include <sstream>
#include <fstream>
#include <format>
#include "haru_random.h"
#include "haru_ffmpeg.h"
#include "file_utils.h"
namespace haru {
    const int HARU_RANDOM_FFMPEG_MIN = 1;
    const int HARU_RANDOM_FFMPEG_MAX = 18;
    HaruRandom haru_random_ffmpeg(HARU_RANDOM_FFMPEG_MIN,HARU_RANDOM_FFMPEG_MAX);
    void add_mp3_to_mp4(std::string mp3,std::string mp4,std::string output) {
        HaruFFMpegMergeCMD cmd(mp3,mp4,output);
        std::cout << cmd.get_command() << std::endl;
        std::system(cmd.get_command().c_str());
    }
    void add_mp3_to_mp4(std::string mp3,std::string mp4) {
        std::filesystem::path mpath(mp4);
        std::string output = mpath.parent_path().string() + "/final-" + mpath.filename().string();
        add_mp3_to_mp4(mp3,mp4,output);
        std::cout << "delete file => " << mp4 << std::endl;
        delete_file(mp4);
        std::cout << "rename file => " << output << " to " << mp4 << std::endl;
        rename(output,mp4);
    }
    std::string get_filename(std::string &path,std::string ext) {
        std::filesystem::path filePath(path);
        return filePath.parent_path().filename().string() + ext;
    }
    void combine_video(std::vector<std::string> files,std::string export_path,std::string ext) {
        for (std::string file : files) {
            std::string file_output_mov = export_path + "/" + get_filename(file,ext);
            HaruFFMpegMergeVideoCMD cmd(file,file_output_mov);
            std::cout << "combine_video=>" << cmd.get_command() << std::endl;
            std::system(cmd.get_command().c_str());
            // std::string file_output_mp4 = export_path + "/" + get_filename(file,".mp4");
            // HaruFFMpegMovToMp4CMD cmd1(file_output_mov,file_output_mp4);
            // std::cout << "combine_video=>" << cmd1.get_command() << std::endl;
            // std::system(cmd1.get_command().c_str());
        }
    }
    void convert_mov_mp4(std::string mov,std::string mp4) {
        HaruFFMpegMovToMp4CMD cmd(mov,mp4);
        std::cout << "convert_mov_mp4=>" << cmd.get_command() << std::endl;
        std::system(cmd.get_command().c_str());
    }
    void convert_mts_mp4(std::string &mov,std::string &mp4) {
        HaruFFMpegMtsToMp4CMD cmd(mov,mp4);
        std::cout << "convert_mts_mp4=>" << cmd.get_command() << std::endl;
        std::system(cmd.get_command().c_str());
    }
    std::string get_mp3_filename(std::string &&name) {
        // Finding a substring
        size_t index = name.find(".mp3");
        if (index != std::string::npos) {
            std::cout << "Found at index: " << index << std::endl;
            return name.substr(0, index);
        } else {
            std::cout << "Not found" << std::endl;
            return name;
        }
    }
    std::string getOutput_filename(std::string &workspace_path,std::vector<std::string> &files) {
        std::stringstream ss;
        for (int i = 0; i < files.size(); i++) {
            std::filesystem::path filePath(files[i]);
            if (i == 0) {
                ss << workspace_path << "/merged-mp3-";
            }
            if (i < files.size() - 1) {
                ss << get_mp3_filename(filePath.filename()) << "-";
            } else {
                ss << get_mp3_filename(filePath.filename()) << ".mp3";
            }
        }
        return ss.str();
    };
    std::string concatenate_mp3(std::string &mp3_path,std::vector<std::string> &mp3_files) {
        std::filesystem::path mpath(mp3_path);
        if (!exists(mpath)) {
            std::filesystem::create_directory(mpath);
        }
        std::string output = getOutput_filename(mp3_path,mp3_files);
        std::cout << "merge_mp3::output => " << output << std::endl;
        std::ofstream out(output, std::ios::binary);
        if (out) {
            for (const auto& file : mp3_files) {
                std::ifstream in(file, std::ios::binary);
                if (in) {
                    out << in.rdbuf(); // Copy first file
                }
            }
        }
        out.close();
        return output;
    }

}
