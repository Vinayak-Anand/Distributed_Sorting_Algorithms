#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
using namespace std;
// Structure for arguments
struct Arguments {
    vector<int>& arr;
    int n;
    int index;
    
    Arguments(vector<int>& a, int size, int idx) : arr(a), n(size), index(idx) {}
};

// Utility function to print the array
void printArray(const vector<int>& arr) {
    for (int i = 0; i < arr.size(); i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

// Comparison in each thread
void compare(Arguments args) {
    int index = args.index;
    int temp = args.arr[index];
    if ((index + 1 < args.n) && (args.arr[index + 1] < args.arr[index])) {
        args.arr[index] = args.arr[index + 1];
        args.arr[index + 1] = temp;
    }
}

// Main function performing the odd even transposition sort
double oddEvenTranspositionSort(vector<int>& arr) {
    auto start = chrono::high_resolution_clock::now();
    
    int n = arr.size();
    int max_threads = (n + 1) / 2;
    
    // For n rounds
    for (int i = 1; i <= n; i++) {
        vector<thread> threads;
        
        // Odd exchanges
        if (i % 2 == 1) {
            for (int j = 0, index = 0; j < max_threads; j++, index += 2) {
                Arguments args(arr, n, index);
                threads.push_back(thread(compare, args));
            }
        }
        // Even exchanges
        else {
            for (int j = 0, index = 1; j < max_threads - 1; j++, index += 2) {
                Arguments args(arr, n, index);
                threads.push_back(thread(compare, args));
            }
        }
        
        // Join all threads
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    return duration.count();
}

// Generate random array for testing
vector<int> generateRandomArray(int size) {
    vector<int> arr(size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, 1000);
    
    for (int i = 0; i < size; i++) {
        arr[i] = distrib(gen);
    }
    
    return arr;
}

// Verify if array is sorted
bool isSorted(const vector<int>& arr) {
    for (int i = 1; i < arr.size(); i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

// Run the algorithm with different sizes
void runOddEvenTranspositionSort() {
    vector<int> sizes = {10, 20, 30, 50};
    cout << "=== Odd-Even Transposition Sort ===" << endl;
    for (int size : sizes) {
        vector<int> arr = generateRandomArray(size);
        vector<int> arrCopy = arr;

        cout << "\nOriginal Array (Size " << size << "):" << endl;
        printArray(arr);
        
        double time = oddEvenTranspositionSort(arr);

        cout << "Sorted Array:" << endl;
        printArray(arr);

        bool sorted = isSorted(arr);
        
        // Verify correctness by comparing with standard sort
        sort(arrCopy.begin(), arrCopy.end());
        bool correctSort = arr == arrCopy;
        
        cout << "Size\tTime(ms)\tVerification" << endl;
    
        cout <<size << "\t" << time << " ms\t" 
                  << (sorted && correctSort ? "Correct" : "Incorrect") << endl;
    }
    cout << endl;
}


int main() {
    runOddEvenTranspositionSort();
    return 0;
}