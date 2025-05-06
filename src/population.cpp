#include "population.h"

#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "disease.h"
#include "person.h"

Population::Population(int size, int travel_radius, int encounters, int init_incubations,
                       int init_infections, std::shared_ptr<Disease> disease,
                       const std::string &name)
    : size(size),
      travel_radius(travel_radius),
      encounters(encounters),
      disease(std::move(disease)),
      name(name) {
    // Validate arguments
    if (this->disease.get() == nullptr) {
        throw std::invalid_argument("Disase shared pointer cannot be null");
    }
    if (init_incubations < 0 || init_infections < 0) {
        throw std::invalid_argument("Initial incubations and infections must be non-negative");
    }
    if (init_incubations < init_infections) {
        throw std::invalid_argument(
            "Initial incubation must be greater than or equal to initial infections");
    }
    if (init_incubations + init_infections > size * size) {
        throw std::invalid_argument("Initial incubations and infections exceed population size");
    }
    validate();

    // Initalize the random number generator
    std::random_device rd;
    rng.seed(rd());

    // Initialize status counts
    status_count.resize(5, 0);

    // Initialize grid of people
    people.resize(size);
    for (int i = 0; i < size; ++i) {
        people[i].resize(size);
        for (int j = 0; j < size; ++j) {
            people[i][j] = std::make_unique<Person>(i, j);
        }
    }

    // Precompute neighbors for each cell (i, j)
    neighbors.resize(size, std::vector<std::vector<Person *>>(size));
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            for (int di = -travel_radius; di <= travel_radius; ++di) {
                for (int dj = -travel_radius; dj <= travel_radius; ++dj) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < size && nj >= 0 && nj < size && !(di == 0 && dj == 0)) {
                        neighbors[i][j].push_back(people[ni][nj].get());
                    }
                }
            }
        }
    }

    // Initialize some Incubations and Infections at start
    std::vector<Person *> candidates = flatten();
    std::vector<Person *> incubations = sample(candidates, init_incubations);
    std::vector<Person *> infections = sample(incubations, init_infections);

    // NOTE: Missing "this" has caused a severe bug here!
    for (auto person : incubations) {
        person->incubate(this->disease->get_days_in_incubation());
    }
    for (auto person : infections) {
        person->infect(this->disease->get_days_with_symptoms());
    }

    // Update status counts
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            Person *person = people[i][j].get();
            if (person == nullptr) continue;
            Status status = person->get_status();
            status_count[static_cast<int>(status)] += 1;
        }
    }

    // Initialize infectious people in class member
    int incubated_count = status_count[static_cast<int>(Status::Incubated)];
    int infected_count = status_count[static_cast<int>(Status::Infected)];
    infectious_people.reserve(incubated_count + infected_count);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            Person *person = people[i][j].get();
            if (person != nullptr && person->is_infectious()) {
                infectious_people.push_back(person);
            }
        }
    }
}

void Population::update() {
    std::vector<int> new_status_count(status_count.size(), 0);
    std::vector<Person *> new_infectious_people;

    // Phase 1: Update statuses and collect new infectious people
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            Person *person = people[i][j].get();
            if (person == nullptr) continue;

            person->update(disease.get(), rng);
            if (person->is_infectious()) {
                new_infectious_people.push_back(person);
            }
            Status status = person->get_status();
            new_status_count[static_cast<int>(status)] += 1;
        }
    }

    // Phase 2: Process interactions for previous infectious people
    for (Person *person : infectious_people) {
        // NOTE: Add this to reduce the spread of the disease for lower transmission rate
        // if (get_chance() > disease->get_transmission_rate()) {
        //     continue;
        // }
        std::pair<int, int> pos = person->get_position();
        int i = pos.first, j = pos.second;

        std::vector<Person *> targets = get_encountered(i, j);
        for (Person *neighbor : targets) {
            interact(person, neighbor);
        }
    }

    infectious_people = new_infectious_people;
    status_count = new_status_count;

    // OLD VERSION: BOTTLE NECK PERFORMANCE
    // // Get previous infectious people
    // std::queue<Person *> infectious_people = get_infectious();

    // // For a time step
    // // For each person in population
    // for (int i = 0; i < size; ++i) {
    //     for (int j = 0; j < size; ++j) {
    //         Person *person = people[i][j].get();
    //         if (person == nullptr) continue;

    //         // If current person is infectious in previous time step
    //         // Get encountered person, let them interact each other
    //         // NOTE: Avoiding the situation where a newly infected person
    //         // infects other people in the loop creating a chain of unwanted
    //         // infections
    //         if (person == infectious_people.front()) {
    //             infectious_people.pop();
    //             for (Person *encounered : get_encountered(i, j)) {
    //                 interact(person, encounered);
    //             }
    //         }
    //         // Internally update each person
    //         person->update(&disease);
    //         Status status = person->get_status();

    //         // Update new status statistics
    //         new_status_count[static_cast<int>(status)] += 1;
    //     }
    // }
    // status_count = new_status_count;
}

