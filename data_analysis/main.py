import os
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from mote_data_receiver import MoteDataReceiver
from data_processing import get_all_filenames
from visualization import update_box_plot, update_combined_lineplot

if __name__ == "__main__":
    # Determine the latest data directory
    i = 1
    while os.path.exists(os.path.join("..", "logs", f"data_{i}")):
        i += 1
    i -= 1
    data_directory = os.path.join("..", "logs", f"data_{i}")
    file_names = get_all_filenames(data_directory)

    # Initialize receivers for each file
    receivers = [
        MoteDataReceiver(f"{data_directory}/{file_name}") for file_name in file_names
    ]

    # Initialize Matplotlib subplots
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 8))

    # Set up animations
    ani1 = FuncAnimation(fig, update_box_plot, fargs=(receivers, ax1), interval=100)
    ani2 = FuncAnimation(fig, update_combined_lineplot, fargs=(ax2, ax3), interval=100)

    plt.tight_layout()
    plt.show()