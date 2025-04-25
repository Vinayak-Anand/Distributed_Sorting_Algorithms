#include <iostream>
#include <thread>
#include <vector>
#include <climits>
#include <chrono>
#include <random>
#include <algorithm>
using namespace std;
// Structure to store the number value and whether it is marked
struct Element {
    int value;
    bool isMarked;
};

// Structure for a process node
struct Node {
    // Left and right values
    Element *lValue, *rValue;
    // Corresponding area
    int area;
    // Left and right neighbors
    Node *left;
    Node *right;
};

// Structure to store arguments
struct Arguments {
    Node *node;
    
    Arguments(Node* n) : node(n) {}
};

// Utility function to print the elements in order according to the rule based on area
vector<int> getSortedList(Node *root) {
    vector<int> result;
    Node *temp = root;
    while (temp != nullptr) {
        if (temp->area == -1) {
            result.push_back(temp->rValue->value);
        } else {
            result.push_back(temp->lValue->value);
        }
        temp = temp->right;
    }
    return result;
}

// Comparison function used in each process
void compare(Arguments args) {
    if (args.node->left != nullptr) {
        if (args.node->left->rValue->value > args.node->lValue->value) {
            // if marked element moves left, increase the area of the next one
            // if marked element moves right, decrease the area of the next one
            if (args.node->left->rValue->isMarked == true) {
                args.node->area--;
            }

            if (args.node->lValue->isMarked == true) {
                args.node->area++;
            }
            Element *temp = args.node->left->rValue;
            args.node->left->rValue = args.node->lValue;
            args.node->lValue = temp;
        }
    }

    if (args.node->right != nullptr) {
        if (args.node->right->lValue->value < args.node->rValue->value) {
            // if marked element moves left, increase the area of the next one
            // if marked element moves right, decrease the area of the next one
            if (args.node->right->lValue->isMarked == true) {
                args.node->right->area++;
            }

            if (args.node->rValue->isMarked == true) {
                args.node->right->area--;
            }
            Element *temp = args.node->right->lValue;
            args.node->right->lValue = args.node->rValue;
            args.node->rValue = temp;
        }
    }

    if (args.node->lValue->value > args.node->rValue->value) {
        Element *temp = args.node->lValue;
        args.node->lValue = args.node->rValue;
        args.node->rValue = temp;
    }
}

// Main function performing sasaki time optimal algorithm
double sasakiTimeOptimalSort(vector<int>& arr, vector<int>& result) {
    auto start = chrono::high_resolution_clock::now();
    
    int n = arr.size();
    Node *prev = nullptr, *root = nullptr;
    
    // Initialization of process nodes in the linked list
    for (int i = 0; i < n; i++) {
        Node *node = new Node();
        if (i == 0) {
            Element *lElement = new Element();
            lElement->value = INT_MIN;
            lElement->isMarked = false;
            Element *rElement = new Element();
            rElement->value = arr[i];
            rElement->isMarked = true;
            node->lValue = lElement;
            node->rValue = rElement;
            node->area = -1;
            root = node;
        } else if (i == n - 1) {
            Element *lElement = new Element();
            lElement->value = arr[i];
            lElement->isMarked = true;
            Element *rElement = new Element();
            rElement->value = INT_MAX;
            rElement->isMarked = false;
            node->lValue = lElement;
            node->rValue = rElement;
            node->area = 0;
        } else {
            Element *lElement = new Element();
            lElement->value = arr[i];
            lElement->isMarked = false;
            Element *rElement = new Element();
            rElement->value = arr[i];
            rElement->isMarked = false;
            node->lValue = lElement;
            node->rValue = rElement;
            node->area = 0;
        }
        node->left = prev;
        node->right = nullptr;
        if (prev != nullptr) {
            prev->right = node;
        }
        prev = node;
    }
    
    Node *temp = root;

    // For n - 1 rounds
    for (int i = 1; i < n; i++) {
        vector<thread> threads;
        temp = root;
        
        for (int j = 0; j < n; j++) {
            Arguments args(temp);
            threads.push_back(thread(compare, args));
            temp = temp->right;
        }
        
        // Join all threads
        for (auto& thread : threads) {
            thread.join();
        }
    }
    
    // Get sorted result
    result = getSortedList(root);
    
    // Cleanup memory
    temp = root;
    while (temp != nullptr) {
        Node* nextNode = temp->right;
        delete temp->lValue;
        delete temp->rValue;
        delete temp;
        temp = nextNode;
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
// Utility function to print the array
void printArray(const vector<int>& arr) {
    for (int val : arr) {
        cout << val << " ";
    }
    cout << endl;
}


// Run the algorithm with different sizes
void runSasakiTimeOptimalSort() {
    vector<int> sizes = {10, 20, 30, 50};
    cout << "=== Sasaki Time Optimal Sort ===" << endl;
    
    
    for (int size : sizes) {
        vector<int> arr = generateRandomArray(size);
        vector<int> arrCopy = arr;
        vector<int> result;

        cout << "\nOriginal Array (size " << size << "):" << endl;
        printArray(arr);
        
        double time = sasakiTimeOptimalSort(arr, result);
        bool sorted = isSorted(result);

        cout << "Sorted Array:" << endl;
        printArray(result);
        
        // Verify correctness by comparing with standard sort
        sort(arrCopy.begin(), arrCopy.end());
        bool correctSort = result == arrCopy;
        cout << "Size\tTime(ms)\tVerification" << endl;
        
        cout << size << "\t" << time << " ms\t" 
                  << (sorted && correctSort ? "Correct" : "Incorrect") << endl;
    }
    cout << endl;
}


int main() {
    runSasakiTimeOptimalSort();
    return 0;
}