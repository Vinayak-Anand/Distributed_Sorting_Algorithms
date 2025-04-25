# README.txt

## Parallel Sorting Algorithms in C++

This project contains C++ implementations of three parallel sorting algorithms using threads. Each algorithm takes a different approach to sorting in a distributed-like environment, making use of multi-threading to simulate parallelism. Here's what's included:

1. Odd-Even Transposition Sort
2. Sasaki's Time Optimal Sort
3. Alternate Time Optimal Sort (Median-based)

Each algorithm is in its own file with code to generate random arrays, measure execution time, and verify correctness. 

---

1. Odd-Even Transposition Sort

---

File: odd_even_transposition_sort.cpp

Description:

- This algorithm sorts by alternating between odd and even indexed comparisons, swapping adjacent elements if they're out of order.
- Uses C++ threads to handle comparisons in parallel, mimicking a distributed system.

Key Features:

- Runs in n rounds, switching between odd and even phases.
- Each round uses up to ceiling(n/2) threads for comparisons.

Time Complexity: O(n) with O(n) processors (threads).

- Each of the n rounds performs O(n) comparisons in parallel, making it linear in time with enough processors.

Space Complexity: O(n) for the input array and O(1) additional space (excluding thread overhead).

- Uses a single vector for the array and a small SortArgs struct per thread.

Data Structure:

- Vector (vector): Stores the input array, allowing fast random access (O(1)) for comparisons and swaps. Chosen for simplicity and cache-friendly sequential access.
- SortArgs struct: Bundles the array reference, size, and index for each thread. Minimal memory footprint, reused across rounds.

Number of Rounds: Exactly n rounds (n odd and even phases combined).

---

2. Sasaki's Time Optimal Sort

---

File: sasaki_time_optimal_sort.cpp

Description:

- Implements Sasaki's time-optimal algorithm, designed for distributed systems.
- Models the array as a linked list of process nodes, each holding two values (left and right) and an "area" to track element movement.
- Threads simulate parallel comparisons and swaps across nodes.

Key Features:

- Optimized for minimal rounds (n-1) in a parallel model.
- Handles marked/unmarked elements to guide sorting decisions.
- Uses a linked list to represent distributed processes.

Time Complexity: O(n) with O(n) processors.

- Runs n-1 rounds, each performing O(n) comparisons in parallel.
- Constant-time operations per node (comparisons and swaps).

Space Complexity: O(n) for the linked list nodes and elements.

- Each of n nodes stores two Element structs (value and mark) and pointers to neighbors.
- Additional O(n) for the output array.

Data Structure:

- Linked List of Nodes: Each Node has left/right Element structs and pointers to neighbors. Chosen to model distributed processes with local communication (left/right neighbors).
- Element struct: Stores a value and a boolean mark, enabling Sasaki's marking-based logic.
- Linked list allows flexible node connections but sacrifices cache locality compared to arrays.

Number of Rounds: Exactly n-1 rounds.

---

3. Alternate Time Optimal Sort (Median-based)

---

File: alternate_time_optimal_sort.cpp

Description:

- A custom parallel sorting algorithm that processes elements in groups of three, sorting them by min, mid, and max values.
- Uses a cyclic pattern (based on round number mod 3) to select "center" indices for comparisons.
- Threads handle each group of three elements in parallel.

Key Features:

- Unique approach using three-way comparisons to reduce communication steps.
- Scales well with parallelism due to independent group processing.

Time Complexity: O(n) with O(n) processors.

- Runs n-1 rounds, each processing up to ceiling(n/3) groups in parallel.
- Each group involves constant-time min/max/mid calculations.
- Note: The algorithm's theoretical complexity could approach O(log^2 n) in optimized distributed models with median partitioning, but this implementation is O(n) due to the fixed n-1 rounds.

Space Complexity: O(n) for the input array and O(1) additional space (excluding threads).

- Uses a single vector and a small SortArgs struct per thread.
- No additional data structures beyond thread arguments.

Data Structure:

- Vector (vector): Stores the input array, enabling fast O(1) access for three-way comparisons. Chosen for simplicity and cache efficiency.
- SortArgs struct: Passes array reference, size, and center index to threads. Lightweight and reused across rounds.

Number of Rounds: Exactly n-1 rounds, driven by the cyclic three-way comparison pattern.

---

## How to Compile and Run

Each file is self-contained and requires a C++11-compatible compiler with POSIX threading support (e.g., g++). Here's how to compile and run:

g++ -std=c++11 -pthread odd_even_transposition_sort.cpp -o odd_even_sort 
g++ -std=c++11 -pthread sasaki_time_optimal_sort.cpp -o sasaki_sort 
g++ -std=c++11 -pthread alternate_time_optimal_sort.cpp -o median_sort
g++ -std=c++11 -pthread comparison_program.cpp -o comparison

Then run each program:

./odd_even_sort 
./sasaki_sort 
./median_sort
./comparison

The programs test arrays of sizes 10, 20, 30, and 50, but you can modify the sizes vector in each file to experiment with other sizes.

---

## Testing and Output

Each program:

- Generates random arrays (values between 1 and 1000).
- Prints the original and sorted arrays.
- Measures execution time in milliseconds.
- Verifies correctness by checking if the output is sorted and matches std::sort.

Sample Output (for each size):

- Original array
- Sorted array
- Table with size, time (ms), and verification ("Correct" or "Incorrect")

Comparison program
  - Run the comparison program to get the results of all the three algorithms on n=10,20,30 and 50
  - Alongside the time taken by each algorithm on the inputs

---


## Implementation Notes

- Threading: C++ threads simulate parallel processors. The actual performance depends on your system's thread scheduling and CPU cores.
- Random Input: The generateRandomArray function uses std::mt19937 for reproducible random numbers. Adjust the range in uniform_int_distribution if needed.
- Memory Management:
  - Odd-Even and Alternate sorts use minimal extra memory, relying on the input vector.
  - Sasaki's sort allocates O(n) nodes and elements, which are cleaned up after sorting.
- Correctness: Each algorithm is verified against std::sort and checked for sorted order to ensure reliability.
- Data Structure Choices:
  - vector is used in Odd-Even and Alternate sorts for fast access and simplicity, ideal for array-based algorithms.
  - Sasaki's linked list of nodes reflects its distributed model, prioritizing logical structure over cache performance.

---

## Author

Name: Vinayak Anand
Roll Number: S20220010243
