# N-Body Gravitational Simulator

Simulation of N particles under gravitational interactions using the 
Barnes-Hut algorithm. Implemented as part of a simulation course at 
ENSTA Paris (2025), as a team of three.

## Method

The Barnes-Hut algorithm recursively partitions space into an octree 
(3D) or quadtree (2D), grouping distant particles into a single mass. 
This reduces the complexity of force computation from O(N²) to 
O(N log N).

At each time step:
1. The tree is built by inserting all particles into the root box
2. Each box stores its total mass and center of mass
3. For each particle, forces are computed by traversing the tree:
   - If a box is far enough (s/d < θ), it is treated as a single 
     point mass (multipole acceptance criterion)
   - Otherwise, the box is opened and its children are inspected
4. Positions and velocities are updated using the Leapfrog 
   (Störmer-Verlet) integrator

The opening parameter θ controls the trade-off between accuracy and 
performance. A smaller θ gives more accurate results at higher 
computational cost.

## Project Structure
gravitational-simulator/
├── src/
│   ├── Particule.hpp / Particule.cpp   # Particle class and linked list
│   ├── Boite.hpp / Boite.cpp           # Barnes-Hut tree (box/node)
│   ├── main.hpp / main.cpp             # Entry point
│   ├── test2D.cpp                      # 2D simulation with SFML
│   └── test3D.cpp                      # 3D simulation with Raylib
├── report/
│   └── report.pdf                      # Full project report (French)
└── README.md

## Dependencies

**2D simulation** — [SFML](https://www.sfml-dev.org/) (graphics, 
window, system)

**3D simulation** — [Raylib](https://www.raylib.com/)

Install on Ubuntu/Debian:
```bash
# SFML
sudo apt install libsfml-dev

# Raylib
sudo apt install libraylib-dev
```

## Build & Run

**2D simulation:**
```bash
g++ src/test2D.cpp src/Boite.cpp src/Particule.cpp \
    -o test2D \
    -lsfml-graphics -lsfml-window -lsfml-system
./test2D
```

**3D simulation:**
```bash
g++ src/test3D.cpp src/Boite.cpp src/Particule.cpp \
    -o test3D \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./test3D
```

In the 3D simulation, press **B** to toggle the octree visualization.

## Key Parameters

Defined in `src/Particule.hpp`:

| Parameter | Default | Description |
|-----------|---------|-------------|
| `D` | `3` | Spatial dimension (change to `2` for 2D) |
| `THETA` | `0.5` | MAC opening parameter |
| `eps` | `1.0` | Softening factor (avoids singularities) |
| `G` | `1.0` | Gravitational constant |

Defined in `src/test2D.cpp` / `src/test3D.cpp`:

| Parameter | Default | Description |
|-----------|---------|-------------|
| `N` | `20` / `200` | Number of particles |
| `dt` | `0.001` | Time step |

## Authors

Leto Van Ruymbeke, Jude Khazzaka, Ayoub Ed-Dib — ENSTA Paris, 2025
