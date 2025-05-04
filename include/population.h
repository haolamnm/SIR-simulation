#ifndef POPULATION_H
#define POPULATION_H

#include <queue>
#include <vector>

#include "Disease.h"
#include "Person.h"

class Population {
   private:
    const Disease *disease;
    int size, travel_radius, encounters;
    std::vector<std::vector<Person *>> people;
    std::vector<int> status_count;

   public:
    Population(int size, int travel_radius, int encounters, int init_incubations,
               int init_infections, const Disease &disease);

    void update();
    void view() const;
    void stats() const;

   private:
    std::vector<Person *> flatten() const;
    std::vector<Person *> sample(std::vector<Person *> people, int count) const;

    std::vector<Person *> get_encountered(int row, int col) const;
    std::queue<Person *> get_infectious() const;
    bool interact(Person *current, Person *other);
};

#endif
