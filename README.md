# BinTable

**A lightweight, native database client for people who care about performance.**

BinTable is an open-source database GUI built with C++20 and Dear ImGui. No JVM. No Electron. No bloat — just a fast, native application that starts instantly and runs the same way on Linux, Windows, and macOS.

---

## Why BinTable?

Most database clients today are either too heavy or too expensive:

- **DBeaver** ships a full JVM — a database browser shouldn't require a Java runtime.
- **TablePlus, DataGrip** lock advanced features behind subscriptions.
- **Electron-based tools** carry a browser engine just to render a table.

BinTable takes the opposite approach: a single compiled binary, minimal dependencies, and a UI that renders at native speed. It is developed **Linux-first** — a platform that has historically been an afterthought for database tooling — while delivering equal quality on Windows and macOS.

If you prefer tools that are fast, transparent, and stay out of your way, BinTable is for you.

---

## Features

### Current

- **PostgreSQL** — connect, browse schemas, run queries, view results
- **Schema browser** — hierarchical tree: server → database → schema → tables, views, functions
- **Query editor** — multi-tab SQL editor with results grid
- **Table viewer** — `SELECT *` with virtual scrolling for large result sets
- **Async connection test** — non-blocking connect/test without freezing the UI
- **Dark theme** — a clean dark style built for long coding sessions
- **Font Awesome icons** — crisp icon glyphs at any DPI
- **High-DPI aware** — automatically adapts to display scale changes
- **Debian packaging** — `.deb` build script for Linux distribution

### Planned

- **MariaDB / MySQL** support
- **SQLite** support
- **LLM integration via llama.cpp** — on-device AI assistant for writing and explaining queries, with no data leaving your machine
- Schema diff and migration helpers
- Connection profile persistence
- Query history
- Export to CSV / JSON

---

## Design Philosophy

| Principle | Detail |
|---|---|
| **Native** | Compiled C++ binary. Starts in milliseconds, uses single-digit MB of RAM at idle. |
| **No runtime dependencies** | No JVM, no Node, no Python. Just the binary and a few system libraries. |
| **Linux-first** | Developed and tested primarily on Linux. Windows and macOS are first-class targets, not afterthoughts. |
| **Minimal dependencies** | UI: Dear ImGui + SDL3 + OpenGL. Database: official C client libraries only. Nothing else. |
| **Open source** | Core features are and will remain free. |
| **Cross-platform parity** | The same codebase, the same binary behavior, the same UI on all three platforms. |

---

## Technology Stack

| Layer | Technology |
|---|---|
| Language | C++20 |
| UI framework | [Dear ImGui](https://github.com/ocornut/imgui) |
| Windowing / input | SDL3 |
| Rendering | OpenGL 3.3 Core |
| Font rendering | FreeType |
| PostgreSQL driver | libpq (official C library) |
| LLM inference *(planned)* | [llama.cpp](https://github.com/ggerganov/llama.cpp) |
| Build system | CMake 3.20+ |

No other third-party libraries are used.

---

## Building from Source

### Prerequisites

| Platform | Requirements |
|---|---|
| **Linux** | `cmake`, `ninja` (or `make`), `libpq-dev`, `libfreetype-dev`, OpenGL headers (`libgl-dev`) |
| **Windows** | CMake, Visual Studio 2022 (or MSVC Build Tools), PostgreSQL client libraries |
| **macOS** | CMake, Xcode Command Line Tools, `postgresql` (Homebrew), `freetype` (Homebrew) |

SDL3, Dear ImGui, and (if not found on the system) FreeType are fetched and built automatically via CMake `FetchContent`.

### Build

```bash
git clone https://github.com/your-username/BinTable.git
cd BinTable
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

The `bintable` executable and `assets/` directory will be placed in the build output folder.

#### Linux — one-liner

```bash
sudo apt install cmake ninja-build libpq-dev libfreetype-dev libgl-dev
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build
```

#### Debian package

```bash
bash scripts/build-deb.sh
```

---

## Supported Databases

| Database | Status |
|---|---|
| PostgreSQL | Supported |
| MariaDB / MySQL | Planned |
| SQLite | Planned |

---

---

## Contributing

Contributions are welcome.

## License

MIT — see [LICENSE](LICENSE).
