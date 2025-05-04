#include "disease.h"

#include <stdexcept>

Disease::Disease(double transmission_rate, double fatality_rate, int days_in_incubation,
                 int days_with_symptoms, const std::string &name)
    : transmission_rate(transmission_rate),
      fatality_rate(fatality_rate),
      days_in_incubation(days_in_incubation),
      days_with_symptoms(days_with_symptoms),
      name(name) {
    validate();
}

double Disease::get_transmission_rate() const {
    return transmission_rate;
}

double Disease::get_fatality_rate() const {
    return fatality_rate;
}

int Disease::get_days_in_incubation() const {
    return days_in_incubation;
}

int Disease::get_days_with_symptoms() const {
    return days_with_symptoms;
}

std::string Disease::get_name() const {
    return name;
}

void Disease::set_transmission_rate(double rate) {
    transmission_rate = rate;
    validate();
}

void Disease::set_fatality_rate(double rate) {
    fatality_rate = rate;
    validate();
}

void Disease::set_days_in_incubation(int days) {
    days_in_incubation = days;
    validate();
}

void Disease::set_days_with_symptoms(int days) {
    days_with_symptoms = days;
    validate();
}

void Disease::set_name(const std::string &name) {
    this->name = name;
}

void Disease::validate() const {
    if (transmission_rate < 0 || transmission_rate > 1) {
        throw std::invalid_argument("Transmission rate must be between 0 and 1");
    }
    if (fatality_rate < 0 || fatality_rate > 1) {
        throw std::invalid_argument("Fatality rate must be between 0 and 1");
    }
    if (days_in_incubation < 0) {
        throw std::invalid_argument("Days in incubation must be non-negative");
    }
    if (days_with_symptoms < 0) {
        throw std::invalid_argument("Days with symptoms must be non-negative");
    }
}
