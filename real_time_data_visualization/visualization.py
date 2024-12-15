import matplotlib.pyplot as plt
import numpy as np
from data_processing import data, aggregated_data, parse_data

def update_box_plot(frame, receivers, ax1):
    window_size = 50
    
    # Read new data
    for receiver in receivers:
        rcv_data = receiver.read()
        if rcv_data:
            parse_data(rcv_data)

    # Clear old box plot
    ax1.clear()

    # Prepare data for box plot
    if data:
        box_data = []
        stats_text = []
        for node_id, values in data.items():
            vals, _ = zip(*values)
            start_idx = max(0, len(vals) - window_size)
            mean_value = np.mean(vals[start_idx:])
            var_value = np.var(vals[start_idx:])
            stats_text.append(f"Node {node_id}: Mean={mean_value:.2f}, Var={var_value:.2f}")
            box_data.append(vals[start_idx:])
            
        # Update box plot with new data
        ax1.boxplot(box_data, labels=[f"Node {k}" for k in data.keys()])
        
        # Display mean and variance as legend text (dummy handle trick for multiline text)
        ax1.legend(
            [plt.Line2D([0], [0], color="w", marker="o", markerfacecolor="k", markersize=0)],
            ["\n".join(stats_text)],
            loc="upper right",
            fontsize="small",
            handlelength=0,
            handletextpad=0
        )

    ax1.set_title("Live Box Plot of Samples")
    ax1.set_ylabel("Sample Values")
    ax1.grid(True)


def update_combined_lineplot(frame, ax2, ax3):
    ax2.clear()
    ax3.clear()

    window_size = 50
    # Plot aggregator data
    if aggregated_data:
        for key, value in aggregated_data.items():
            agg_values, missed_data, agg_time = zip(*value)
            start_idx = max(0, len(agg_values) - window_size)
            # Aggregated line plot (thicker line)
            ax2.plot(
                agg_time[start_idx:],
                agg_values[start_idx:],
                marker=',',
                markersize=10,
                linestyle='-',
                linewidth=3,
                label=f"N1: Aggregated Data From Node: {key}"
            )
            missed_window_cum = np.cumsum(missed_data[max(0, len(missed_data) - window_size):])
            missed = sum(missed_data)
            # Plot cumulative missed data
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
        ax3.set_title("Live Missed Data Plot")
        ax3.set_xlabel("Time [s]")
        ax3.set_ylabel("Count")
        ax3.legend(loc="upper left")
        ax3.grid(True)

    # Plot individual node data
    if data:
        max_len = max([len(values) for values in data.values()])
        for node_id, values_list in data.items():
            vals, time_stamps = zip(*values_list)
            start_idx = max(0, max_len - window_size)
            ax2.plot(
                time_stamps[start_idx:],
                vals[start_idx:],
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