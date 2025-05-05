from nptyping import NDArray, Shape, Int
from ssir.disease import Disease

class Population:
    def __init__(self, size: int, travel_radius: int, encounters: int,
                 init_incubations: int, init_infections: int, disease: Disease,
                 name: str = "") -> None:
        """Initializes the Population object with various parameters."""
        ...

    def update(self) -> None:
        """Advances the simulation by one day."""
        ...

    @property
    def people(self) -> NDArray[Shape["*, *"], Int]:
        """2D numpy array of shape (size, size), each cell is int representing status."""
        ...

    @property
    def stats(self) -> NDArray[Shape[5], Int]:
        """1D numpy array with 5 elements representing status counts
        [Susceptible, Incubated, Infected, Recovered, Dead]."""
        ...

    @property
    def size(self) -> int:
        """Returns the size of the population."""
        ...

    @property
    def travel_radius(self) -> int:
        """Returns the travel radius of the population."""
        ...

    @property
    def encounters(self) -> int:
        """Returns the number of encounters per day."""
        ...

    @property
    def name(self) -> str:
        """Returns the name of the population."""
        ...

    @travel_radius.setter
    def travel_radius(self, radius: int) -> None:
        """Sets the travel radius."""
        ...

    @encounters.setter
    def encounters(self, encounters: int) -> None:
        """Sets the number of encounters."""
        ...

    @name.setter
    def name(self, name: str) -> None:
        """Sets the name of the population."""
        ...
