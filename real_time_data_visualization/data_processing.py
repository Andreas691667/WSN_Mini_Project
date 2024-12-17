import os

# Global data storages
data = {}
aggregated_data = {}

def get_all_filenames(directory: str):
    return [
        f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))
    ]

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
                    # Convert ms to seconds
                    time_value = int(time_value[1:-2]) / 1000
                    data[node_id].append((sample_value, time_value))

            elif parts[0].startswith("aggregator"):
                node_id = int(parts[1].split(":")[1])
                agg_value = parts[2].split(":")[1]
                agg_value = 0 if agg_value == "" else float(agg_value)
                missed = int(parts[3].split(":")[1])
                time_value = parts[-1].split(":")[1]
                # Convert ms to seconds
                time_value = int(time_value[1:-2]) / 1000
                if node_id not in aggregated_data:
                    aggregated_data[node_id] = []
                aggregated_data[node_id].append((round(agg_value), missed, time_value))
    except Exception as e:
        print(f"Error parsing data: {e}")