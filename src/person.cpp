#include "person.h"

#include <random>
#include <stdexcept>
#include <utility>

#include "disease.h"

Person::Person(int i, int j) : i(i), j(j), status(Status::Susceptible) {}

bool Person::incubate(int days_in_incubation) {
    // Only when Status is Susceptible
    if (status != Status::Susceptible) {
        return false;
    }
    status = Status::Incubated;
    remain_incubated_days = days_in_incubation;
    remain_infected_days = -1;
    return true;
}

bool Person::infect(int days_with_symptoms) {
    // Only when Status is Incubated
    if (this->status != Status::Incubated) {
        return false;
    }
    status = Status::Infected;
    remain_incubated_days = 0;
    remain_infected_days = days_with_symptoms;
    return true;
}

bool Person::recover() {
    // Only when Status is Infected
    if (status != Status::Infected) {
        return false;
    }
    status = Status::Recovered;
    remain_incubated_days = 0;
    remain_infected_days = 0;
    return true;
}

bool Person::die() {
    // Only when Status is Infected
    if (status != Status::Infected) {
        return false;
    }
    status = Status::Dead;
    remain_incubated_days = 0;
    remain_infected_days = 0;
    return true;
}

void Person::update(const Disease *disease, std::mt19937 &rng) {
    if (disease == nullptr) {
        throw std::invalid_argument("Disease pointer cannot be null");
    }

    switch (status) {
        case Status::Susceptible:
            // NOTE: Susceptible -> Incubated only available in SISa model
            break;
        case Status::Incubated:
            if (remain_incubated_days > 0) {
                remain_incubated_days -= 1;
            }
            if (remain_incubated_days == 0) {
                infect(disease->get_days_with_symptoms());
            }
            break;
        case Status::Infected:
            if (remain_infected_days > 0) {
                remain_infected_days -= 1;
            }
            if (remain_infected_days == 0) {
                // A Person has a small chance being dead
                if (get_chance(rng) < disease->get_fatality_rate()) {
                    die();
                } else {
                    recover();
                }
            }
            break;
        case Status::Recovered:
            // NOTE: Recovered -> Susceptible only available in SIRS model
            break;
        case Status::Dead:
            break;
    }
}

bool Person::is_susceptible() const {
    return status == Status::Susceptible;
}

bool Person::is_infectious() const {
    // NOTE: Both Incubated and Infected is infectious
    return status == Status::Incubated || status == Status::Infected;
}

bool Person::is_removed() const {
    return status == Status::Recovered || status == Status::Dead;
}

Status Person::get_status() const {
    return status;
}

char Person::get_symbol() const {
    switch (status) {
        case Status::Susceptible:
            return 'S';
        case Status::Incubated:
            return '.';
        case Status::Infected:
            return '-';
        case Status::Recovered:
            return '+';
        case Status::Dead:
            return '!';
        default:
            return '?';
    }
}

std::pair<int, int> Person::get_position() const {
    return std::make_pair(i, j);
}

double Person::get_chance(std::mt19937 &rng) const {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}
