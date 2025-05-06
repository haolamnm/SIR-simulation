#ifndef POPULATION_H
#define POPULATION_H

#include <memory>
#include <random>
#include <string>
#include <vector>

#include "disease.h"
#include "person.h"

/**
 * @class Population
 * @brief Represents a population on a grid for simulating disease spread
 * */
class Population {
   private:
    int size = 1;                      ///< Grid size (size x size)
    int travel_radius = 1;             ///< Maximum encounter distance
    int encounters = 1;                ///< Number of encounters per person
    int init_incubations = 1;          ///< Initial number of incubated people for reset logic
    int init_infections = 0;           ///< Initial number of infected people for reset logic
    unsigned int seed = 0;             ///< Seed for the RNG
    std::string name = "";             ///< Name of the population
    std::shared_ptr<Disease> disease;  ///< Disease parameters
    mutable std::mt19937 rng;          ///< Random number generator

    std::vector<int> status_count = std::vector<int>(5, 0);    ///< Counts of each Status
    std::vector<Person *> infectious_people;                   ///< Keep track of infectious people
    std::vector<std::vector<std::unique_ptr<Person>>> people;  ///< 2D grid of Persons
    std::vector<std::vector<std::vector<Person *>>>
        neighbors;  ///< Precomputed neighbors for each person

   public:
    Population(int size, int travel_radius, int encounters, int init_incubations,
               int init_infections, std::shared_ptr<Disease> disease, unsigned int seed = 0,
               const std::string &name = "");

    void update();
    void reset(bool same_seed = false);

    std::vector<std::vector<int>> get_people() const;
    const std::vector<int> &get_status_count() const;
    int get_size() const;
    int get_travel_radius() const;
    int get_encounters() const;
    std::string get_name() const;
    unsigned int get_seed() const;

    void set_travel_radius(int radius);
    void set_encounters(int encounters);
    void set_name(const std::string &name);
    void set_seed(unsigned int seed);

   private:
    void validate() const;

    std::vector<Person *> flatten() const;
    std::vector<Person *> sample(std::vector<Person *> people, int count) const;

    std::vector<Person *> get_encountered(int row, int col) const;
    bool interact(Person *current, Person *other);
    double get_chance(std::mt19937 &rng) const;
};

#endif
