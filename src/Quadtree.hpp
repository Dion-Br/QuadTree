#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "AxisAlignedBoundingBox.hpp"
#include <vector>
#include <memory>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iterator>

// --- Quadtree Template ---
template <typename MetadataType>
class Quadtree {
public:
    // The actual object stored in the tree
    struct Element {
        AxisAlignedBoundingBox box;
        MetadataType data;

        bool operator==(const Element& other) const {
            return box.x == other.box.x && box.y == other.box.y &&
                   box.w == other.box.w && box.h == other.box.h &&
                   data == other.data;
        }
    };

    // Hasher for std::unordered_set
    struct ElementHasher {
        std::size_t operator()(const Element& e) const {
            return std::hash<double>{}(e.box.x) ^ (std::hash<double>{}(e.box.y) << 1);
        }
    };

    // --- Iterator Class (Snapshot) ---
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Element;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const Element*;
        using reference         = const Element&;

        Iterator() : current_index(0) {}

        explicit Iterator(std::vector<Element>&& data)
            : elements(std::make_shared<std::vector<Element>>(std::move(data))), current_index(0) {}

        reference operator*() const { return (*elements)[current_index]; }
        pointer operator->() const { return &(*elements)[current_index]; }

        Iterator& operator++() {
            current_index++;
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            bool this_exhausted = is_exhausted();
            bool other_exhausted = other.is_exhausted();
            if (this_exhausted && other_exhausted) return true;
            if (this_exhausted != other_exhausted) return false;
            return elements == other.elements && current_index == other.current_index;
        }

        bool operator!=(const Iterator& other) const { return !(*this == other); }

    private:
        std::shared_ptr<std::vector<Element>> elements;
        size_t current_index;
        bool is_exhausted() const { return !elements || current_index >= elements->size(); }
    };

private:
    AxisAlignedBoundingBox boundary;
    unsigned int capacity;
    unsigned int depth;
    unsigned int max_depth = 8;

    std::vector<Element> elements;
    bool divided = false;

    // Children
    std::unique_ptr<Quadtree> northeast;
    std::unique_ptr<Quadtree> northwest;
    std::unique_ptr<Quadtree> southeast;
    std::unique_ptr<Quadtree> southwest;

    // Helper to collect all unique elements recursively
    void collect_all_unique(std::unordered_set<Element, ElementHasher>& acc) const {
        for (const auto& el : elements) {
            acc.insert(el);
        }
        if (divided) {
            northeast->collect_all_unique(acc);
            northwest->collect_all_unique(acc);
            southeast->collect_all_unique(acc);
            southwest->collect_all_unique(acc);
        }
    }

    // NEW: Helper to collect boundaries of all nodes
    void collect_regions(std::vector<AxisAlignedBoundingBox>& acc) const {
        acc.push_back(boundary);
        if (divided) {
            northeast->collect_regions(acc);
            northwest->collect_regions(acc);
            southeast->collect_regions(acc);
            southwest->collect_regions(acc);
        }
    }

    void subdivide() {
        double x = boundary.x;
        double y = boundary.y;
        double w = boundary.w;
        double h = boundary.h;
        double hw = w / 2.0;
        double hh = h / 2.0;

        northeast = std::make_unique<Quadtree>(AxisAlignedBoundingBox(x + hw, y, hw, hh), capacity, depth + 1);
        northwest = std::make_unique<Quadtree>(AxisAlignedBoundingBox(x, y, hw, hh), capacity, depth + 1);
        southeast = std::make_unique<Quadtree>(AxisAlignedBoundingBox(x + hw, y + hh, hw, hh), capacity, depth + 1);
        southwest = std::make_unique<Quadtree>(AxisAlignedBoundingBox(x, y + hh, hw, hh), capacity, depth + 1);

        divided = true;
    }

public:
    Quadtree(const AxisAlignedBoundingBox& bounds, unsigned int region_capacity, unsigned int current_depth = 0)
        : boundary(bounds), capacity(region_capacity), depth(current_depth) {}

    void insert(const AxisAlignedBoundingBox& aabb, const MetadataType& meta) {
        if (!collides(boundary, aabb)) return;

        if (depth >= max_depth) {
            elements.push_back({aabb, meta});
            return;
        }

        if (!divided && elements.size() < capacity) {
            elements.push_back({aabb, meta});
            return;
        }

        if (!divided) {
            subdivide();
            auto old_elements = elements;
            elements.clear();

            for (const auto& el : old_elements) {
                bool placed = false;
                if (collides(northeast->boundary, el.box)) { northeast->insert(el.box, el.data); placed = true; }
                if (collides(northwest->boundary, el.box)) { northwest->insert(el.box, el.data); placed = true; }
                if (collides(southeast->boundary, el.box)) { southeast->insert(el.box, el.data); placed = true; }
                if (collides(southwest->boundary, el.box)) { southwest->insert(el.box, el.data); placed = true; }
                if (!placed) elements.push_back(el);
            }
        }

        bool placed = false;
        if (divided) {
            if (collides(northeast->boundary, aabb)) { northeast->insert(aabb, meta); placed = true; }
            if (collides(northwest->boundary, aabb)) { northwest->insert(aabb, meta); placed = true; }
            if (collides(southeast->boundary, aabb)) { southeast->insert(aabb, meta); placed = true; }
            if (collides(southwest->boundary, aabb)) { southwest->insert(aabb, meta); placed = true; }
        }
        if (!placed) elements.push_back({aabb, meta});
    }

    std::unordered_set<Element, ElementHasher> query_region(const AxisAlignedBoundingBox& search_area) const {
        std::unordered_set<Element, ElementHasher> result;
        if (!collides(boundary, search_area)) return result;

        for (const auto& el : elements) {
            if (collides(el.box, search_area)) result.insert(el);
        }

        if (divided) {
            auto ne = northeast->query_region(search_area);
            auto nw = northwest->query_region(search_area);
            auto se = southeast->query_region(search_area);
            auto sw = southwest->query_region(search_area);
            result.insert(ne.begin(), ne.end());
            result.insert(nw.begin(), nw.end());
            result.insert(se.begin(), se.end());
            result.insert(sw.begin(), sw.end());
        }
        return result;
    }

    // NEW: Public method to get all region boundaries
    std::vector<AxisAlignedBoundingBox> get_all_regions() const {
        std::vector<AxisAlignedBoundingBox> regions;
        collect_regions(regions);
        return regions;
    }

    Iterator begin() const {
        std::unordered_set<Element, ElementHasher> unique_items;
        collect_all_unique(unique_items);
        std::vector<Element> vec(unique_items.begin(), unique_items.end());
        return Iterator(std::move(vec));
    }

    Iterator end() const { return Iterator(); }
};

#endif