from nptyping import NDArray, Shape, Int
from ssir.population import Population
from ssir.disease import Disease
from typing import overload

class Model:
    @overload
    def __init__(self, days_in_simulation: int, population: Population, name: str = "") -> None:
        """Initializes the Model object with the population and simulation parameters."""
        ...

    @overload
    def __init__(self, size: int, travel_radius: int, encounters: int, init_incubations: int, init_infections: int, disease: Disease, name: str = "") -> None:
        """Initializes the Model object with the population and simulation parameters."""
        ...

    def simulate(self, days: int) -> NDArray[Shape["*, *, *"], Int]:
        """Returns 3D numpy array of shape (days, size, size) representing infection state over time."""
        ...

    @property
    def data(self) -> NDArray[Shape["*, *, *"], Int]:
        """3D numpy array of infection data over time (days, size, size)."""
        ...

    @property
    def stats(self) -> NDArray[Shape["*, *"], Int]:
        """2D numpy array of shape (5, size) representing status counts over time."""
        ...

    @property
    def population(self) -> Population:
        """Returns the Population object used in the simulation."""
        ...

    @property
    def remain_days(self) -> int:
        """Returns the remaining days in the simulation."""
        ...

    @property
    def current_day(self) -> int:
        """Returns the current day of the simulation."""
        ...

    @property
    def name(self) -> str:
        """Returns the name of the model."""
        ...

    @name.setter
    def name(self, name: str) -> None:
        """Sets the name of the model."""
        ...
