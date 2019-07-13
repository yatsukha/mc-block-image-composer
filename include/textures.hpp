#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include <map>
#include <set>
#include <cassert>
#include <iostream>

#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/filesystem.hpp>

namespace tx {
    
    using namespace boost;
    namespace fs = filesystem;

    // TODO: does not help the png include
    const std::string extension = "png";
    const std::pair<std::uint_fast32_t, std::uint_fast32_t> dim = {16, 16};
    const std::set<std::string> excluded{
        "door_top",
        "door_bottom",
        "overlay"
    };

    std::map<std::string, gil::rgb8_image_t> load_textures(
        const std::string& directory) noexcept {

        std::map<std::string, gil::rgb8_image_t> images;

        for (fs::directory_iterator it(fs::path{directory});
             it != fs::directory_iterator{}; ++it) {

            std::string file_name = it->path().string();

            if (file_name.length() < 3 || 
                file_name.substr(file_name.length() - 3) != extension)
                continue;

            bool valid = true;
            for (auto&& token : excluded)
                if (file_name.find(token) != std::string::npos) {
                    valid = false;
                    break;
                }

            if (!valid)
                continue;

            gil::rgb8_image_t img;
            gil::read_and_convert_image(file_name, img, gil::png_tag{});

            if (img.width() != dim.first || img.height() != dim.second)
                continue;

            //std::cout << file_name << std::endl;

            assert(images.count(file_name) == 0);
            images[file_name] = img;
        }

        return images;
    }

};

#endif