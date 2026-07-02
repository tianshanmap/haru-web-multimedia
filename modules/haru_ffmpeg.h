//
// Created by developer on 2026-05-25.
//
#include <iostream>
#include <fstream>
#ifndef HARU_OPENCV_WEB_HARU_FFMPEG_H
#define HARU_OPENCV_WEB_HARU_FFMPEG_H
namespace haru {
    struct HaruMp3Files {
        std::string mp3_path;
        std::vector<std::string> files;
        int index = 0;
        std::vector<std::string> select_mp3() {
            std::vector<std::string> vec;
            if (index < files.size() - 3) {
                vec = std::vector<std::string>{files[index],files[index+1],files[index+2]};
                index = index + 3;
            } else {
                if (index == files.size() - 3) {
                    vec = std::vector<std::string>{files[index],files[index+1],files[index+2]};
                    index = 0;
                } else if (index == files.size() - 2) {
                    vec = std::vector<std::string>{files[index],files[index+1],files[0]};
                    index = 1;
                } else if (index == files.size() - 1) {
                    vec = std::vector<std::string>{files[index],files[0],files[1]};
                    index = 2;
                } else {
                    vec = std::vector<std::string>{files[0],files[1],files[2]};
                    index = 3;
                }
            }
            return vec;
        };
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
        std::string getOutput(std::vector<std::string> files) {
            std::stringstream ss;
            for (int i = 0; i < files.size(); i++) {
                std::filesystem::path filePath(files[i]);
                if (i == 0) {
                    ss << mp3_path << "/merged-mp3-";
                }
                if (i < files.size() - 1) {
                    ss << get_mp3_filename(filePath.filename()) << "-";
                } else {
                    ss << get_mp3_filename(filePath.filename()) << ".mp3";
                }
            }
            return ss.str();
        };
        //function returns merged mp3 filename
        std::string merge_mp3(std::string &mp3_path,std::vector<std::string> &mp3_files) {
            std::filesystem::path mpath(mp3_path);
            if (!exists(mpath)) {
                std::filesystem::create_directory(mpath);
            }
            std::string output = getOutput(mp3_files);
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
        std::string merge_mp3() {
            std::vector<std::string> mp3_files = select_mp3();
            return merge_mp3(mp3_path,mp3_files);
        }
    };
    struct HaruFFMpegMergeCMD {
        std::string mp3;
        std::string mp4;
        std::string output;
        HaruFFMpegMergeCMD(std::string mp3,std::string mp4,std::string output) {
            this->mp3 = mp3;
            this->mp4 = mp4;
            this->output = output;
        }
        /*
        *ffmpeg -i video1.mp4 -stream_loop -1 -i audio2.mp3 -c:v copy -shortest \
    -map 0:v -map 1:a \
    -y output.mp4
         */
        std::string get_command() {
            std::string command = "ffmpeg -i " + mp4 + " -stream_loop -1 -i " + mp3 + " -c:v copy -shortest -map 0:v -map 1:a -y " + output;
            return command;
        }
    };

    struct HaruFFMpegMergeVideoCMD {
        std::string input_file;
        std::string output_file;
        HaruFFMpegMergeVideoCMD(std::string &input,std::string &output) {
            this->input_file = input;
            this->output_file = output;
        }
        std::string get_command() {
            std::string command = "ffmpeg -f concat -safe 0 -i " + input_file + "/mov.txt -c copy " + output_file;
            return command;
        }
    };
    struct HaruFFMpegMovToMp4CMD {
        std::string input_file;
        std::string output_file;
        HaruFFMpegMovToMp4CMD(std::string &input,std::string &output) {
            this->input_file = input;
            this->output_file = output;
        }
        std::string get_command() {
            std::string command = "ffmpeg -i " + input_file + " -c copy -movflags +faststart " + output_file;
            return command;
        }
    };
    struct HaruFFMpegMtsToMp4CMD {
        std::string input_file;
        std::string output_file;
        HaruFFMpegMtsToMp4CMD(std::string &input,std::string &output) {
            this->input_file = input;
            this->output_file = output;
        }
        std::string get_command() {
            std::string command = "ffmpeg -i " + input_file + " -c:v libx264 -crf 23 -c:a aac -b:a 128k " + output_file;
            return command;
        }
    };
    // bool concatMP3(const std::string& file1,const std::string& file2,const std::string& output);
    // void concatenateMP3(const std::string& file1, const std::string& file2, const std::string& output);
    std::string concatenate_mp3(std::string &mp3_workspace,std::vector<std::string> &mp3_files);
    void add_mp3_to_mp4(std::string mp3,std::string mp4,std::string output);
    void add_mp3_to_mp4(std::string mp3,std::string mp4);
    void combine_video(std::vector<std::string> files,std::string output_path,std::string ext);
    void convert_mov_mp4(std::string mov,std::string mp4);
    void convert_mts_mp4(std::string &mov,std::string &mp4);
}
#endif //HARU_OPENCV_WEB_HARU_FFMPEG_H
