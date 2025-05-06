import matplotlib.animation as animation
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from windows import import_module

import_module("ssir.cp312-win_amd64.pyd")

from ssir import Model  # noqa: E402


def show_simulation(model: Model, save_path: str | None = None, show: bool = True) -> animation.FuncAnimation:
    """Visualize the simulation with grid and status counts."""
    data = model.data
    stats = model.stats
    days, size, _ = data.shape
    if stats.shape != (days, 5):
        raise ValueError("Stats shape mismatch")

    colors = ["white", "pink", "red", "gray", "black"]
    cmap = ListedColormap(colors)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 6), gridspec_kw={"width_ratios": [3, 1]})
    fig.suptitle(f"Simulation of {model.name}")

    # Grid plot
    im = ax1.imshow(data[0], cmap=cmap, vmin=0, vmax=4)
    ax1.axis("off")

    # Bar plot
    states = ["S", "E", "I", "R", "D"]
    bar_container = ax2.bar(states, stats[0], color=colors, edgecolor="black")
    ax2.set_ylim(0, model.population.size**2)
    ax2.set_ylabel("Count")
    ax2.set_title("Status Counts")

    def update(frame: range) -> list:
        im.set_data(data[frame])
        ax1.set_title(f"Day {frame}")
        for rect, h in zip(bar_container, stats[frame], strict=False):
            rect.set_height(h)
        return [im, *list(bar_container)]

    ani = animation.FuncAnimation(fig, update, frames=range(days), blit=True, interval=200, repeat=False)

    if save_path:
        ani.save(save_path, writer="pillow", fps=5)
        print(f"Saved animation to {save_path}")

    if show:
        plt.show()
    else:
        plt.close(fig)

    return ani


if __name__ == "__main__":
    pass
