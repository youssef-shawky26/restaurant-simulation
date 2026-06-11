# 🍽️ Real-Time Restaurant Queuing Simulation

A dynamic, real-time C++ simulation that models restaurant crowd management and table turnover operations. This project features custom-implemented linear data structures and a clean graphical interface built using the **Raylib** library.

---

## 🚀 Key Features

* **Custom Queue Infrastructures:** Implements three independent queue architectures built from scratch:
  * **Dine-In Queue:** A fixed-size circular array queue managing standard walkthrough customers.
  * **Priority Reservation Queue:** A dedicated queue ensuring customers with pre-booked tables are seated before regular walk-ins.
  * **Takeaway Queue:** A dynamic, singly-linked list queue managing customer orders handled by a dedicated cashier.
* **Dynamic Hostess Routing:** Features a load-balancing seating algorithm that cycles through tables sequentially, matching party sizes ($1$ to $4$ people) to the smallest available table configurations to maximize spatial efficiency.
* **Real-Time Visual Dashboard:** Renders visual representations of restaurant tables (color-coded for busy vs. empty states), ticking timers for service durations, and graphical representations of all waiting lines.
* **End-of-Run Analytics:** Generates graphical, scrollable pop-up summary cards upon simulation completion, detailing table utilization statistics (occupancy percentages) and complete individual customer logs.

---

## 🛠️ System Architecture & Mechanics

The simulation ticks forward incrementally, where each simulation tick corresponds to a fixed frame window ($15$ frames at $60$ FPS). 

### The Workflow Pipeline
1. **Arrival Phase:** Customers are generated randomly based on a configurable global arrival rate. They are assigned a group size and flagged as Dine-In, Priority, or Takeaway.
2. **Seating Phase:** The `Hostess::prioritySeating` routine fires continuously, draining the Priority queue first, then the Walk-In queue, assigning groups to the structural `Server` tables.
3. **Leaving Phase:** The core clock tracks customer service times. Upon expiration, tables are cleared instantly, and their historical data is pushed into a tracking `std::vector` for final metric analysis.

---

## 💻 Tech Stack

* **Language:** C++11 or higher
* **Graphics Library:** [Raylib](https://www.raylib.com/)
* **Standard Headers Used:** `<vector>`, `<random>`, `<iomanip>`, `<iostream>`

---

## 🧑‍💻 How to Run Locally

### Prerequisites
Ensure you have a C++ compiler installed (such as GCC/MinGW) and that the **Raylib** development headers and binaries are correctly configured on your environment paths.

### 1. Compilation
Open your terminal inside the project directory and run the compilation command matching your operating system:

* **Windows**
```bash
  g++ main.cpp -o main -IC:\raylib\include -LC:\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm
