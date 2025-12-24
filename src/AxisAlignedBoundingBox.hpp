#ifndef AXIS_ALIGNED_BOUNDING_BOX_HPP
#define AXIS_ALIGNED_BOUNDING_BOX_HPP

class AxisAlignedBoundingBox {
public:
    double x, y, w, h;

    AxisAlignedBoundingBox(double x = 0, double y = 0, double w = 0, double h = 0)
        : x(x), y(y), w(w), h(h) {}

    friend bool collides(const AxisAlignedBoundingBox& one, const AxisAlignedBoundingBox& two) {
        return (one.x < two.x + two.w &&
                one.x + one.w > two.x &&
                one.y < two.y + two.h &&
                one.y + one.h > two.y);
    }
};
#endif