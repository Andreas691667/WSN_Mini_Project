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
aggregated_data = {}


def parse_data(lines):
    try:
        for line in lines:
            parts = line.split(", ")
            if parts[0].startswith("source"):
                node_id = parts[1].split(":")[1]
                sample_value = int(parts[2].split(":")[1])
                if node_id is not None:
                    node_id = int(node_id)
                    if node_id not in data:
                        data[node_id] = []
                    time_value = parts[-1].split(":")[1]
                    time_value = int(time_value[1:-2]) / 1000 # Convert ms to seconds
                    data[node_id].append((sample_value, time_value))
            elif parts[0].startswith("aggregator"):
                node_id = int(parts[1].split(":")[1])
                agg_value = parts[2].split(":")[1]
                agg_value = 0 if agg_value == "" else float(agg_value)
                missed = int(parts[3].split(":")[1])
                time_value = parts[-1].split(":")[1]
                time_value = int(time_value[1:-2]) / 1000 # Convert ms to seconds
                if node_id not in aggregated_data:
                    aggregated_data[node_id] = []
                aggregated_data[node_id].append((round(agg_value), missed, time_value))
    except Exception as e:
        print(f"Error parsing data: {e}")



def update_box_plot(frame):
    window_size = 50
    
    for receiver in receivers:
        rcv_data = receiver.read()
        if rcv_data:
            parse_data(rcv_data)

    # Clear old box plot
    ax1.clear()

    # Prepare data for box plot
    if data:
        box_data = []

        # Calculate mean and variance for each node
        stats_text = []
        for node_id, values in data.items():
            values, _ = zip(*values)
            start_idx = max(0, len(values) - window_size)
            mean_value = np.mean(values[start_idx:])
            var_value = np.var(values[start_idx:])
            stats_text.append(f"Node {node_id}: Mean={mean_value:.2f}, Var={var_value:.2f}")
            box_data.append(values[start_idx:])
            
        # Update box plot with new data
        ax1.boxplot(box_data, labels=[f"Node {k}" for k in data.keys()])
        
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
    ax1.grid(True)


def update_combined_lineplot(frame):
    ax2.clear()
    ax3.clear()

    # Define the sliding window size
    window_size = 50
    if aggregated_data:
        for key, value in aggregated_data.items():
            # Plot line for aggregated values with increased thickness
            agg_values, missed_data, agg_time = zip(*(value))
            # start_idx = max(0, len(agg_values) - window_size)
            agg_time = list(filter(lambda x: agg_time[-1] - x < 5, agg_time))
            start_idx = max(0, len(agg_values) - len(agg_time))
            ax2.plot(
                agg_time,
                agg_values[start_idx:],
                marker=',',
                markersize=10,
                linestyle='-',
                linewidth=3,  # Thicker line for aggregated data
                label=f"N1: Aggregated Data From Node: {key}"
            )
            missed_window_cum = np.cumsum(missed_data[max(0, len(missed_data) - window_size):])
            missed = sum(missed_data)
            # Plot line for cumulative missed data
            if missed_data:
                start_idx = max(0, len(missed_data) - window_size)
                ax3.plot(
                    agg_time[start_idx:],
                    missed_window_cum,
                    marker='o',
                    markersize=1,
                    linestyle='-',
                    label=f"Total Missed Data (node: {key}): {missed}"
                )
                # Update legend dynamically
        ax3.set_title("Live Missed Data Plot")
        ax3.set_xlabel("Time [s]")
        ax3.set_ylabel("Count")
        ax3.legend(loc="upper left")
        ax3.grid(True)

    # Plot lines for each node's sample values
    if data:
        max_len = max([len(values) for values in data.values()])
            
        for node_id, values in data.items():
            values, time = zip(*values)
            # start_idx = max(0, max_len - window_size)
            agg_time = list(filter(lambda x: time[-1] - x < 5, agg_time))
            start_idx = max(0, len(agg_values) - len(agg_time))
            ax2.plot(
                time[start_idx:],
                values[start_idx:],
                marker=',',
                markersize=3,
                linestyle='--',
                alpha=0.4,
                label=f"Node {node_id}"
            )

    # Add labels, title, and legend
    ax2.set_title("Live Line Plot")
    ax2.set_xlabel("Time [s]")
    ax2.set_ylabel("Values")
    ax2.legend(loc="upper left")
    ax2.grid(True)
    
# def update_missed_plot(frame):
#     ax3.clear()
#     window_size = 50
#     if aggregated_data:
#         _, agg_time = zip(*aggregated_data)
#         missed_window_cum = np.cumsum(missed_data[max(0, len(missed_data) - window_size):])
#         missed = sum(missed_data)
#         # Plot line for cumulative missed data
#         if missed_data:
#             start_idx = max(0, len(missed_data) - window_size)
#             ax3.plot(
#                 agg_time[start_idx:],
#                 missed_window_cum,
#                 marker='o',
#                 markersize=1,
#                 linestyle='-',
#                 color='black',
#                 label=f"Missed Data - Total: {missed}"
#             )
#             # Update legend dynamically
#         ax3.set_xlabel("Time")
#         ax3.set_ylabel("Missed Data")
#         ax3.legend(loc="upper left")

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
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8))  # Three subplots: box plot, missed plot and line plot

    # ani1 = FuncAnimation(fig, update_missed_plot, interval=1000)
    ani1 = FuncAnimation(fig, update_box_plot, interval=1000)
    ani2 = FuncAnimation(fig, update_combined_lineplot, interval=1000)

    plt.tight_layout()
    plt.show()
