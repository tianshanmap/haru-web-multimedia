//
// Created by developer on 2026-05-27.
//
#include "../include/httplib.hpp"
#include <simplelogger/simple_logger.hpp>

#ifndef HARU_OPENCV_WEB_HARU_HTTPSERVER_H
#define HARU_OPENCV_WEB_HARU_HTTPSERVER_H
namespace haru {
    int webMain(YamlConfig &config);
    class HaruHttpServer {
    private:
        httplib::Server *svr;
        std::map<std::string,std::string> mapping;
    public:
        HaruHttpServer(httplib::Server *svr) {
            this->svr = svr;
        };
        void add_mapping(std::string key,std::string value) {
            this->mapping[key] = value;
        }
        std::map<std::string,std::string> get_mappings() {
            return this->mapping;
        }
        void Get(const std::string &pattern, std::function<void(const httplib::Request &, httplib::Response &)> handler) {
            this->svr->Get(pattern, handler);
            add_mapping(pattern,"GET");
        }
        void Post(const std::string &pattern, std::function<void(const httplib::Request &, httplib::Response &)> handler) {
            this->svr->Post(pattern, handler);
            add_mapping(pattern,"POST");
        }
        void Options(const std::string &pattern, std::function<void(const httplib::Request &, httplib::Response &)> handler) {
            this->svr->Options(pattern, handler);
            add_mapping(pattern,"OPTIONS");
        }
        void set_mount_point(std::string &&path, std::string &dir) {
            this->svr->set_mount_point(path, dir);
        }
        void start(std::string &host, int port) {
            SimpleLogger &logger = SimpleLogger::getInstance();
            logger.info("List of the supported endpoint=>");
            for (auto &item : mapping) {
                logger.info("       " + item.first + " => " + item.second);
            }
            this->svr->set_logger([](const httplib::Request& req, const httplib::Response& res) {
              std::cout << req.method << " " << req.path << " -> " << res.status << std::endl;
            });
            this->svr->set_error_logger([](const httplib::Error& err, const httplib::Request* req) {
              std::cerr << httplib::to_string(err) << " while processing request";
              if (req) {
                std::cerr << ", client: " << req->get_header_value("X-Forwarded-For")
                          << ", request: '" << req->method << " " << req->path << " " << req->version << "'"
                          << ", host: " << req->get_header_value("Host");
              }
              std::cerr << std::endl;
            });
            this->svr->set_error_handler([](const auto& req, auto& res) {
              auto fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
              char buf[BUFSIZ];
              snprintf(buf, sizeof(buf), fmt, res.status);
              res.set_content(buf, "text/html");
            });
            this->svr->set_exception_handler([](const auto& req, auto& res, std::exception_ptr ep) {
              auto fmt = "<h1>Error 500</h1><p>%s</p>";
              char buf[BUFSIZ];
              try {
                std::rethrow_exception(ep);
              } catch (std::exception &e) {
                snprintf(buf, sizeof(buf), fmt, e.what());
              } catch (...) { // See the following NOTE
                snprintf(buf, sizeof(buf), fmt, "Unknown Exception");
              }
              res.set_content(buf, "text/html");
              res.status = 500;
            });
            //pre-route-handler
            this->svr->set_pre_routing_handler([](const auto& req, auto& res) {
              if (req.path == "/hello") {
                res.set_content("world", "text/html");
                return httplib::Server::HandlerResponse::Handled;
              }
              return httplib::Server::HandlerResponse::Unhandled;
            });
            //post-route-handler
            this->svr->set_post_routing_handler([](const auto& req, auto& res) {
              res.set_header("ADDITIONAL_HEADER", "value");
            });
            //Pre-request-handler
            this->svr->set_pre_request_handler([](const auto& req, auto& res) {
              if (req.matched_route == "/user/:user") {
                auto user = req.path_params.at("user");
                if (user != "john") {
                  res.status = httplib::StatusCode::Forbidden_403;
                  res.set_content("error", "text/html");
                  return httplib::Server::HandlerResponse::Handled;
                }
              }
              return httplib::Server::HandlerResponse::Unhandled;
            });
            /*
            *Max queued requests
            You can also provide an optional parameter to limit the maximum number of
            pending requests, i.e. requests accept()ed by the listener but still waiting
            to be serviced by worker threads.
             */
            this->svr->new_task_queue = [] {
                return new httplib::ThreadPool(/*base_threads=*/12, /*max_threads=*/0, /*max_queued_requests=*/18);
            };
            this->svr->listen(host, port);
        }
    };
}
#endif //HARU_OPENCV_WEB_HARU_HTTPSERVER_H
