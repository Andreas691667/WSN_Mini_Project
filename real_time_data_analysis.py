import time
import os
import sys
from typing import Optional, List


class MoteDataReceiver:
    def __init__(self, file_path):
        self.file_path = file_path
        self.last_position = 0

    def read(self) -> Optional[str]:
        current_size = os.path.getsize(self.file_path)
        if current_size > self.last_position:
            with open(self.file_path, "r") as f:
                f.seek(self.last_position)
                new_data = f.read()
                if new_data:
                    self.last_position = f.tell()
                    return new_data
        return None


def get_all_filenames(directory: str) -> List[str]:
    return [
        f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))
    ]


if __name__ == "__main__":
    i = 1
    while os.path.exists(f"data_{i}"):
        i += 1
    i -= 1
    data_directory = f"data_{i}"
    file_names = get_all_filenames(data_directory)

    receivers = [MoteDataReceiver(f"{data_directory}/{file_name}") for file_name in file_names]

    while True:
        for receiver in receivers:
            rcv_data = receiver.read()
            if rcv_data:
                print(rcv_data)
        time.sleep(0.5)