std::vector<std::vector<int>> Population::get_people() const {
    std::vector<std::vector<int>> grid;
    grid.resize(size, std::vector<int>(size, 0));

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (people[i][j].get() == nullptr) continue;
            Status status = people[i][j]->get_status();
            grid[i][j] = static_cast<int>(status);
        }
    }
    return grid;
}

const std::vector<int> &Population::get_status_count() const {
    return status_count;
}

int Population::get_size() const {
    return size;
}

int Population::get_travel_radius() const {
    return travel_radius;
}

int Population::get_encounters() const {
    return encounters;
}

std::string Population::get_name() const {
    return name;
}

void Population::set_travel_radius(int radius) {
    travel_radius = radius;
    validate();
}

void Population::set_encounters(int encounters) {
    this->encounters = encounters;
    validate();
}

void Population::set_name(const std::string &name) {
    this->name = name;
}

void Population::validate() const {
    if (size <= 0) {
        throw std::invalid_argument("Size must be positive");
    }
    if (travel_radius < 0) {
        throw std::invalid_argument("Travel radius must be non-negative");
    }
    if (encounters < 0) {
        throw std::invalid_argument("Encounters must be non-negative");
    }
    if (status_count.size() != 5) {
        throw std::invalid_argument("Status count must have 5 elements");
    }
}

std::vector<Person *> Population::flatten() const {
    std::vector<Person *> flat;
    flat.reserve(size * size);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            Person *person = people[i][j].get();
            if (person == nullptr) continue;
            flat.push_back(person);
        }
    }
    return flat;
}

std::vector<Person *> Population::sample(std::vector<Person *> people, int count) const {
    std::vector<Person *> result;
    result.reserve(count);

    // NOTE: Should check for "count" arg
    if (people.empty() || count <= 0) return result;

    std::uniform_int_distribution<> dist(0, people.size() - 1);

    // NOTE: Same person could appear multiple times
    for (int i = 0; i < count; ++i) {
        int index = dist(rng);
        Person *person = people[index];
        if (person == nullptr) continue;
        result.push_back(person);
    }
    return result;
}

std::vector<Person *> Population::get_encountered(int row, int col) const {
    if (neighbors[row][col].empty()) {
        return {};
    }
    return sample(neighbors[row][col], encounters);

    // OLD VERSION
    // // Calculate indices of the neighbors
    // // NOTE: Can use modulo to wrap around the other side
    // int start_row = std::max(row - travel_radius, 0);
    // int start_col = std::max(col - travel_radius, 0);

    // int end_row = std::min(row + travel_radius, size - 1);
    // int end_col = std::min(col + travel_radius, size - 1);

    // // Gather the neighbors
    // int neighbors_size = (end_row - start_row + 1) * (end_col - start_col + 1);

    // std::vector<Person *> neighbors;
    // neighbors.reserve(neighbors_size);

    // for (int i = start_row; i <= end_row; ++i) {
    //     for (int j = start_col; j <= end_col; ++j) {
    //         if (i == row && j == col) continue;
    //         neighbors.push_back(people[i][j].get());
    //     }
    // }
    // // Sample encountered neighbors
    // return sample(neighbors, encounters);
}

// std::queue<Person *> Population::get_infectious() const {
//     std::queue<Person *> infections;

//     for (int i = 0; i < size; ++i) {
//         for (int j = 0; j < size; ++j) {
//             Person *person = people[i][j].get();
//             if (person == nullptr) continue;

//             if (person->is_infectious()) {
//                 infections.push(person);
//             }
//         }
//     }
//     return infections;
// }

bool Population::interact(Person *current, Person *other) {
    // NOTE: If other person is already infectious, the
    // current person cannot transfer the disease
    if (current == nullptr || other == nullptr || !current->is_infectious() ||
        !other->is_susceptible()) {
        return false;
    }
    // If the other person is not infectious, try to infect by transmission rate
    // NOTE: Actually only when other is Susceptile
    if (get_chance() < disease->get_transmission_rate()) {
        other->incubate(disease->get_days_in_incubation());
        return true;
    }
    return false;
}

double Population::get_chance() const {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}
