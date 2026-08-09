# 🤖 Robot Collision Detection and Response Simulator

> A real-time 2D kinematic motion and 1D-track impulse physics simulator built in C++ using SFML.

---

## 👥 Team Details

* **Suhaib Ahmed**
* **Hammad Mujeeb**

* **Course:** B.S. Artificial Intelligence & Robotics
* **Project:** Robot Collision & Impulse Response Simulator

---

## 🎯 Project Overview

This application simulates real-time kinematic movement, momentum exchange, and dynamic collision responses between two autonomous robotic agents (**Robot A** and **Robot B**) constrained to a 1D bounded track.

Built from scratch using C++ and SFML, the application renders custom vector-drawn robot sprites, directional velocity vectors, animated contact sparks, and interactive UI widgets without relying on third-party GUI libraries.

---

## 🛠️ Technical Architecture & Code Structure

### Core Classes & Architecture
* **`Slider` Class:** Custom draggable horizontal control supporting pixel-to-coordinate mapping for real-time velocity ($v_A, v_B$) and simulation speed adjustment ($0.2\times$ to $4.0\times$).
* **`Button` & `IconButton` Classes:** Custom UI widgets handling collision mode selection (Elastic vs. Inelastic), play/pause state toggling, and environment resets.
* **`drawRobot()` Engine:** Procedurally draws multi-part robot sprites (body, head, eyes, antenna, arms, hands, and label tags) using SFML shapes.
* **`drawVelocityArrow()` & `drawImpactEffect()`:** Procedural visual overlay systems rendering dynamic directional vectors and a decaying impulse spark animation upon collision.

### Physics & Kinematics Engine
* **Impulse Physics Model:** Implements standard 1D two-body collision math supporting elastic ($e = 1.0$) and perfectly inelastic ($e = 0.0$) coefficients of restitution.
* **Boundary Dynamics:** Automatic velocity inversion at track boundaries ($X_{\text{start}} = 100\text{px}$, $X_{\text{end}} = 700\text{px}$).
* **Framerate Independence:** Scaled $\Delta t$ step logic with frame capping to maintain steady performance regardless of display hardware.

---

## 🛠️ Tech Stack & Requirements

* **Language:** C++17
* **Graphics Library:** SFML 2.5+ (`sfml-graphics`, `sfml-window`, `sfml-system`)
* **Build System:** CMake 3.20+
* **Display Resolution:** $800 \times 540$ Windowed
* **Font Fallback Support:** Multi-platform font detection (`arial.ttf` on Windows / `DejaVuSans.ttf` on Linux) with non-blocking fallback if fonts are missing.

---

## 🚀 How to Build and Run

### Prerequisites
* C++17 compatible compiler (`GCC`, `Clang`, or `MSVC`)
* SFML 2.5+ library installed and linked in `CMakeLists.txt`

### Build Steps (CLion)
1. Open the project repository folder in **CLion**.
2. Let CMake load and build the target executable.
3. Click **Run** (`Shift + F10`) to execute `robotcollison.cpp`.

---

## 🎮 How to Controls & Demo
1. **Adjust Speeds:** Drag Slider $A$ or Slider $B$ to set initial velocities ($\text{m/s}$).
2. **Toggle Modes:** Switch between **Elastic** (kinetic energy conserved) and **Perfectly Inelastic** (robots couple together).
3. **Simulation Speed:** Adjust the **Speed** slider to alter time scaling ($\Delta t$).
4. **Controls:** Toggle playback using the **Play/Pause** icon or click **Reset** to return agents to default positions.
