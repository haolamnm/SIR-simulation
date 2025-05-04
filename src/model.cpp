#include "Model.h"

Model::Model(int days_in_simulation, Population &population) {
    remain_days = days_in_simulation;
    current_day = 1;
    this->population = &population;
}

void Model::simulate(int days = -1) {
    days = std::min(days, remain_days);
    days = (days == -1) ? remain_days : days;

    for (int d = 1; d <= days; ++d) {
        population->update();
        population->stats();
        population->view();
    }
    remain_days -= days;
    current_day += days;
}

int Model::get_current_day() const {
    return current_day;
}
