#include <iostream>
#include <cassert>
#include "Quadtree.hpp"

// === collides() Tests ===
void test_collides_normal() {
    std::cout << "[Test] collides() - normal: ";
    AxisAlignedBoundingBox a(0, 0, 10, 10), b(5, 5, 10, 10);
    assert(collides(a, b) == true);
    std::cout << "PASSED\n";
}

void test_collides_edge() {
    std::cout << "[Test] collides() - edge (no overlap): ";
    AxisAlignedBoundingBox a(0, 0, 10, 10), b(10, 0, 10, 10);
    assert(collides(a, b) == false);
    std::cout << "PASSED\n";
}

// === Constructor Tests ===
void test_constructor_normal() {
    std::cout << "[Test] Constructor - normal: ";
    Quadtree<int> qt(AxisAlignedBoundingBox(0, 0, 100, 100), 4);
    assert(qt.begin() == qt.end()); // Empty tree
    std::cout << "PASSED\n";
}

void test_constructor_edge() {
    std::cout << "[Test] Constructor - capacity 1: ";
    Quadtree<int> qt(AxisAlignedBoundingBox(0, 0, 100, 100), 1);
    qt.insert(AxisAlignedBoundingBox(10, 10, 5, 5), 1);
    qt.insert(AxisAlignedBoundingBox(60, 60, 5, 5), 2);
    assert(qt.query_region(AxisAlignedBoundingBox(0, 0, 100, 100)).size() == 2);
    std::cout << "PASSED\n";
}

// === insert() Tests ===
void test_insert_normal() {
    std::cout << "[Test] insert() - normal: ";
    Quadtree<int> qt(AxisAlignedBoundingBox(0, 0, 100, 100), 4);
    qt.insert(AxisAlignedBoundingBox(10, 10, 5, 5), 42);
    assert(qt.query_region(AxisAlignedBoundingBox(0, 0, 100, 100)).size() == 1);
    std::cout << "PASSED\n";
}

void test_insert_edge() {
    std::cout << "[Test] insert() - outside bounds: ";
    Quadtree<int> qt(AxisAlignedBoundingBox(0, 0, 100, 100), 4);
    qt.insert(AxisAlignedBoundingBox(200, 200, 10, 10), 1);
    assert(qt.query_region(AxisAlignedBoundingBox(0, 0, 100, 100)).size() == 0);
    std::cout << "PASSED\n";
}

// === query_region() Tests ===
void test_query_normal() {
    std::cout << "[Test] query_region() - normal: ";
    Quadtree<int> qt(AxisAlignedBoundingBox(0, 0, 100, 100), 4);
    qt.insert(AxisAlignedBoundingBox(10, 10, 5, 5), 1);
    qt.insert(AxisAlignedBoundingBox(80, 80, 5, 5), 2);
    assert(qt.query_region(AxisAlignedBoundingBox(0, 0, 30, 30)).size() == 1);
    std::cout << "PASSED\n";
}

void test_query_edge() {
    std::cout << "[Test] query_region() - deduplication: ";
    Quadtree<int> qt(AxisAlignedBoundingBox(0, 0, 100, 100), 1);
    qt.insert(AxisAlignedBoundingBox(45, 45, 10, 10), 999); // Spans quadrants
    assert(qt.query_region(AxisAlignedBoundingBox(0, 0, 100, 100)).size() == 1);
    std::cout << "PASSED\n";
}

// === Iterator Tests ===
void test_iterator_normal() {
    std::cout << "[Test] begin()/end() - normal: ";
    Quadtree<int> qt(AxisAlignedBoundingBox(0, 0, 100, 100), 4);
    qt.insert(AxisAlignedBoundingBox(10, 10, 5, 5), 1);
    qt.insert(AxisAlignedBoundingBox(50, 50, 5, 5), 2);
    int count = 0;
    for (auto it = qt.begin(); it != qt.end(); ++it) count++;
    assert(count == 2);
    std::cout << "PASSED\n";
}

void test_iterator_edge() {
    std::cout << "[Test] begin()/end() - empty tree: ";
    Quadtree<int> qt(AxisAlignedBoundingBox(0, 0, 100, 100), 4);
    int count = 0;
    for (auto it = qt.begin(); it != qt.end(); ++it) count++;
    assert(count == 0);
    assert(qt.begin() == qt.end());
    std::cout << "PASSED\n";
}

int main() {
    std::cout << "=== Quadtree Unit Tests ===\n";
    test_collides_normal();
    test_collides_edge();
    test_constructor_normal();
    test_constructor_edge();
    test_insert_normal();
    test_insert_edge();
    test_query_normal();
    test_query_edge();
    test_iterator_normal();
    test_iterator_edge();
    std::cout << "=== All 10 Tests Passed! ===\n";
    return 0;
}