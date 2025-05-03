#include "Disease.h"

Disease::Disease(float transmission_rate, float fatality_rate, int days_in_incubation,
                 int days_with_symptoms) {
    this->transmission_rate = transmission_rate;
    this->fatality_rate = fatality_rate;
    this->days_in_incubation = days_in_incubation;
    this->days_with_symptoms = days_with_symptoms;
}

float Disease::get_transmission_rate() const {
    return transmission_rate;
}

float Disease::get_fatality_rate() const {
    return fatality_rate;
}

int Disease::get_days_in_incubation() const {
    return days_in_incubation;
}

int Disease::get_days_with_symptoms() const {
    return days_with_symptoms;
}
