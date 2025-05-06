from visualize import show_simulation
from windows import import_module

import_module("ssir.cp312-win_amd64.pyd")

from ssir import Disease, Model, Population  # noqa: E402

# try:
#     # Create a Disease object
#     disease = Disease(
#         transmission_rate=0.3, fatality_rate=0.1, days_in_incubation=5, days_with_symptoms=7, name="TestDisease"
#     )
#     print("Disease created:", disease.name)

#     # Create a Population object
#     population = Population(
#         size=10,
#         travel_radius=1,
#         encounters=2,
#         init_incubations=1,
#         init_infections=1,
#         disease=disease,
#         name="TestPopulation",
#     )
#     print("Population created:", population.name)

#     # Create a Model object
#     model = Model(days_in_simulation=30, population=population, name="TestModel")
#     print("Model created:", model.name)

#     # Run a simulation
#     model.simulate(10)
#     print("Simulation completed")

# except Exception as e:
#     print("Error:", str(e))
#     raise

disease = Disease(
    transmission_rate=0.2,
    fatality_rate=0,
    days_in_incubation=7,
    days_with_symptoms=8,
    name="Virus",
)
pop = Population(
    size=15,
    travel_radius=1,
    encounters=4,
    init_incubations=1,
    init_infections=0,
    disease=disease,
    name="City",
)
model = Model(
    days_in_simulation=50,
    population=pop,
    name="Model",
)
model.simulate(days=50)

show_simulation(model)
