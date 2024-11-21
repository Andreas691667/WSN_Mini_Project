import time
import os
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import numpy as np

class MoteDataReceiver:
    def __init__(self, file_path):
        self.file_path = file_path
        self.last_position = 0

    def read(self):
        current_size = os.path.getsize(self.file_path)
        if current_size > self.last_position:
            with open(self.file_path, "r") as f:
                f.seek(self.last_position)
                new_data = f.read()
                if new_data:
                    self.last_position = f.tell()
                    return new_data.strip().split("\n")  # Return lines as a list
        return None


def get_all_filenames(directory: str):
    return [
        f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))
    ]


# Global variables to store parsed data
data = {}
aggregated_data = []


def parse_data(lines):
    global aggregated_data
    for line in lines:
        parts = line.split(", ")
        for part in parts:
            if part.startswith("sample:"):
                sample_value = int(part.split(":")[1])
                # Update global data dictionary
                node_id = next(
                    (p.split(":")[1] for p in parts if p.startswith("node_id")), None
                )
                if node_id is not None:
                    node_id = int(node_id)
                    if node_id not in data:
                        data[node_id] = []
                    data[node_id].append(sample_value)
            elif part.startswith("Aggregated data:"):
                agg_value = float(part.split(":")[1])
                aggregated_data.append(round(agg_value))


def update_box_plot(frame):
    for receiver in receivers:
        rcv_data = receiver.read()
        if rcv_data:
            parse_data(rcv_data)

    # Clear old box plot
    ax1.clear()

    # Prepare data for box plot
    if data:
        box_data = [values for values in data.values()]
        ax1.boxplot(box_data, labels=[f"Node {k}" for k in data.keys()])

        # Calculate mean and variance for each node
        stats_text = []
        for node_id, values in data.items():
            mean_value = np.mean(values)
            var_value = np.var(values)
            stats_text.append(f"Node {node_id}: Mean={mean_value:.2f}, Var={var_value:.2f}")
        
        # Display mean and variance as legend text
        ax1.legend(
            [plt.Line2D([0], [0], color="w", marker="o", markerfacecolor="k", markersize=0)],  # Dummy handle
            ["\n".join(stats_text)],
            loc="upper right",
            fontsize="small",
            handlelength=0,
            handletextpad=0
        )

    ax1.set_title("Live Box Plot of Samples")
    ax1.set_ylabel("Sample Values")
    ax1.set_xlabel("Node IDs")


def update_combined_lineplot(frame):
    ax2.clear()

    # Define the sliding window size
    window_size = 50

    # Plot line for aggregated values with increased thickness
    if aggregated_data:
        start_idx = max(0, len(aggregated_data) - window_size)
        ax2.plot(
            range(start_idx, len(aggregated_data)),
            aggregated_data[start_idx:],
            marker='o',
            linestyle='-',
            color='red',
            linewidth=2.5,  # Thicker line for aggregated data
            label="Aggregated Data"
        )

    # Plot lines for each node's sample values
    if data:
        for node_id, values in data.items():
            start_idx = max(0, len(values) - window_size)
            ax2.plot(
                range(start_idx, len(values)),
                values[start_idx:],
                marker='o',
                linestyle='-',
                label=f"Node {node_id}"
            )

    # Add labels, title, and legend
    ax2.set_title("Live Line Plot: Sliding Window of Last 100 Values")
    ax2.set_xlabel("Time (steps)")
    ax2.set_ylabel("Values")
    ax2.legend(loc="upper left")
    ax2.grid(True)

if __name__ == "__main__":
    # Determine the latest data directory
    i = 1
    while os.path.exists(f"data_{i}"):
        i += 1
    i -= 1
    data_directory = f"data_{i}"
    file_names = get_all_filenames(data_directory)

    # Initialize receivers for each file
    receivers = [
        MoteDataReceiver(f"{data_directory}/{file_name}") for file_name in file_names
    ]

    # Initialize Matplotlib plot
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))  # Two subplots: box plot and line plot

    ani1 = FuncAnimation(fig, update_box_plot, interval=1000)
    ani2 = FuncAnimation(fig, update_combined_lineplot, interval=1000)

    plt.tight_layout()
    plt.show()
