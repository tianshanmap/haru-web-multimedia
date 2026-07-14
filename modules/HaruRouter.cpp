//
// Created by developer on 2026-07-11.
//

#include "HaruRouter.h"
#include "opencv_utils.h"
#include "../include/httplib.hpp"
#include "file_utils.h"
#include "haru_http_handlers.h"
#include "haru_service.h"

namespace haru {
    inline void handle_frame(cv::Mat *frame,httplib::Response& res) {
        std::vector<uchar> image_data;
        convertImage(*frame,image_data);
        delete frame;
        std::string s(image_data.begin(), image_data.end());
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(s, "image/jpeg");
    };
    void HaruRouter::register_mapping(YamlConfig &config) {
        std::cout << "***registering mapping***" << std::endl;
        this->srv.Get("/transform/rotation", [](const httplib::Request& req, httplib::Response &res)
        {
            auto id = req.get_param_value("id");
            auto angle_id = req.get_param_value("angle");
            std::cout << "id=" << id << std::endl;
            std::cout << "angle=" << angle_id << std::endl;
            auto angle = std::stod(angle_id);
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = rotation(mat,angle);
            handle_frame(rotated_mat,res);
        });
        this->srv.Get("/transform/rotation", [](const httplib::Request& req, httplib::Response &res)
                {
            auto id = req.get_param_value("id");
            auto angle_id = req.get_param_value("angle");
            std::cout << "id=" << id << std::endl;
            std::cout << "angle=" << angle_id << std::endl;
            auto angle = std::stod(angle_id);
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = rotation(mat,angle);
            handle_frame(rotated_mat,res);
        });

        this->srv.Get("/transform/rotation/save", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto angle_id = req.get_param_value("angle");
            std::cout << "id=" << id << std::endl;
            std::cout << "angle=" << angle_id << std::endl;
            auto angle = std::stod(angle_id);
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = rotation(mat,angle);
            write_image(id,*rotated_mat);
            handle_frame(rotated_mat,res);
        });

