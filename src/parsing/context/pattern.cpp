#include "pattern.h"

#include <eigen3/Eigen/SVD>

#include <cstdint>

#include "../dashes.h"

detail::PatternExporter::PatternExporter(
    std::vector<std::vector<Vector>>& paths)
    : paths_{paths} {}

void detail::PatternExporter::plot(const std::vector<Vector>& polyline,
                                   const std::vector<double>& dasharray) {
    // Generate fewer lines in case the line is not dashed (to_dashes would
    // generate a line per line segment)
    if (dasharray.empty()) {
        paths_.push_back(polyline);
        return;
    }

    to_dashes(polyline, dasharray, [this](Vector start, Vector end) {
        paths_.push_back({start, end});
    });
}

std::vector<Vector> detail::compute_tiling_offsets(
    Vector pattern_size, const CoordinateSystem& coordinate_system,
    const std::vector<std::vector<Vector>>& clipping_path) {
    // We use a very simple approach to tiling here: In the coordinate system
    // it is defined in, the pattern is a rectangle located at (0, 0). We add an
    // additional scale, so that the size of the pattern is (1, 1). Then we take
    // the inverse of that and transform our clipping path into this coordinate
    // system. We build the bounding box of the clipping path and round it
    // outward to the nearest integer coordinates. Now we can just enumerate
    // all integer coordinates in the bounding box and transform them into
    // root space.
    //
    // An alternative approach that would not require inverting a matrix would
    // be to use the fact, that the affine transformation to root space can
    // only transform the rectangle into a parallelogram, and than doing
    // parallelogram tiling in root space.

    Transform to_root = coordinate_system.transform();
    to_root.scale(pattern_size);
    Transform from_root =
        to_root.inverse(Eigen::TransformTraits::AffineCompact);

    Rect bounding_box;
    for (auto& polyline : clipping_path) {
        for (auto point : polyline) {
            bounding_box.extend(from_root * point);
        }
    }

    std::vector<Vector> result;
    Vector base_point = to_root * Vector{0, 0};

    // int64_t can hold all reasonable values that the double coefficients can
    // have
    auto int_min_point = bounding_box.min().cast<std::int64_t>();
    auto int_max_point = bounding_box.max().cast<std::int64_t>();

    for (std::int64_t x = int_min_point(0); x <= int_max_point(0); x++) {
        for (std::int64_t y = int_min_point(1); y <= int_max_point(1); y++) {
            result.push_back(to_root * Vector{x, y} - base_point);
        }
    }

    return result;
}
