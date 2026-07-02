#include <string>
#include "modules/haru_yaml.h"
#include "modules/file_utils.h"
#include "modules/haru_ffmpeg.h"
#include "modules/haru_httpserver.h"

using namespace haru;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " config_file_path" << std::endl;
        return 1;
    }
    // YamlConfig config{.yaml_path="../config/application.yaml"};
    YamlConfig config{.yaml_path=argv[1]};
    config.load_yaml();
    SimpleLogger &logger = SimpleLogger::getInstance();
    logger.configure("../log/log.txt",LogLevel::INFO);
    logger.debug("debug");
    logger.info("info");
    logger.warning("warning");
    logger.error("error");

    webMain(config);
}