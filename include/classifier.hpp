#ifndef CLASSIFIER_HPP
#define CLASSIFIER_HPP

#include <iostream>
#include <vector>
#include <cassert>
#include <memory>

#include <boost/gil.hpp>

namespace cf {

    using namespace boost;

    const std::uint_fast32_t bucket_size = 8;
    const std::uint_fast32_t buckets = 256 / bucket_size;

    using __color_histogram =
        std::vector<std::vector<std::vector<std::uint_fast32_t>>>;

    std::shared_ptr<__color_histogram> make_histogram() {
        return std::shared_ptr<__color_histogram>(
            new __color_histogram(
                buckets,
                std::vector<std::vector<std::uint_fast32_t>>(
                    buckets,
                    std::vector<std::uint_fast32_t>(
                        buckets, 
                        0))));
    }

    class color_histogram {
        private:
            std::shared_ptr<__color_histogram> data = make_histogram();
        public:
            auto& operator[](std::uint_fast32_t idx) noexcept {
                return (*data)[idx];
            }
    };

    color_histogram calculate_histogram(gil::rgb8c_view_t view) noexcept {
        color_histogram hist;

        for (int x = 0; x < view.width(); ++x)
            for (int y = 0; y < view.height(); ++y) {
                auto p = view(x, y);
                ++hist[p[0] / bucket_size]
                      [p[1] / bucket_size]
                      [p[2] / bucket_size];
            }

        return hist;
    }
    
    
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