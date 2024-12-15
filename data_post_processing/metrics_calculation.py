import pandas as pd
import re


# Function to parse a single aggregator log file
def parse_aggregator_log(file_path):
    aggregator_logs = []
    with open(file_path, "r") as file:
        for line in file:
            # Regex to extract relevant fields from the aggregator log line
            match = re.match(
                r"aggregator, node_id: (\d+), sample: ([\d.]+), missed_samples: (\d+), msg_ids_in_msg: \[([0-9, ]+)\], node_ids_in_msg: \[([0-9, ]+)\] , time: (\d+)",
                line,
            )
            if match:
                node_id = int(match.group(1))
                sample = float(match.group(2))
                missed_samples = int(match.group(3))
                msg_ids_in_msg = list(map(int, match.group(4).split(", ")))
                node_ids_in_msg = list(map(int, match.group(5).split(", ")))
                time = int(match.group(6))

                # Store parsed data in a dictionary
                aggregator_logs.append(
                    {
                        "node_id": node_id,
                        "sample": sample,
                        "missed_samples": missed_samples,
                        "msg_ids_in_msg": msg_ids_in_msg,
                        "node_ids_in_msg": node_ids_in_msg,
                        "time": time,
                    }
                )
    return pd.DataFrame(aggregator_logs)


# Function to parse multiple source log files and aggregate them
def parse_source_logs(file_paths):
    source_logs = []
    for file_path in file_paths:
        with open(file_path, "r") as file:
            for line in file:
                # Regex to extract relevant fields from the source log line
                match = re.match(
                    r"source, node_id: (\d+), sample: ([\d.]+), mean: ([\d.]+), variance: ([\d.]+), msg_id: (\d+) , time: (\d+)",
                    line,
                )
                if match:
                    node_id = int(match.group(1))
                    sample = float(match.group(2))
                    mean = float(match.group(3))
                    variance = float(match.group(4))
                    msg_id = int(match.group(5))
                    time = int(match.group(6))

                    # Store parsed data in a dictionary
                    source_logs.append(
                        {
                            "node_id": node_id,
                            "sample": sample,
                            "mean": mean,
                            "variance": variance,
                            "msg_id": msg_id,
                            "time": time,
                        }
                    )
    return pd.DataFrame(source_logs)


# Function to calculate metrics based on the logs
def calculate_metrics(aggregator_df, source_df):
    total_source_messages = len(source_df)
    received_messages = 0
    total_latency = 0
    total_accuracy = 0

    for _, row in source_df.iterrows():
        # Check if the message was received by the aggregator by matching the message ID and node ID
        match = aggregator_df[
            # Apply the condition that ensures the msg_id and node_id are paired correctly
            aggregator_df.apply(
                lambda row_agg: any(
                    (row["msg_id"] == msg_id) and (int(row["node_id"]) == node_id)
                    for msg_id, node_id in zip(
                        row_agg["msg_ids_in_msg"], row_agg["node_ids_in_msg"]
                    )
                ),
                axis=1,
            )
        ]

        if not match.empty:
            received_messages += 1
            # Calculate latency
            latency = match["time"].values[0] - row["time"]
            total_latency += latency

            # Calculate accuracy
            accuracy = abs(row["sample"] - match["sample"].values[0])
            total_accuracy += accuracy

    missed_messages = total_source_messages - received_messages
    completeness = (received_messages / total_source_messages) * 100
    avg_latency = total_latency / received_messages if received_messages > 0 else 0
    avg_accuracy = total_accuracy / received_messages if received_messages > 0 else 0

    return {
        "total_messages": total_source_messages,
        "received_messages": received_messages,
        "missed_messages": missed_messages,
        "completeness": completeness,
        "avg_latency": avg_latency,
        "avg_accuracy": avg_accuracy,
    }


# Paths to your .txt log files
prefix = "logs/data_3/mote_"
aggregator_log_file = f"{prefix}0_data.txt"
source_log_files = [
    f"{prefix}3_data.txt",
    f"{prefix}4_data.txt",
    f"{prefix}5_data.txt",
    f"{prefix}6_data.txt",
    f"{prefix}7_data.txt",
    f"{prefix}8_data.txt",
]

# Parse the aggregator log and all source logs
aggregator_df = parse_aggregator_log(aggregator_log_file)
source_df = parse_source_logs(source_log_files)

# Calculate metrics
metrics = calculate_metrics(aggregator_df, source_df)

# Output the metrics
print(f"Total messages: {metrics['total_messages']}")
print(f"Received messages: {metrics['received_messages']}")
print(f"Missed messages: {metrics['missed_messages']}")
print(f"Completeness: {metrics['completeness']:.2f}%")
print(f"Average latency: {metrics['avg_latency']} ms")
print(f"Average accuracy: {metrics['avg_accuracy']} units")
