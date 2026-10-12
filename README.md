<!--
SPDX-FileCopyrightText: 2026 Michael Wipplinger (micaah42)

SPDX-License-Identifier: LGPL-3.0-or-later
-->

# QPerformance

QPerformance is a Qt library for monitoring system and application performance,
recording samples, and displaying them in QML charts. Monitors publish values
only while they have subscribers, so a chart or recorder can start and stop
sampling as it is attached or detached.

## Getting Started
### 🧰 Requirements and platform notes

- CMake 3.22 or newer
- Qt 6.5 or newer, including Qt Quick
- Qt Test, if building the tests

`CPUMonitor` and `MemoryMonitor` currently read Linux `/proc` files. CPU
temperature is read from `/sys/class/thermal/thermal_zone0/temp` when that file
is available. These two monitors are therefore Linux-specific in the current
implementation. `FPSMonitor` measures frame swaps from a `QQuickWindow`.

### 🔧 Build shared library form source and install

From this directory, configure and build the library:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.5.0/gcc_64
cmake --build build
```

Omit `CMAKE_PREFIX_PATH` if Qt is already discoverable by CMake. Run the tests
with:

```sh
ctest --test-dir build --output-on-failure
```

### Use with CMake FetchContent

In the consuming project's `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(qperformance
    GIT_REPOSITORY https://github.com/micaah42/qperformance.git
    GIT_TAG main
)
FetchContent_MakeAvailable(qperformance)

target_link_libraries(my_app PRIVATE qperformance)
```

For reproducible builds, replace `main` with a release tag or commit.

### Use as a Git submodule

Add the repository to the consuming project:

```sh
git submodule add https://github.com/micaah42/qperformance.git external/qperformance
```

Then add and link it from CMake:

```cmake
add_subdirectory(external/qperformance)
target_link_libraries(my_app PRIVATE qperformance)
```

The QML module URI is `QPerformanceQml`. Import it in QML with
`import QPerformanceQml`.

## 📊 Features

### 📈 Monitors

- `CPUMonitor`: total and per-core CPU load, CPU identification, and
  temperature when the Linux thermal-zone file is available.
- `MemoryMonitor`: memory and swap capacity, available/free amounts, and
  usage.
- `FPSMonitor`: current and average frame rate for a `QQuickWindow`.

Planned monitors:

- [⏳] `ProcessCPUMonitor`: monitor the CPU load caused by a specific process.
- [⏳] `ProcessMemoryMonitor`: monitor the memory usage caused by a specific process.

### 📉 Charts

- `PerformanceChart`: time-window chart that redraws its stored samples.
- `LazyRedrawChart`: chart that shifts a cached image and draws new segments.
- `CPUGraph`: CPU-specific chart for per-core usage.

Planned charts:

- [⏳] `PerformanceChartNew`: hardware-accelerated chart that redraws all data
  points.
- [⏳] `StaticChart`: chart that keeps existing lines stationary and overwrites
  them when they reach the chart edge.

The chart classes are QML `QQuickPaintedItem`s. Their `duration` property is in
milliseconds. A minimal QML example:

```qml
import QtQuick
import QPerformanceQml

PerformanceChart {
    width: 480
    height: 160
    monitor: CPUMonitor
    duration: 5000
    min: 0
    max: 100
    colors: ["#4caf50"]
}
```

### ⏺️ Recording

`Recorder` subscribes to a monitor and buffers samples for compressed `.qpdp`
recordings. The format can also be read and written through the C++ static
methods on `Recorder`. A Python reader is provided in
[libdatapoints.py](libdatapoints.py); it requires Python 3 and NumPy.

Current implementation caveats:

- Each flush assigns a timestamped path under the current directory, so
  `fileName` does not currently provide a stable output-path override.
- Data is appended as compressed payloads. The Python helper reads a single
  payload, so a recording with multiple flushes may need additional parsing.

## 📄 License

This library is available under the LGPL v3 or later versions.

Source files use SPDX headers for REUSE compliance. To add a header to new
source files and check compliance, run this from the repository root:

```sh
./tools/update-license-header.sh
```

The script uses `reuse annotate` to add headers to files that do not already
have them, then runs `reuse lint`.

## 🤝 Contributing

Bug reports, feature requests, and fixes are welcome. Please open an issue or
pull request in the repository.
