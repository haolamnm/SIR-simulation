from nptyping import Int, NDArray, Shape
from ssir.disease import Disease

class Population:
    def __init__(
        self,
        size: int,
        travel_radius: int,
        encounters: int,
        init_incubations: int,
        init_infections: int,
        disease: Disease,
        seed: int = 0,
        name: str = "",
    ) -> None:
        """Initializes the Population object with various parameters."""
        ...

    def update(self) -> None:
        """Advances the simulation by one day."""
        ...

    def reset(self, same_seed: bool = False) -> None:
        """Reset the population to its initial state."""
        ...

    @property
    def people(self) -> NDArray[Shape["*, *, [size, size]"], Int]:  # noqa: F722
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

    @property
    def seed(self) -> int:
        """Returns the seed of the RNG."""
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

    @seed.setter
    def seed(self, seed: int) -> None:
        """Sets the sed of the RNG."""
        ...
