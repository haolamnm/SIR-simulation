#ifndef PERSON_H
#define PERSON_H

#include <random>

#include "disease.h"

/**
 * @brief Enum class representing the disease status of a person
 * */
enum class Status {
    Susceptible = 0,  ///< Can contract the disease
    Incubated,        ///< Infected but asymptomatic
    Infected,         ///< Symptomatic and infectious
    Recovered,        ///< Immue after recovery
    Dead,             ///< Dead
};

/**
 * @class Person
 * @brief Represents a person with a disease status and progression state
 * */
class Person {
   private:
    Status status = Status::Susceptible;  ///< Current disease status
    int remain_incubated_days = -1;       ///< Remaining days in incubation period
    int remain_infected_days = -1;        ///< Remaining days with symptoms
    int i;                                ///< The y coordinate in grid
    int j;                                ///< The x coordinate in grid

   public:
    Person(int i, int j);

    bool incubate(int days_in_incubation);
    bool infect(int days_with_symptoms);
    bool recover();
    bool die();
    void update(const Disease *disease, std::mt19937 &rng);

    bool is_susceptible() const;
    bool is_infectious() const;
    bool is_removed() const;

    Status get_status() const;
    char get_symbol() const;
    std::pair<int, int> get_position() const;

   private:
    double get_chance(std::mt19937 &rng) const;
};

#endif
