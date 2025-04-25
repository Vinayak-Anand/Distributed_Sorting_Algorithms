#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <climits>
#include <mutex>
#include <memory>
using namespace std;

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

// Quick way to display an array
void printArray(const vector<int>& arr) {
    for (int val : arr) {
        cout << val << " ";
    }
    cout << endl;
}

// ----- Odd-Even Transposition Sort Algorithm -----
struct OEArguments {
    vector<int>& arr;
    int n;
    int index;
    mutex& mtx;
    
    OEArguments(vector<int>& a, int size, int idx, mutex& m) 
        : arr(a), n(size), index(idx), mtx(m) {}
};

void oeCompare(OEArguments args) {
    int index = args.index;
    
    // Use mutex to protect the comparison and swap operation
    lock_guard<mutex> lock(args.mtx);
    if ((index + 1 < args.n) && (args.arr[index + 1] < args.arr[index])) {
        int temp = args.arr[index];
        args.arr[index] = args.arr[index + 1];
        args.arr[index + 1] = temp;
    }
}

double oddEvenTranspositionSort(vector<int>& arr) {
    auto start = chrono::high_resolution_clock::now();
    
    int n = arr.size();
    int max_threads = (n + 1) / 2;
    mutex mtx;
    
    // For n rounds
    for (int i = 1; i <= n; i++) {
        vector<thread> threads;
        
        // Odd exchanges
        if (i % 2 == 1) {
            for (int j = 0, index = 0; j < max_threads; j++, index += 2) {
                if (index + 1 >= n) continue; // Skip if out of bounds
                OEArguments args(arr, n, index, mtx);
                threads.push_back(thread(oeCompare, args));
            }
        }
        // Even exchanges
        else {
            for (int j = 0, index = 1; j < max_threads - 1; j++, index += 2) {
                if (index + 1 >= n) continue; // Skip if out of bounds
                OEArguments args(arr, n, index, mtx);
                threads.push_back(thread(oeCompare, args));
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

// ----- Sasaki's Time Optimal Sort Algorithm -----
struct Element {
    int value;
    bool isMarked;
};

struct Node {
    unique_ptr<Element> lValue, rValue;
    int area;
    Node *left;
    Node *right;
    
    Node() : left(nullptr), right(nullptr), area(0) {}
    ~Node() = default; // Let unique_ptr handle cleanup
};

struct SArguments {
    Node *node;
    mutex& mtx;
    
    SArguments(Node* n, mutex& m) : node(n), mtx(m) {}
};

vector<int> getSortedList(Node *root, size_t size) {
    vector<int> result(size);
    Node *temp = root;
    int i = 0;
    
    while (temp != nullptr && i < size) {
        if (temp->area == -1) {
            result[i++] = temp->rValue->value;
        } else {
            result[i++] = temp->lValue->value;
        }
        temp = temp->right;
    }
    
    return result;
}

void sasakiCompare(SArguments args) {
    lock_guard<mutex> lock(args.mtx);
    
    if (args.node->left != nullptr) {
        if (args.node->left->rValue->value > args.node->lValue->value) {
            if (args.node->left->rValue->isMarked == true) {
                args.node->area--;
            }
            if (args.node->lValue->isMarked == true) {
                args.node->area++;
            }
            
            // Swap Elements safely using C++11 compatible code
            unique_ptr<Element> tempElement(new Element());
            tempElement->value = args.node->left->rValue->value;
            tempElement->isMarked = args.node->left->rValue->isMarked;
            
            args.node->left->rValue->value = args.node->lValue->value;
            args.node->left->rValue->isMarked = args.node->lValue->isMarked;
            
            args.node->lValue->value = tempElement->value;
            args.node->lValue->isMarked = tempElement->isMarked;
        }
    }
    
    if (args.node->right != nullptr) {
        if (args.node->right->lValue->value < args.node->rValue->value) {
            if (args.node->right->lValue->isMarked == true) {
                args.node->right->area++;
            }
            if (args.node->rValue->isMarked == true) {
                args.node->right->area--;
            }
            
            // Swap Elements safely using C++11 compatible code
            unique_ptr<Element> tempElement(new Element());
            tempElement->value = args.node->right->lValue->value;
            tempElement->isMarked = args.node->right->lValue->isMarked;
            
            args.node->right->lValue->value = args.node->rValue->value;
            args.node->right->lValue->isMarked = args.node->rValue->isMarked;
            
            args.node->rValue->value = tempElement->value;
            args.node->rValue->isMarked = tempElement->isMarked;
        }
    }
    
    if (args.node->lValue->value > args.node->rValue->value) {
        // Swap Elements safely using C++11 compatible code
        unique_ptr<Element> tempElement(new Element());
        tempElement->value = args.node->lValue->value;
        tempElement->isMarked = args.node->lValue->isMarked;
        
        args.node->lValue->value = args.node->rValue->value;
        args.node->lValue->isMarked = args.node->rValue->isMarked;
        
        args.node->rValue->value = tempElement->value;
        args.node->rValue->isMarked = tempElement->isMarked;
    }
}

double sasakiTimeOptimalSort(vector<int>& arr, vector<int>& result) {
    auto start = chrono::high_resolution_clock::now();
    
    int n = arr.size();
    vector<unique_ptr<Node>> nodeList; // Store all nodes to ensure proper cleanup
    nodeList.reserve(n);
    
    Node *prev = nullptr, *root = nullptr;
    mutex mtx;
    
    // Initialization of process nodes in the linked list
    for (int i = 0; i < n; i++) {
        unique_ptr<Node> uniqueNode(new Node());
        Node* node = uniqueNode.get();
        
        if (i == 0) {
            // First node
            node->lValue.reset(new Element());
            node->lValue->value = INT_MIN;
            node->lValue->isMarked = false;
            
            node->rValue.reset(new Element());
            node->rValue->value = arr[i];
            node->rValue->isMarked = true;
            
            node->area = -1;
            root = node;
        } else if (i == n - 1) {
            // Last node
            node->lValue.reset(new Element());
            node->lValue->value = arr[i];
            node->lValue->isMarked = true;
            
            node->rValue.reset(new Element());
            node->rValue->value = INT_MAX;
            node->rValue->isMarked = false;
            
            node->area = 0;
        } else {
            // Middle nodes
            node->lValue.reset(new Element());
            node->lValue->value = arr[i];
            node->lValue->isMarked = true;
            
            node->rValue.reset(new Element());
            node->rValue->value = arr[i];
            node->rValue->isMarked = false;
            
            node->area = 0;
        }
        
        node->left = prev;
        node->right = nullptr;
        
        if (prev != nullptr) {
            prev->right = node;
        }
        
        prev = node;
        nodeList.push_back(move(uniqueNode));
    }
    
    // For n - 1 rounds
    for (int i = 1; i < n; i++) {
        vector<thread> threads;
        Node *temp = root;
        
        for (int j = 0; j < n && temp != nullptr; j++) {
            SArguments args(temp, mtx);
            threads.push_back(thread(sasakiCompare, args));
            temp = temp->right;
        }
        
        // Join all threads
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
    // Get sorted result
    result = getSortedList(root, n);
    
    // Memory cleanup is handled by unique_ptr
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    return duration.count();
}

// ----- Alternate Time Optimal Sort Algorithm -----
struct AArguments {
    vector<int>& arr;
    int n;
    int center;
    mutex& mtx;
    
    AArguments(vector<int>& a, int size, int c, mutex& m) 
        : arr(a), n(size), center(c), mtx(m) {}
};

void swap(vector<int>& arr, int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void alternateCompare(AArguments args) {
    lock_guard<mutex> lock(args.mtx);
    
    // edge case
    if (args.center - 1 < 0) {
        if (args.center + 1 < args.n && args.arr[args.center] > args.arr[args.center + 1]) {
            swap(args.arr, args.center, args.center + 1);
        }
    }
    else if (args.center + 1 >= args.n) {
        if (args.center - 1 >= 0 && args.arr[args.center] < args.arr[args.center - 1]) {
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

double alternateTimeOptimalSorting(vector<int>& arr) {
    auto start = chrono::high_resolution_clock::now();
    
    int n = arr.size();
    mutex mtx;
    
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
            AArguments args(arr, n, j, mtx);
            threads.push_back(thread(alternateCompare, args));
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

// ----- Main Comparison Function -----
int main() {
    vector<int> sizes = {10, 20, 30, 50};
    
    cout << "==== Comparison of Distributed Sorting Algorithms ====" << endl << endl;
    
    cout << left << setw(10) << "Size" 
         << setw(25) << "Odd-Even (ms)" 
         << setw(25) << "Sasaki (ms)" 
         << setw(25) << "Alternative (ms)" << endl;
    
    cout << string(85, '-') << endl;
    
    for (int size : sizes) {
        // Generate the same random array for all algorithms
        vector<int> arr = generateRandomArray(size);
        
        // Print the random array
        cout << "Random Array (Size " << size << "):" << endl;
        printArray(arr);
        cout << endl;
        
        // Test Odd-Even Transposition Sort
        vector<int> arr1 = arr;
        double time1 = oddEvenTranspositionSort(arr1);
        bool sorted1 = isSorted(arr1);
        
        // Test Sasaki's Time Optimal Sort
        vector<int> arr2 = arr;
        vector<int> result2;
        double time2 = sasakiTimeOptimalSort(arr2, result2);
        bool sorted2 = isSorted(result2);
        
        // Test Alternative Time Optimal Sort
        vector<int> arr3 = arr;
        double time3 = alternateTimeOptimalSorting(arr3);
        bool sorted3 = isSorted(arr3);
        
        cout << left << setw(10) << size 
             << setw(25) << fixed << setprecision(3) << time1 
             << setw(25) << time2 
             << setw(25) << time3  << endl;
    }
    
    // Analysis
    cout << endl << "==== Analysis ====" << endl;
    cout << "1. Odd-Even Transposition Sort: O(n) time complexity, requires n rounds." << endl;
    cout << "2. Sasaki's Time Optimal Sort: O(n) time complexity with optimized communication, requires n-1 rounds." << endl;
    cout << "3. Alternative Time Optimal Sort: O(n) time complexity using local neighborhood operations." << endl;
    
    cout << endl << "The time-optimal algorithms (Sasaki's and Alternative) aim to reduce the" << endl;
    cout << "total number of communication rounds required compared to the basic Odd-Even sort." << endl;
    cout << "Sasaki's algorithm uses a more complex node structure and area marking strategy," << endl;
    cout << "while the Alternative algorithm uses triplet-based comparisons in a modulo-3 pattern." << endl;
    
    return 0;
}