import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np
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

    # Color scheme
    colors = ["#D3D3D3", "#FF69B4", "#FF0000", "#228B22", "#333333"]  # Light Gray, Pink, Red, Forest Green, Light Black
    cmap = ListedColormap(colors)

    # Dark theme setup
    plt.style.use("dark_background")
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(9, 4), gridspec_kw={"width_ratios": [3, 1]}, facecolor="#1E1E1E")
    fig.suptitle(f"Simulation of {model.name}", color="white")

    # Grid plot
    im = ax1.imshow(data[0], cmap=cmap, vmin=0, vmax=4)
    ax1.axis("off")
    title = ax1.set_title("Day 0", color="white", fontsize=12, pad=10)
    ax1.set_facecolor("#2D2D2D")

    # Bar plot
    states = ["S", "E", "I", "R", "D"]
    bar_container = ax2.bar(states, stats[0], color=colors, edgecolor="white")
    ax2.set_ylim(0, model.population.size**2)
    ax2.set_ylabel("Count", color="white")
    ax2.set_title("Status Counts", color="white")
    ax2.set_facecolor("#2D2D2D")
    ax2.tick_params(colors="white")
    ax2.grid(True, axis="y", color="gray", linestyle="--", alpha=0.5)

    def update(frame: int) -> list:
        im.set_data(data[frame])
        title.set_text(f"Day {frame}")
        plt.draw()
        for rect, h in zip(bar_container, stats[frame], strict=False):
            rect.set_height(h)
        return [im, *list(bar_container)]

    ani = animation.FuncAnimation(fig, update, frames=range(days), blit=True, interval=200, repeat=False)

    if save_path:
        ani.save(save_path, writer="pillow", fps=5)
        print(f"Saved animation to {save_path}")

    if show:
        plt.tight_layout()
        plt.show()
    else:
        plt.close(fig)

    return ani


if __name__ == "__main__":
    pass
