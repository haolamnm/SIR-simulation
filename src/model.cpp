#include "model.h"

#include <memory>
#include <stdexcept>
#include <string>

#include "disease.h"
#include "population.h"

Model::Model(int days_in_simulation, std::shared_ptr<Population> population,
             const std::string &name)
    : remain_days(days_in_simulation), population(std::move(population)), name(name) {
    if (days_in_simulation < 0) {
        throw std::invalid_argument("Days in simulation must be non-negative");
    }
    // Reserve all the necessary memory
    data.reserve(days_in_simulation);
    stats.reserve(days_in_simulation);
}

Model::Model(int days_in_simulation, int size, int travel_radius, int encounters,
             int init_incubations, int init_infections, const Disease &disease,
             const std::string &name)
    : remain_days(days_in_simulation),
      population(std::make_shared<Population>(size, travel_radius, encounters, init_incubations,
                                              init_infections, disease)),
      name(name) {
    if (days_in_simulation < 0) {
        throw std::invalid_argument("Days in simulation must be non-negative");
    }
    // Reserve all the necessary memory
    data.reserve(days_in_simulation);
    stats.reserve(days_in_simulation);
}

bool Model::simulate(int days = -1) {
    if (days < -1) {
        throw std::invalid_argument("Days must be non-negative or -1");
    }

    // Determine the number of days to run the simulation
    days = (days == -1) ? remain_days : std::min(days, remain_days);
    if (days <= 0) return false;

    for (int d = 1; d <= days; ++d) {
        population->update();
        data.push_back(population->get_people());
        stats.push_back(population->get_status_count());
    }
    remain_days -= days;
    current_day += days;

    return true;
}

const std::vector<std::vector<std::vector<int>>> &Model::get_data() const {
    return data;
}

const std::vector<std::vector<int>> &Model::get_stats() const {
    return stats;
}

std::shared_ptr<Population> Model::get_population() const {
    return population;
}

int Model::get_current_day() const {
    return current_day;
}

int Model::get_remain_days() const {
    return remain_days;
}

std::string Model::get_name() const {
    return name;
}

void Model::set_name(const std::string &name) {
    this->name = name;
}
