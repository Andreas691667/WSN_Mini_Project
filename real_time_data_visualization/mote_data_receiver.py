import os

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