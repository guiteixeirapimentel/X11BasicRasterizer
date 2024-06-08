#include <iostream>
#include <X11BasicRaster.hpp>

int main()
{
    auto window = pimentel::X11Window::MakeX11Window(800, 600);
    if(!window)
    {
        std::cout << "Failed to create X11 window!" << std::endl;
        return 1;
    }

    std::cout << "Hello" << std::endl;
    while(window->Step())
    {
        window->ClearX11Window(pimentel::Color{.member_wise = {
            .b = 0,
            .g = 0,
            .r = 0,
            .a = 255,
        }});

        window->DrawPixel(50, 50, pimentel::Color{
            .member_wise{
                .b = 255,
                .g = 255,
                .r = 255,
                .a = 255,
            }
        });
    }
    return 0;
}