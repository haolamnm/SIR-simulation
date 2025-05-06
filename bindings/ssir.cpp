#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <memory>

#include "disease.h"
#include "model.h"
#include "person.h"
#include "population.h"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<int>);
PYBIND11_MAKE_OPAQUE(std::vector<std::vector<int>>);
PYBIND11_MAKE_OPAQUE(std::vector<std::vector<std::vector<int>>>);

PYBIND11_MODULE(ssir, m) {
    m.doc() = "Python bindings foor SIR disease simulation model";

    // Register exception translation
    py::register_exception<std::invalid_argument>(m, "ValueError");

    // Bind nested vector type
    pybind11::bind_vector<std::vector<int>>(m, "VectorInt");
    pybind11::bind_vector<std::vector<std::vector<int>>>(m, "VectorVectorInt");
    pybind11::bind_vector<std::vector<std::vector<std::vector<int>>>>(m, "VectorVectorVectorInt");

    // Bind Status enum
    py::enum_<Status>(m, "Status", "Disease status of a person")
        .value("Susceptible", Status::Susceptible, "Can contract the disease")
        .value("Incubated", Status::Incubated, "Infected but asymptomatic")
        .value("Infected", Status::Infected, "Symptomatic and infectious")
        .value("Recovered", Status::Recovered, "Immune after recovery")
        .value("Dead", Status::Dead, "Dead")
        .export_values();

    // Bind Disease class
    py::class_<Disease, std::shared_ptr<Disease>>(
        m, "Disease", "Represents a disease with epidemiological parameters")
        .def(py::init<double, double, int, int, const std::string &>(),
             py::arg("transmission_rate"), py::arg("fatality_rate"), py::arg("days_in_incubation"),
             py::arg("days_with_symptoms"), py::arg("name") = "",
             "Initialize a Disease with the given parameters.\n"
             "Args:\n"
             "    transmission_rate (float): Probability of transmission (0 to 1).\n"
             "    fatality_rate (float): Probability of fatal outcome (0 to 1).\n"
             "    days_in_incubation (int): Number of days in incubation period.\n"
             "    days_with_symptoms (int): Number of days with symptoms.\n"
             "    name (str, optional): Name of the disease.\n"
             "Raises:\n"
             "    ValueError: If parameters are invalid.")
        .def_property("transmission_rate", &Disease::get_transmission_rate,
                      &Disease::set_transmission_rate, "Transmission rate (0 to 1).")
        .def_property("fatality_rate", &Disease::get_fatality_rate, &Disease::set_fatality_rate,
                      "Fatality rate (0 to 1).")
        .def_property("days_in_incubation", &Disease::get_days_in_incubation,
                      &Disease::set_days_in_incubation, "Days in incubation period.")
        .def_property("days_with_symptoms", &Disease::get_days_with_symptoms,
                      &Disease::set_days_with_symptoms, "Days with symptoms.")
        .def_property("name", &Disease::get_name, &Disease::set_name, "Name of the disease.");

    // Bind Population class
    py::class_<Population, std::shared_ptr<Population>>(
        m, "Population", "Represents a population on a grid for simulating disease spread")
        .def(py::init<int, int, int, int, int, std::shared_ptr<Disease>, unsigned int,
                      const std::string &>(),
             py::arg("size"), py::arg("travel_radius"), py::arg("encounters"),
             py::arg("init_incubations"), py::arg("init_infections"), py::arg("disease"),
             py::arg("seed") = 0, py::arg("name") = "",
             "Initialize a Population with the given parameters.\n"
             "Args:\n"
             "    size (int): Grid size (size x size, positive).\n"
             "    travel_radius (int): Maximum encounter distance (non-negative).\n"
             "    encounters (int): Number of interactions per person (non-negative).\n"
             "    init_incubations (int): Number of initially incubated persons (non-negative).\n"
             "    init_infections (int): Number of initially infected persons (non-negative).\n"
             "    disease (Disease): Disease parameters.\n"
             "    seed (int): Seed for the RNG.\n"
             "    name (str, optional): Name of the population.\n"
             "Raises:\n"
             "    ValueError: If parameters are invalid.")
        .def("update", &Population::update, "Update the population for one time step.")
        .def(
            "reset", [](Population &self, bool same_seed) { self.reset(same_seed); },
            py::arg("same_seed") = false, "Reset the population to its initial state.")
        .def_property_readonly(
            "people",
            [](const Population &self) {
                const auto data = self.get_people();
                size_t size = data.size();
                py::array_t<int> array({size, size});
                auto r = array.mutable_unchecked<2>();
                for (size_t i = 0; i < size; ++i) {
                    for (size_t j = 0; j < size; ++j) {
                        r(i, j) = data[i][j];
                    }
                }
                return array;
            },
            "2D array of person statuses.")
        .def_property_readonly(
            "stats",
            [](const Population &self) {
                const auto &data = self.get_status_count();
                py::array_t<int> array(data.size());
                auto r = array.mutable_unchecked<1>();
                for (size_t i = 0; i < data.size(); ++i) {
                    r(i) = data[i];
                }
                return array;
            },
            "Array of counts for each status (Susceptible to Dead).")
        .def_property_readonly("size", &Population::get_size, "Grid size (size x size).")
        .def_property("travel_radius", &Population::get_travel_radius,
                      &Population::set_travel_radius, "Maximum encounter distance (non-negative).")
        .def_property("encounters", &Population::get_encounters, &Population::set_encounters,
                      "Number of interactions per person (non-negative).")
        .def_property("name", &Population::get_name, &Population::set_name,
                      "Name of the population.")
        .def_property("seed", &Population::get_seed, &Population::set_seed, "Seed of the RNG.");

    // Bind Model class
    py::class_<Model>(m, "Model", "Represents a SIR model for simulating disease spread")
        .def(py::init<int, std::shared_ptr<Population>, const std::string &>(),
             py::arg("days_in_simulation"), py::arg("population"), py::arg("name") = "",
             "Initialize a Model with the given Population.\n"
             "Args:\n"
             "    days_in_simulation (int): Total number of days to simulate (non-negative).\n"
             "    population (Population): The population being simulated.\n"
             "    name (str, optional): Name of the model.\n"
             "Raises:\n"
             "    ValueError: If parameters are invalid.")
        .def(
            "simulate", [](Model &self, int days) { return self.simulate(days); }, py::arg("days"),
            "Simulate the population for the given number of days.\n"
            "Args:\n"
            "    days (int): Number of days to simulate.\n"
            "Returns:\n"
            "    bool: True if simulation succeeded.\n"
            "Raises:\n"
            "    ValueError: If days is invalid.")
        .def(
            "reset", [](Model &self, bool same_seed) { self.reset(same_seed); },
            py::arg("same_seed") = false, "Reset the model to its initial state")
        .def_property_readonly(
            "data",
            [](const Model &self) {
                const auto &data = self.get_data();
                if (data.empty()) return py::array_t<int>({0, 0, 0});
                size_t time = data.size();
                size_t size = data[0].size();
                py::array_t<int> array({time, size, size});
                auto r = array.mutable_unchecked<3>();
                for (size_t i = 0; i < time; ++i) {
                    for (size_t j = 0; j < size; ++j) {
                        for (size_t k = 0; k < size; ++k) {
                            r(i, j, k) = data[i][j][k];
                        }
                    }
                }
                return array;
            },
            "3D array of population states for each day.")
        .def_property_readonly(
            "stats",
            [](const Model &self) {
                const auto &stats = self.get_stats();
                if (stats.empty()) return py::array_t<int>(py::array::ShapeContainer{0, 5});
                size_t time = stats.size();
                size_t status_count = stats[0].size();
                py::array_t<int> array({time, status_count});
                auto r = array.mutable_unchecked<2>();
                for (size_t i = 0; i < time; ++i) {
                    for (size_t j = 0; j < status_count; ++j) {
                        r(i, j) = stats[i][j];
                    }
                }
                return array;
            },
            "2D array of status counts for each day.")
        .def_property_readonly("population", &Model::get_population, "Population being simulated.")
        .def_property_readonly("current_day", &Model::get_current_day, "Current simulation day.")
        .def_property_readonly("remain_days", &Model::get_remain_days, "Remaining simulation days.")
        .def_property("name", &Model::get_name, &Model::set_name, "Name of the model.");
}
