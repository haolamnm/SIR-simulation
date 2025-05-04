#ifndef POPULATION_H
#define POPULATION_H

#include <memory>
#include <queue>
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
    Disease disease;                                           ///< Disease parameters
    int size = 1;                                              ///< Grid size (size x size)
    int travel_radius = 1;                                     ///< Maximum encounter distance
    int encounters = 1;                                        ///< Number of encounters per person
    std::vector<std::vector<std::unique_ptr<Person>>> people;  ///< 2D grid of Persons
    std::vector<int> status_count = std::vector<int>(5, 0);    ///< Counts of each Status
    std::string name = "";                                     ///< Name of the population

   public:
    Population() = default;
    Population(int size, int travel_radius, int encounters, int init_incubations,
               int init_infections, const Disease &disease, const std::string &name = "");

    void update();

    std::vector<std::vector<int>> get_people() const;
    const std::vector<int> &get_status_count() const;
    int get_size() const;
    int get_travel_radius() const;
    int get_encounters() const;
    std::string get_name() const;

    void set_travel_radius(int radius);
    void set_encounters(int encounters);
    void set_name(const std::string &name);

   private:
    void validate() const;

    std::vector<Person *> flatten() const;
    std::vector<Person *> sample(std::vector<Person *> people, int count) const;

    std::vector<Person *> get_encountered(int row, int col) const;
    std::queue<Person *> get_infectious() const;
    bool interact(Person *current, Person *other);
};

#endif
