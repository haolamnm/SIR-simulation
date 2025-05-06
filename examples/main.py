from visualize import show_simulation
from windows import import_module

import_module("ssir.cp312-win_amd64.pyd")

from ssir import Disease, Model, Population  # noqa: E402

disease = Disease(
    transmission_rate=0.4,
    fatality_rate=0.01,
    days_in_incubation=7,
    days_with_symptoms=8,
    name="Virus",
)
pop = Population(
    size=50,
    travel_radius=3,
    encounters=4,
    init_incubations=3,
    init_infections=1,
    disease=disease,
    name="City",
)
model = Model(
    days_in_simulation=200,
    population=pop,
    name="Model",
)

model.simulate(days=200)
show_simulation(model)