        this->srv.Get("/transform/grey", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            std::cout << "id=" << id << std::endl;
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = grey(mat);
            handle_frame(rotated_mat,res);
         });
        this->srv.Get("/transform/greyscale", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto alpha_blue = req.get_param_value("alphaBlue");
            auto alpha_green = req.get_param_value("alphaGreen");
            auto alpha_red = req.get_param_value("alphaRed");
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = greyscale(mat,std::stod(alpha_blue),std::stod(alpha_green),std::stod(alpha_red));
            handle_frame(rotated_mat,res);
         });
        this->srv.Get("/transform/grey/save", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            std::cout << "id=" << id << std::endl;
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = grey(mat);
            write_image(id,*rotated_mat);
            handle_frame(rotated_mat,res);
         });

        this->srv.Get("/transform/blur", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            std::cout << "id=" << id << std::endl;
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = blurImage(mat);
            handle_frame(rotated_mat,res);
        });
        this->srv.Get("/transform/style", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto sigmaS = req.get_param_value("sigmaS");
            auto sigmaR = req.get_param_value("sigmaR");
            std::cout << "id=" << id << std::endl;
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = styleImage(mat,std::stod(sigmaS),std::stod(sigmaR));
            handle_frame(rotated_mat,res);
        });
        this->srv.Get("/transform/gaussinblur", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto sigmaX = req.get_param_value("sigmaX");
            auto sigmaY = req.get_param_value("sigmaY");
            std::cout << "id=" << id << std::endl;
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = gaussinblurImage(mat,std::stod(sigmaX),std::stod(sigmaY));
            handle_frame(rotated_mat,res);
        });
        this->srv.Get("/transform/medianblur", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto ksize = req.get_param_value("ksize");
            std::cout << "/transform/medianblur >> id=" << id << std::endl;
            std::cout << "/transform/medianblur >> ksize=" << ksize << std::endl;
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = medianblurImage(mat,std::stoi(ksize));
            handle_frame(rotated_mat,res);
        });
        this->srv.Get("/transform/bilateral", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto d = req.get_param_value("diameter");
            auto sigmaColor = req.get_param_value("sigmaColor");
            auto sigmaSpace = req.get_param_value("sigmaSpace");
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = bilateralImage(mat,std::stoi(d),std::stoi(sigmaColor),std::stoi(sigmaSpace));
            handle_frame(rotated_mat,res);
        });
        this->srv.Get("/transform/flip", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto direction = req.get_param_value("direction");
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = flipImage(mat,std::stoi(direction));
            handle_frame(rotated_mat,res);
        });
        this->srv.Get("/transform/normalize", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto alpha = req.get_param_value("alpha");
            auto beta = req.get_param_value("beta");
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = normalizeImage(mat,std::stod(alpha),std::stod(beta));
            handle_frame(rotated_mat,res);
        });
        this->srv.Get("/transform/contrast", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto alpha = req.get_param_value("alpha");
            auto beta = req.get_param_value("beta");
            cv::Mat mat = read_image(id);
            cv::Mat *rotated_mat = contrastImage(mat,std::stod(alpha),std::stod(beta));
            handle_frame(rotated_mat,res);
        });
        this->srv.Get("/transform/sketch", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto kind = req.get_param_value("kind");
            auto sigmaS = req.get_param_value("sigmaS");
            auto sigmaR = req.get_param_value("sigmaR");
            auto shadeFactor = req.get_param_value("shadeFactor");
            cv::Mat mat = read_image(id);
            cv::Mat *grey_mat;
            cv::Mat *color_mat;
            std::tie(grey_mat,color_mat) = performSketch(mat,std::stod(sigmaS),std::stod(sigmaR),std::stod(shadeFactor));
            if (kind == "grey") {
                handle_frame(grey_mat,res);
                delete color_mat;
            } else {
                handle_frame(color_mat,res);
                delete grey_mat;
            }
        });
        this->srv.Get("/transform/detailEnhance", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto sigmaS = req.get_param_value("sigmaS");
            auto sigmaR = req.get_param_value("sigmaR");
            cv::Mat mat = read_image(id);
            cv::Mat* result = detailEnhanceImage(mat,std::stod(sigmaS),std::stod(sigmaR));
            handle_frame(result,res);
        });
        this->srv.Get("/transform/edgePreserving", [](const auto &req, auto &res)
                {
            auto id = req.get_param_value("id");
            auto sigmaS = req.get_param_value("sigmaS");
            auto sigmaR = req.get_param_value("sigmaR");
            cv::Mat mat = read_image(id);
            cv::Mat* result = edgePreservingImage(mat,std::stod(sigmaS),std::stod(sigmaR));
            handle_frame(result,res);
        });
        this->srv.Get("/filesystem/upload_target_path", [config](const auto &req, auto &res)
                {
            std::string content_type = "text/html";
            // Allow requests from any frontend origin
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(get_upload_target_path(config), content_type); });
        this->srv.Get("/filesystem/video/audio_list", [config](const auto &req, auto &res)
                {
            std::string audio_path = config.media_audio_path;
            std::string s = get_audio_as_json(audio_path);
            // Allow requests from any frontend origin
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(s, "application/json"); });

        this->srv.Get("/filesystem/video/export_list", [config](const auto &req, auto &res)
                {
            std::string audio_path = config.media_video_export;
            std::string s = get_video_as_json(audio_path);
            // Allow requests from any frontend origin
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(s, "application/json"); });

        this->srv.Post("/filesystem/video/generate/v1", [config](const auto &req, auto &res)
                {
            std::cout << "/filesystem/video/generate/v1 called" << std::endl;
            if (req.has_header("Content-Type") && req.get_header_value("Content-Type") != "application/json") {
                res.status = 400;
                res.set_content(R"({"error": "Content-Type must be application/json"})", "application/json");
                return;
            }
            std::cout << "/filesystem/video/generate/v1 creating request..." << std::endl;
            VideoCreateRequestV1 video_create_request = get_video_create_request_v1(req);
            std::cout << "video_create_request(image_path)=" << video_create_request.image_path << std::endl;
            std::cout << "video_create_request(audio_name)=" << video_create_request.audio_name << std::endl;
            std::cout << "video_create_request(video_name)=" << video_create_request.video_name << std::endl;
            std::cout << "video_export=" << config.media_video_export << std::endl;
            if (!video_create_request.image_files.empty()) {
                for (auto image_file : video_create_request.image_files) {
                    std::cout << "image_file=" << image_file << std::endl;
                }
            };
            // std::string audio_path = config.media_audio_path;
            // std::string audio_workspace_path = config.media_audio_workspace;
            // std::string mp3_name = concatenate_mp3(audio_workspace_path, video_create_request.audio_files);
            std::string video_export = config.media_video_export + "/" + video_create_request.video_name + ".mp4";
            video_create_request.video_name = video_export;
            std::string response = harusvc::create_video_v1(video_create_request);
            // std::string s = get_folder_as_json(config.media_video_export);
            // Allow requests from any frontend origin
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(response, "application/json"); });
        this->srv.Get("/video/play", [](const httplib::Request& req, httplib::Response& res) {
            // std::string file_path = "/Users/developer/Documents/book/movie/2030_trailer.mp4";
            auto file_path = req.get_param_value("id");
            // Use a shared pointer to keep the file open during streaming
            auto file = std::make_shared<std::ifstream>(file_path, std::ios::binary);
            if (!file->is_open()) {
                res.status = 404;
                return;
            }

            // Set content-type and content-length
            file->seekg(0, std::ios::end);
            auto file_size = file->tellg();
            file->seekg(0, std::ios::beg);

            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content_provider(
                file_size,
                "video/mp4",
                [file](size_t offset, size_t length, httplib::DataSink &sink) {
                    file->seekg(offset);
                    std::vector<char> buffer(length);
                    file->read(buffer.data(), length);
                    sink.write(buffer.data(), file->gcount());
                    return true;
                }
            );
        });
        this->srv.Post("/video/capture", [config](const auto &req, auto &res)
                {
            if (req.has_header("Content-Type") && req.get_header_value("Content-Type") != "application/json") {
                res.status = 400;
                res.set_content(R"({"error": "Content-Type must be application/json"})", "application/json");
                return;
            }
            VideoCaptureRequest video_capture_request = get_video_capture_request(req);
            std::cout << "payload:" << video_capture_request.payload << std::endl;
            std::cout << "filename:" << video_capture_request.filename << std::endl;
            std::string prefix = "data:image/jpeg;base64,";
            if (video_capture_request.payload.compare(0, prefix.length(), prefix) == 0) {
                video_capture_request.payload.erase(0, prefix.length());
            }
            std::vector<unsigned char> image_bytes = base64_decode(video_capture_request.payload);
            std::ofstream outfile(video_capture_request.filename, std::ios::out | std::ios::binary);
            if (outfile.is_open()) {
                outfile.write(reinterpret_cast<const char*>(image_bytes.data()), image_bytes.size());
                outfile.close();
            }
            std::string response = get_common_response();
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(response, "application/json"); });
    }
}
