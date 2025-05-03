#include "Utils.h"

#include <cstdlib>

float get_chance() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}
