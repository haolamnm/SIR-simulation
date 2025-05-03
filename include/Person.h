#ifndef PERSON_H
#define PERSON_H

#include "Disease.h"

const int STATUS = 5;

enum class Status {
    Susceptible = 0,
    Incubated,
    Infected,
    Recovered,
    Dead,
};

class Person {
   private:
    Status status;
    int remain_incubated_days;
    int remain_infected_days;

   public:
    Person();

    bool incubate(int days_in_incubation);
    bool infect(int days_with_symptoms);
    bool recover();
    bool die();
    void update(const Disease *disease);

    bool is_infectious() const;
    int get_status() const;
    char get_symbol() const;

   private:
};

#endif
