#include "utils.h"

int Index(int i){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, i-1);
    return dis(gen);
}
