# Data Logging

RoboMotion Simulator can log simulation data to CSV files for analysis.

## Enabling Logging

1. Open the **Robot Stats** window from the main menu.
2. Optionally click **Find log folder** to choose a custom output directory.
3. Click **Start Logging** to begin recording.
4. The current log file name and directory are shown in the window.
5. A **[REC] Recording...** indicator confirms logging is active.
6. Click **Stop Logging** to finish.

## Log File Location

By default, log files are stored in a `logs/` directory next to the executable. You can change the output directory using the folder picker button in the Robot Stats window.

Log files are named with a timestamp: `log_<epoch>.csv` (e.g., `log_17435892834567.csv`).

The directory is created automatically if it does not exist.

## CSV Format

Each row represents one simulation frame. The columns are:

### Global Columns

| Column          | Unit    | Description                          |
|-----------------|---------|--------------------------------------|
| `time`          | HH:MM:SS.mmm | Wall-clock timestamp           |
| `pos_x`         | mm      | Robot X position                     |
| `pos_y`         | mm      | Robot Y position                     |
| `chassis_angle` | rad     | Robot orientation                    |
| `vel_x`         | mm/s    | Global X velocity                    |
| `vel_y`         | mm/s    | Global Y velocity                    |
| `angular_vel`   | rad/s   | Angular velocity                     |
| `distance_x`    | mm      | Cumulative X distance traveled       |
| `distance_y`    | mm      | Cumulative Y distance traveled       |

### Per-Wheel Columns (repeated for each wheel `wN`)

| Column                  | Unit  | Description                      |
|-------------------------|-------|----------------------------------|
| `wN_speed`              | mm/s  | Wheel linear speed               |
| `wN_roller_speed`       | mm/s  | Roller speed (omni/mecanum)      |
| `wN_distance`           | mm    | Cumulative distance traveled     |
| `wN_last_displacement`  | mm    | Distance traveled in this frame  |

## Usage Tips

- Log data is written every frame, so file sizes can grow quickly at high frame rates.
- Use a spreadsheet application or Python/pandas to analyze the CSV files.
- The timestamp uses wall-clock time, not simulation time.
- When overwriting is enabled (default for new logs), the CSV header is written at the top of the file.
