#include "population.h"

#include <cmath>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "disease.h"
#include "person.h"

Population::Population(int size, int travel_radius, int encounters, int init_incubations,
                       int init_infections, std::shared_ptr<Disease> disease, unsigned int seed,
                       const std::string &name)
    : size(size),
      travel_radius(travel_radius),
      encounters(encounters),
      disease(std::move(disease)),
      name(name),
      init_incubations(init_incubations),
      init_infections(init_infections) {
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
    this->seed = (seed == 0) ? rd() : seed;
    rng.seed(this->seed);

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

    // Update status count and current infectious people
    // NOTE: At peak infectious people counts will be equal to the population size
    infectious_people.reserve(size * size);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            Person *person = people[i][j].get();
            if (person == nullptr) continue;

            Status status = person->get_status();
            status_count[static_cast<int>(status)] += 1;

            if (person->is_infectious()) {
                infectious_people.push_back(person);
            }
        }
    }
}

void Population::update() {
    // NOTE: Stop early if the population is already stable
    if (infectious_people.empty()) return;

    std::vector<int> new_status_count(status_count.size(), 0);
    std::vector<Person *> new_infectious_people;

    // Phase 1: Update statuses and collect new infectious people
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            Person *person = people[i][j].get();
            if (person == nullptr) continue;
            person->update(disease.get(), rng);
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

    // Phase 3: Collect the new infectious people and update new status counts
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            Person *person = people[i][j].get();
            if (person->is_infectious()) {
                new_infectious_people.push_back(person);
            }
            Status status = person->get_status();
            new_status_count[static_cast<int>(status)] += 1;
        }
    }

    infectious_people = new_infectious_people;
    status_count = new_status_count;
}

void Population::reset(bool same_seed) {
    // Reset RNG for deterministically behavior
    if (same_seed) {
        rng.seed(this->seed);
    }

    // Reset people grid
    people.clear();
    people.resize(size);
    for (int i = 0; i < size; ++i) {
        people[i].resize(size);
        for (int j = 0; j < size; ++j) {
            people[i][j] = std::make_unique<Person>(i, j);
        }
    }

    // Recompute neighbors
    // NOTE: Clear people left the neighbor pointers become dangling
    neighbors.clear();
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

    // Reset tracking information
    status_count.clear();
    status_count.resize(5, 0);
    infectious_people.clear();

    // Apply initial statuses
    // NOTE: Recreate initial state by calling sample to achieve the same RNG state
    std::vector<Person *> candidates = flatten();
    std::vector<Person *> incubations = sample(candidates, init_incubations);
    std::vector<Person *> infections = sample(incubations, init_infections);

    // Apply statuses to selected people
    // NOTE: Missing "this" has caused a severe bug in the constructor
    for (auto person : incubations) {
        person->incubate(disease->get_days_in_incubation());
    }
    for (auto person : infections) {
        person->infect(disease->get_days_with_symptoms());
    }

    // Update status count and current infectious people
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            Person *person = people[i][j].get();
            if (person == nullptr) continue;

            Status status = person->get_status();
            status_count[static_cast<int>(status)] += 1;

            if (person->is_infectious()) {
                infectious_people.push_back(person);
            }
        }
    }
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

unsigned int Population::get_seed() const {
    return seed;
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

void Population::set_seed(unsigned int seed) {
    this->seed = seed;
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
}

bool Population::interact(Person *current, Person *other) {
    // NOTE: If other person is already infectious, the
    // current person cannot transfer the disease
    if (current == nullptr || other == nullptr || !current->is_infectious() ||
        !other->is_susceptible()) {
        return false;
    }
    // If the other person is not infectious, try to infect by transmission rate
    // NOTE: Actually only when other is Susceptile
    double transmission_rate = std::pow(disease->get_transmission_rate(), 3.0);
    if (get_chance(rng) < transmission_rate) {
        other->incubate(disease->get_days_in_incubation());
        return true;
    }
    return false;
}

double Population::get_chance(std::mt19937 &rng) const {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}
