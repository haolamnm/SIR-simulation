#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <string>
#include <vector>

#include "population.h"

/**
 * @class Model
 * @brief Represents a SIR model for simulating disease spread in a population
 * */
class Model {
   private:
    int remain_days = 0;                     ///< Remaining days until the simulation finishes
    int current_day = 1;                     ///< Current day of the simulation
    std::shared_ptr<Population> population;  ///< Shared pointer to population being simulated
    std::string name = "";                   ///< Name of the model

    std::vector<std::vector<std::vector<int>>>
        data;                             ///< 3D vector of population states for each day
    std::vector<std::vector<int>> stats;  ///< 2D vector of status counts for each day

   public:
    Model(int days_in_simulation, std::shared_ptr<Population> population,
          const std::string &name = "");

    bool simulate(int days);

    const std::vector<std::vector<std::vector<int>>> &get_data() const;
    const std::vector<std::vector<int>> &get_stats() const;
    std::shared_ptr<Population> get_population() const;
    int get_remain_days() const;
    int get_current_day() const;
    std::string get_name() const;

    void set_name(const std::string &name);

   private:
};

void print_progress_bar(int progress, int total, int bar_width = 50);

#endif
