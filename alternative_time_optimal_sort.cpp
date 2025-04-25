#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;
// Structure for arguments
struct Arguments {
    vector<int>& arr;
    int n;
    int center;
    
    Arguments(vector<int>& a, int size, int c) : arr(a), n(size), center(c) {}
};

// Utility function to print the array
void printArray(const vector<int>& arr) {
    for (int i = 0; i < arr.size(); i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

// Utility function to swap two numbers
void swap(vector<int>& arr, int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

// Comparison function used while performing an individual process
void compare(Arguments args) {
    // edge case
    if (args.center - 1 < 0) {
        if (args.arr[args.center] > args.arr[args.center + 1]) {
            swap(args.arr, args.center, args.center + 1);
        }
    }
    else if (args.center + 1 >= args.n) {
        if (args.arr[args.center] < args.arr[args.center - 1]) {
            swap(args.arr, args.center, args.center - 1);
        }
    }
    // non - edge case
    else {
        int minValue = min({args.arr[args.center], args.arr[args.center - 1], args.arr[args.center + 1]});
        int maxValue = max({args.arr[args.center], args.arr[args.center - 1], args.arr[args.center + 1]});
        int midValue = args.arr[args.center] + args.arr[args.center - 1] + args.arr[args.center + 1] - minValue - maxValue;
        args.arr[args.center - 1] = minValue;
        args.arr[args.center + 1] = maxValue;
        args.arr[args.center] = midValue;
    }
}

// Main function performing the alternate time optimal sorting
double alternateTimeOptimalSorting(vector<int>& arr) {
    auto start = chrono::high_resolution_clock::now();
    
    int n = arr.size();
    
    // For n - 1 rounds
    for (int i = 1; i < n; i++) {
        int remainder = (i + 1) % 3;
        int j;
        if (remainder == 0) {
            j = 2;
        }
        else if (remainder == 1) {
            j = 0;
        }
        else {
            j = 1;
        }
        
        vector<thread> threads;
        
        // For all centers possible at a distance of 3
        while (j < n) {
            Arguments args(arr, n, j);
            threads.push_back(thread(compare, args));
            // Incrementing by 3 to find the next center
            j += 3;
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
void runAlternateTimeOptimalSort() {
    vector<int> sizes = {10, 20, 30, 50};
    cout << "=== Alternate Time Optimal Sort ===" << endl;
    
    for (int size : sizes) {
        vector<int> arr = generateRandomArray(size);
        vector<int> arrCopy = arr;

        cout << "\nOriginal Array (size " << size << "):" << endl;
        printArray(arr);
        
        double time = alternateTimeOptimalSorting(arr);
        bool sorted = isSorted(arr);

        cout << "Sorted Array:" << endl;
        printArray(arr);
        
        // Verify correctness by comparing with standard sort
        sort(arrCopy.begin(), arrCopy.end());
        bool correctSort = arr == arrCopy;
        cout << "Size\tTime(ms)\tVerification" << endl;
        cout << size << "\t" << time << " ms\t" 
                  << (sorted && correctSort ? "Correct" : "Incorrect") << endl;
    }
    cout << endl;
}


int main() {
    runAlternateTimeOptimalSort();
    return 0;
}