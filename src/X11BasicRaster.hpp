#pragma once
#include <cstdint>
#include <memory>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace pimentel
{

union Color
{
    int32_t argb = {};
    struct
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a;
    }member_wise;
};

class X11Window
{
private:
    X11Window(int32_t width, int32_t height);

public:
    static std::unique_ptr<X11Window> MakeX11Window(int32_t width, int32_t height);
    ~X11Window();

    X11Window(const X11Window&) = delete;
    X11Window(X11Window&&) = delete;

    X11Window& operator=(const X11Window&) = delete;
    X11Window& operator=(X11Window&&) = delete;

    bool SuccessfullyCreated() const;

    int32_t GetWidth() const;
    int32_t GetHeight() const;

    void ClearX11Window(const Color& color);

    void DrawPixel(int32_t x, int32_t y, const Color& color);

    bool Step();

private:
    static Display* display_;
    Window root_;
    Window window_;
    GC graphics_context_;
    Atom wm_delete_window_;

    bool success_;
    int32_t width_;
    int32_t height_;

    int32_t* pixel_data_;
    XImage* window_image_;
};

}
