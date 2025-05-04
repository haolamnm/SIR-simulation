#include "utils.h"

#include <cstdlib>

double get_chance() {
    return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}
