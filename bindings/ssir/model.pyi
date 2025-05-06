from nptyping import Int, NDArray, Shape
from ssir.disease import Disease
from ssir.population import Population

class Model:
    def __init__(self, days_in_simulation: int, population: Population, name: str = "") -> None:
        """Initializes the Model object with the population and simulation parameters."""
        ...

    def simulate(self, days: int) -> NDArray[Shape["*, *, *, [days, size, size]"], Int]:
        """Returns 3D numpy array of shape (days, size, size) representing infection state over time."""
        ...

    @property
    def data(self) -> NDArray[Shape["*, *, *, [days, size, size]"], Int]:
        """3D numpy array of infection data over time (days, size, size)."""
        ...

    @property
    def stats(self) -> NDArray[Shape["*, 5, [days, statuses]"], Int]:
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
