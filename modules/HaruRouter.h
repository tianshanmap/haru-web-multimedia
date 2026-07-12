//
// Created by developer on 2026-07-11.
//

#ifndef WEB_MULTIMEDIA_HARUROUTER_H
#define WEB_MULTIMEDIA_HARUROUTER_H
#include "haru_httpserver.h"
#include "haru_yaml.h"


namespace haru {
    class HaruRouter {
    private:
        HaruHttpServer &srv;
    public:
        HaruRouter(HaruHttpServer &server):srv(server) {}
        void register_mapping(YamlConfig &config);
    };

}



#endif //WEB_MULTIMEDIA_HARUROUTER_H
