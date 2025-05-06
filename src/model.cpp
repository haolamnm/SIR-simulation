#include "model.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "population.h"

Model::Model(int days_in_simulation, std::shared_ptr<Population> population,
             const std::string &name)
    : remain_days(days_in_simulation),
      current_day(1),
      days_in_simulation(days_in_simulation),
      population(std::move(population)),
      name(name) {
    if (this->population.get() == nullptr) {
        throw std::invalid_argument("Population shared pointer cannot be null");
    }
    if (days_in_simulation < 0) {
        throw std::invalid_argument("Days in simulation must be non-negative");
    }
    // Reserve all the necessary memory
    data.reserve(days_in_simulation + 1);
    stats.reserve(days_in_simulation + 1);

    stats.push_back(this->population->get_status_count());
    data.push_back(this->population->get_people());
}

bool Model::simulate(int days = -1) {
    if (days < -1) {
        throw std::invalid_argument("Days must be non-negative or -1");
    }

    // Determine the number of days to run the simulation
    days = (days == -1) ? remain_days : std::min(days, remain_days);
    if (days <= 0) return false;

    // Print initial progress bar
    print_progress_bar(0, days);
    const int update_interval = std::max(1, days / 10);

    for (int d = 1; d <= days; ++d) {
        population->update();
        data.push_back(population->get_people());
        stats.push_back(population->get_status_count());

        // Update progress bar after each day
        if (d % update_interval == 0 || d == days) {
            print_progress_bar(d, days);
        }
    }
    remain_days -= days;
    current_day += days;

    // Newline after progress bar
    std::cout << std::endl;

    return true;
}

void Model::reset(bool same_seed) {
    // Reset time
    remain_days = days_in_simulation;
    current_day = 1;

    // Reset population
    population->reset(same_seed);

    // Reset internal data
    data.clear();
    data.push_back(population->get_people());

    stats.clear();
    stats.push_back(population->get_status_count());
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

void print_progress_bar(int progress, int total, int bar_width) {
    float percent = 100.0f * progress / total;
    int filled = static_cast<int>(percent * bar_width / 100.0f);
    std::string bar(filled, '#');
    bar += std::string(bar_width - filled, '-');
    std::cout << "\r[" << bar << "] " << std::fixed << std::setprecision(1) << percent << "%"
              << std::flush;
}
