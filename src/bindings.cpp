#include <emscripten/bind.h>
#include "Quadtree.hpp"
#include <vector>

using namespace emscripten;

using QT = Quadtree<int>;
using Element = QT::Element;

std::vector<Element> query_helper(QT& tree, const AxisAlignedBoundingBox& box) {
    auto set = tree.query_region(box);
    return std::vector<Element>(set.begin(), set.end());
}

std::vector<Element> get_all_helper(QT& tree) {
    std::vector<Element> all;
    for(auto it = tree.begin(); it != tree.end(); ++it) {
        all.push_back(*it);
    }
    return all;
}

EMSCRIPTEN_BINDINGS(quadtree_module) {
    value_object<AxisAlignedBoundingBox>("AxisAlignedBoundingBox")
        .field("x", &AxisAlignedBoundingBox::x)
        .field("y", &AxisAlignedBoundingBox::y)
        .field("w", &AxisAlignedBoundingBox::w)
        .field("h", &AxisAlignedBoundingBox::h);

    value_object<Element>("Element")
        .field("box", &Element::box)
        .field("data", &Element::data);

    register_vector<Element>("VectorElement");

    // NEW: Register vector for Bounding Boxes so we can return the regions
    register_vector<AxisAlignedBoundingBox>("VectorBox");

    class_<QT>("Quadtree")
        .constructor<AxisAlignedBoundingBox, unsigned int>()
        .function("insert", &QT::insert)
        .function("query_region", &query_helper)
        .function("getAll", &get_all_helper)
        // NEW: Expose the function to get all node boundaries
        .function("getAllRegions", &QT::get_all_regions);
}