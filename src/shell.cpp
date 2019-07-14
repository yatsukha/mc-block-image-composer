#include <iostream>
#include <string>

#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include <boost/algorithm/string.hpp>

#include <textures.hpp>
#include <classifier.hpp>

int main() noexcept {
    namespace gil = boost::gil;
    namespace str = boost::algorithm;

    const std::string prompt = "> ";

    auto textures = tx::load_textures("data/block_textures/");
    std::cout << textures.size() << " block textures loaded." << std::endl;

    std::cout << "Enter the path of the image to compose (png/jpg/jpeg)." 
              << std::endl;

    std::string answer;

    std::cout << prompt;
    std::cin >> answer;

    gil::rgb8_image_t img;

    if (str::ends_with(answer, ".png"))
        gil::read_and_convert_image(answer, img, gil::png_tag{});
    else
        gil::read_and_convert_image(answer, img, gil::jpeg_tag{});

    std::cout << "Loaded image is " << img.width() 
              << " wide and " << img.height()
              << " high. Please enter the size of image chunks that will be"
                 " replaced with block textures." 
              << std::endl;

    std::uint_fast32_t size;

    std::cout << prompt;
    std::cin >> answer;
    
    while ((size = std::stoi(answer)) < 0 || 
            size > std::min(img.width(), img.height()))
        std::cout << "Invalid imae size, please try again." << std::endl,
        std::cin >> answer;

    std::cout << "Composing..." << std::endl;

    std::pair<std::uint_fast32_t, std::uint_fast32_t> chunks = {
        img.width() / size, 
        img.height() / size};

    gil::rgb8_image_t result(
        chunks.first * tx::dim.first, 
        chunks.second * tx::dim.second);

    gil::rgb8_image_t chunk(tx::dim.first, tx::dim.second);
    double stats = 0.0;

    for (int i = 0; i < chunks.first; ++i)
        for (int j = 0; j < chunks.second; ++j) {
            std::pair<std::string, double> best_match = {"", -1.0};

            gil::resize_view(
                gil::subimage_view(
                    gil::view(img), i * size, j * size, size, size),
                gil::view(chunk),
                gil::nearest_neighbor_sampler{});
            
            for (auto&& kv : textures) {
                double match = cf::compare(
                    gil::const_view(chunk), gil::const_view(kv.second));

                if (match > best_match.second)
                    best_match.first  = kv.first,
                    best_match.second = match;
            }

            gil::copy_pixels(
                gil::const_view(textures[best_match.first]),
                gil::subimage_view(
                    gil::view(result), 
                    i * tx::dim.first, j * tx::dim.second, 
                    tx::dim.first, tx::dim.second)
            );

            stats += best_match.second;
        }

    std::cout << "Image composed out of blocks with an average similarity of "
              << stats / (chunks.first * chunks.second)
              << "." << std::endl;

    std::cout << "Please enter a path (including the image name) for saving."
              << std::endl;

    std::cout << prompt;
    std::cin >> answer;
    
    if (str::ends_with(answer, ".png"))
        gil::write_view(answer, gil::const_view(result), gil::png_tag{});
    else
        gil::write_view(answer, gil::const_view(result), gil::jpeg_tag{});
}