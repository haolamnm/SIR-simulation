#include "Population.h"

#include <iostream>
#include <queue>
#include <random>
#include <vector>

#include "Disease.h"
#include "Person.h"
#include "Utils.h"

Population::Population(int size, int travel_radius, int encounters, int init_incubations,
                       int init_infections, const Disease &disease) {
    // Init population attributes
    this->size = size;
    this->travel_radius = travel_radius;
    this->encounters = encounters;
    this->disease = &disease;
    status_count.resize(STATUS, 0);

    // Init a grid of people
    people.resize(size);
    for (int row = 0; row < size; ++row) {
        people[row].resize(size, nullptr);
        for (int col = 0; col < size; ++col) {
            people[row][col] = new Person();
        }
    }

    // Init some Incubations and Infections
    std::vector<Person *> flattened = flatten();
    std::vector<Person *> incubations = sample(flattened, init_incubations);
    std::vector<Person *> infections = sample(incubations, init_infections);

    for (Person *person : incubations) {
        person->incubate(disease.get_days_in_incubation());
    }
    for (Person *person : infections) {
        person->infect(disease.get_days_with_symptoms());
    }

    // Update status statistics
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            Person *person = people[row][col];

            status_count[person->get_status()] += 1;
        }
    }
}

void Population::update() {
    std::vector<int> new_status_count(STATUS, 0);

    // Get previous infectious people
    std::queue<Person *> infectious_people = get_infectious();
    int index = 0;

    // For a time step
    // For each person in population
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            Person *person = people[row][col];

            // If current person is infectious in previous time step
            // Get encountered person, let them interact each other
            // NOTE: Avoiding the situation where a newly infected person
            // infects other people in the loop creating a chain of unwanted
            // infections
            if (person == infectious_people.front()) {
                infectious_people.pop();
                for (Person *encounered : get_encountered(row, col)) {
                    interact(person, encounered);
                }
            }
            // Internally update each person
            person->update(disease);

            // Update new status statistics
            new_status_count[person->get_status()] += 1;
        }
    }
    status_count = new_status_count;
}

void Population::view() const {
    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            std::cout << people[row][col]->get_symbol() << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n' << std::flush;
}

void Population::stats() const {
    int total = 0;
    for (int s = 0; s < STATUS; ++s) {
        total += status_count[s];
        std::cout << status_count[s] << ' ';
    }
    std::cout << total << '\n' << std::flush;
}

std::vector<Person *> Population::flatten() const {
    std::vector<Person *> result;
    for (const std::vector<Person *> &row : people) {
        result.insert(result.end(), row.begin(), row.end());
    }
    return result;
}

std::vector<Person *> Population::sample(std::vector<Person *> people, int count) const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, people.size() - 1);

    std::vector<Person *> result;
    result.reserve(count);

    for (int i = 0; i < count; ++i) {
        int index = dist(gen);
        result.push_back(people[index]);
    }
    return result;
}

std::vector<Person *> Population::get_encountered(int row, int col) const {
    // Calculate indices of the neighbors
    // NOTE: Can use modulo to wrap around the other side
    int start_row = std::max(row - travel_radius, 0);
    int start_col = std::max(col - travel_radius, 0);

    int end_row = std::min(row + travel_radius, size - 1);
    int end_col = std::min(col + travel_radius, size - 1);

    // Gather the neighbors
    int neighbors_size = (end_row - start_row + 1) * (end_col - start_col + 1);

    std::vector<Person *> neighbors;
    neighbors.reserve(neighbors_size);

    for (int r = start_row; r <= end_row; ++r) {
        for (int c = start_col; c <= end_col; ++c) {
            if (r == row && c == col) continue;
            neighbors.push_back(people[r][c]);
        }
    }
    // Sample encountered neighbors
    return sample(neighbors, encounters);
}

std::queue<Person *> Population::get_infectious() const {
    std::queue<Person *> result;

    for (int row = 0; row < size; ++row) {
        for (int col = 0; col < size; ++col) {
            if (people[row][col]->is_infectious()) {
                result.push(people[row][col]);
            }
        }
    }
    return result;
}

bool Population::interact(Person *current, Person *other) {
    // NOTE: If other person is already infectious, the
    // current person cannot transfer the disease
    if (other->is_infectious()) {
        return false;
    }
    // If the other person is not infectious, try to infect by transmission rate
    if (get_chance() < disease->get_transmission_rate()) {
        other->incubate(disease->get_days_in_incubation());
        return true;
    }
    return false;
}
