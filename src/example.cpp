#include <textures.hpp>
#include <classifier.hpp>

#include <iostream>
#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

int main() noexcept {
    using namespace boost;

    gil::rgb8_image_t img;
    gil::read_image("data/example_images/cat.jpeg", img, gil::jpeg_tag{});

    // gil::rgb8_image_t resized(img.width() / 2, img.height() / 2);

    // gil::resize_view(
    //     gil::const_view(img), 
    //     gil::view(resized), 
    //     gil::nearest_neighbor_sampler{});

    // gil::write_view(
    //     "data/example_images/cat_small.jpeg", 
    //     gil::const_view(resized), 
    //     gil::jpeg_tag{});

    gil::rgb8_image_t doubled(img.width() / 2, img.height());
    gil::rgb8_image_t resized(img.width() / 2, img.height() / 2);

    gil::resize_view(
        gil::const_view(img),
        gil::view(resized),
        gil::nearest_neighbor_sampler{});

    gil::copy_pixels(
        gil::const_view(resized),
        gil::subimage_view(
            gil::view(doubled), 
            0, 0, 
            doubled.width(), doubled.height() / 2));

    gil::copy_pixels(
        gil::rotated180_view(gil::const_view(resized)),
        gil::subimage_view(
            gil::view(doubled),
            0, doubled.height() / 2,
            doubled.width(), doubled.height() / 2));

    gil::write_view(
        "data/example_images/cat_doubled.jpeg", 
        gil::const_view(doubled), 
        gil::jpeg_tag{});

    // gil::rgb8_image_t a, b;

    // gil::read_and_convert_image(
    //     "data/block_textures/diamond_ore.png", a, gil::png_tag{});
    // gil::read_and_convert_image(
    //     "data/block_textures/diamond_block.png", b, gil::png_tag{});

    // std::cout << cf::compare(gil::const_view(a), gil::const_view(b)) 
    //           << std::endl;

    // gil::read_and_convert_image(
    //     "data/block_textures/tnt_bottom.png", a, gil::png_tag{});
    // gil::read_and_convert_image(
    //     "data/block_textures/redstone_ore.png", b, gil::png_tag{});

    // std::cout << cf::compare(gil::const_view(a), gil::const_view(b)) 
    //           << std::endl;
}