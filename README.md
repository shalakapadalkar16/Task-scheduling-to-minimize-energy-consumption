# ⚡ Energy & Performance-Aware Task Scheduling in Mobile Cloud Computing (C++)

This project implements an energy-efficient task scheduler for Mobile Cloud Computing (MCC). It determines whether tasks should run locally on a mobile device or be offloaded to the cloud—while maintaining strict deadline constraints. By applying DAG-based scheduling logic inspired by a modified HEFT algorithm and energy-aware migration, the system demonstrates significant reduction in mobile energy consumption without sacrificing performance.

---

## 🚀 Technical Highlights
- Developed in **C++** for performance efficiency
- **Directed Acyclic Graph (DAG)**-based task representation
- Considers:
  - Local execution energy
  - Cloud execution latency
  - Wireless transmission overhead
- Ensures **deadline-aware scheduling decisions**
- Prints final assignment of each task (mobile vs. cloud) and resulting energy metrics

---

## 🧠 Core Algorithm (Research-Based)
This implementation is adapted from:
> Lin, Wang, Xie & Pedram,  
> *Energy and performance-aware task scheduling in a mobile cloud computing environment*,  
> IEEE Transactions on Cloud Computing.

Scheduling logic includes:
- Calculating earliest completion and slack time
- Identifying beneficial offloading opportunities
- Reducing mobile energy usage while meeting timing constraints

**Full credit to the authors for the foundational contribution.**

---

## 🏗️ Project Structure

📂 mcc-task-scheduling
├── taskscheduler.cpp # Main scheduling logic implementation (C++)
└── taskscheduler.exe # Compiled executable (Windows)


---

## 🔧 How to Build & Run

### ▶️ Compile (Linux/Mac)
```bash
g++ taskscheduler.cpp -o taskscheduler
```

▶️ Run
```bash
./taskscheduler
```
On Windows, you may run taskscheduler.exe directly.

📊 Output

Prints calculated energy usage

Indicates whether each task should run on:

📱 Local device

☁️ Cloud server

Shows deadline feasibility results

📚 Academic Background & Credits

This project is based on concepts from the publication:

J. Lin, Y. Wang, X. Xie, and M. Pedram,
“Energy and performance-aware task scheduling in a mobile cloud computing environment,”
IEEE Transactions on Cloud Computing.

All scheduling logic, performance constraints, and energy modeling are derived from the authors' methodology.
Full credit to the authors for their foundational contribution.
