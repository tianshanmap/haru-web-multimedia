//
// Created by developer on 2026-05-22.
//

#ifndef HARU_OPENCV_WEB_HARU_RANDOM_H
#define HARU_OPENCV_WEB_HARU_RANDOM_H
#include <random>
using namespace std;
namespace haru {
    class HaruRandom {
    private:
        int min;
        int max;
        mt19937 gen;
        random_device rd;
        uniform_int_distribution<> distrib;
    public:
        HaruRandom(int min,int max) {
            this->min = min;
            this->max = max;
            gen = mt19937(rd());
            distrib = uniform_int_distribution<>(min,max);
        }
        int getRandom() {
            return distrib(gen);
        }
    };
}
#endif //HARU_OPENCV_WEB_HARU_RANDOM_H
