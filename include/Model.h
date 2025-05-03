#ifndef MODEL_H
#define MODEL_H

#include "Population.h"

class Model {
   private:
    int remain_days;
    int current_day;
    Population *population;

   public:
    Model(int days_in_simulation, Population &population);

    void simulate(int days);

    int get_current_day() const;

   private:
};

#endif
