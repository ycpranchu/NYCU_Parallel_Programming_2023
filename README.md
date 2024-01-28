# NYCU Parallel Programming, 2023 Fall

> [!NOTE]
> `Final project and report are included.`

- 📖 Parallel Programming, 2023 Fall, NYCU
- 🧑‍🏫 You, Yi-Ping

Assignments
---

- `1` SIMD Programming - https://hackmd.io/@ycpin/S1qliFGM6
- `2` Multi-thread Programming - https://hackmd.io/@ycpin/SJ0kFr0Qp
- `3` OpenMP Programming
- `4` MPI Programming
- `5` CUDA Programming
- `6` OpenCL Programming - https://hackmd.io/@ycpin/HyGnBoxE6

Final Project
---

🚀 Parallel Partitioning and Floorplanning System

- [Presentation Slide](https://docs.google.com/presentation/d/12aIONbtD71gFwsM7nDYbnz69dS76CbyMq_cxv4zSFq8/edit?usp=sharing)

### Compile

```bash
make clean
make
```

### Generate Testcase

```bash
cd testcase

# set parameters & generate module list
python3 case.py

# set parameters & generate netlist
python3 connect.py
```

### How to Use

```bash
# ./PPF <path_to_case_file> <path_to_connect_file>
# set the number of threads in Plan.h
# set the iterations times and p_bound in main.h

./PPF testcase/case_1000.in testcase/connect_1000.in
```

### Check the Results

```bash
# set the path of target floorplanning file in check_img.py
python3 check_img.py
```
