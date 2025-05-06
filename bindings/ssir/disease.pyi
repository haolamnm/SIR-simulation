class Disease:
    def __init__(
        self,
        transmission_rate: float,
        fatality_rate: float,
        days_in_incubation: int,
        days_with_symptoms: int,
        name: str = "",
    ) -> None:
        """Initializes the Disease object with the given parameters."""
        ...

    @property
    def transmission_rate(self) -> float:
        """Returns the transmission rate of the disease."""
        ...

    @property
    def fatality_rate(self) -> float:
        """Returns the fatality rate of the disease."""
        ...

    @property
    def days_in_incubation(self) -> int:
        """Returns the number of days in incubation period."""
        ...

    @property
    def days_with_symptoms(self) -> int:
        """Returns the number of days with symptoms."""
        ...

    @property
    def name(self) -> str:
        """Returns name of the disease."""
        ...

    @name.setter
    def name(self, name: str) -> None:
        """Sets the name of the disease."""
        ...
