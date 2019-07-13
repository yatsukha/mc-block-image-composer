#ifndef CLASSIFIER_HPP
#define CLASSIFIER_HPP

#include <iostream>
#include <cassert>

#include <boost/gil.hpp>

namespace cf {

    using namespace boost;

    const std::uint_fast32_t bucket_size = 8;
    const std::uint_fast32_t buckets = 256 / bucket_size;
    
    double compare(gil::rgb8c_view_t l, gil::rgb8c_view_t r) noexcept {
        // mem :^)
        assert(buckets <= 64);

        std::uint_fast32_t l_hist[buckets][buckets][buckets]{0},
                           r_hist[buckets][buckets][buckets]{0};

        assert(l.width() == r.width() && l.height() == r.height());

        for (int x = 0; x < l.width(); ++x)
            for (int y = 0; y < l.height(); ++y) {
                gil::rgb8_pixel_t l_p = l(x, y),
                                  r_p = r(x, y);

                ++l_hist[l_p[0] / bucket_size]
                        [l_p[1] / bucket_size]
                        [l_p[2] / bucket_size];

                ++r_hist[r_p[0] / bucket_size]
                        [r_p[1] / bucket_size]
                        [r_p[2] / bucket_size];
            }

        std::uint_fast32_t sum = 0;

        for (int i = 0; i < buckets; ++i)
            for (int j = 0; j < buckets; ++j)
                for (int k = 0; k < buckets; ++k)
                    sum += std::min(l_hist[i][j][k], r_hist[i][j][k]);

        return (double) sum / (l.width() * l.height());
    }

}

#endif