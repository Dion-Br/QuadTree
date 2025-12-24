#include <iostream>
#include <random>
#include <vector>
#include <cassert>
#include "Quadtree.hpp"

// Unit Test 1: Normal Usage
void test_normal_usage() {
    std::cout << "[Test] Normal Usage: ";
    AxisAlignedBoundingBox boundary(0, 0, 100, 100);
    Quadtree<int> qt(boundary, 4);

    // Insert 10 items
    for(int i=0; i<10; ++i) {
        qt.insert(AxisAlignedBoundingBox(i*10, i*10, 5, 5), i);
    }

    // Query a region covering 2 items
    AxisAlignedBoundingBox query(0, 0, 18, 18); // Should catch (0,0) and (10,10)
    auto results = qt.query_region(query);

    assert(results.size() == 2);
    std::cout << "PASSED" << std::endl;
}

// Unit Test 2: Edge Case - Deduplication
void test_deduplication() {
    std::cout << "[Test] Deduplication (Object on boundary): ";

    // Quadtree split at (50, 50)
    AxisAlignedBoundingBox boundary(0, 0, 100, 100);
    Quadtree<int> qt(boundary, 1);

    // Insert item exactly in the center, overlapping all 4 children
    // Center is 50,50. Box at 45,45 with size 10x10 spans 45->55 on both axes.
    AxisAlignedBoundingBox centerBox(45, 45, 10, 10);
    qt.insert(centerBox, 999);

    // 1. Query Region Check
    auto results = qt.query_region(boundary);
    assert(results.size() == 1); // Should only be 1 item despite overlapping children

    // 2. Iterator Check
    int count = 0;
    for(auto it = qt.begin(); it != qt.end(); ++it) {
        count++;
    }
    assert(count == 1); // Iterator should also dedup

    std::cout << "PASSED" << std::endl;
}

// Unit Test 3: Edge Case - Out of Bounds
void test_out_of_bounds() {
    std::cout << "[Test] Out of Bounds Insert: ";
    AxisAlignedBoundingBox boundary(0, 0, 100, 100);
    Quadtree<int> qt(boundary, 4);

    // Insert item outside
    qt.insert(AxisAlignedBoundingBox(200, 200, 10, 10), 1);

    auto results = qt.query_region(boundary);
    assert(results.size() == 0);

    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "=== Running Unit Tests ===" << std::endl;
    test_normal_usage();
    test_deduplication();
    test_out_of_bounds();
    std::cout << "All Unit Tests Passed!" << std::endl << std::endl;

    // --- Original Demo Code ---
    std::cout << "=== Running Demo ===" << std::endl;
    AxisAlignedBoundingBox boundary(0, 0, 400, 400);
    Quadtree<int> qt(boundary, 4);

    std::cout << "Inserting data..." << std::endl;
    for (int i = 0; i < 50; ++i) {
        double x = rand() % 380;
        double y = rand() % 380;
        AxisAlignedBoundingBox box(x, y, 10, 10);
        qt.insert(box, i);
    }

    AxisAlignedBoundingBox queryBox(100, 100, 50, 50);
    auto results = qt.query_region(queryBox);
    std::cout << "Found " << results.size() << " items in region (100,100,50,50)." << std::endl;

    std::cout << "All items in tree:" << std::endl;
    int count = 0;
    for (auto it = qt.begin(); it != qt.end(); ++it) {
        count++;
    }
    std::cout << "Total items iterated: " << count << std::endl;

    return 0;
}