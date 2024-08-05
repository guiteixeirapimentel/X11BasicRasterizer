#include "X11BasicRaster.hpp"
#include <iostream>
#include <cstring>

using namespace pimentel;

Display* X11Window::display_ = nullptr;

X11Window::X11Window(int32_t width, int32_t height)
    :
    root_(None),
    window_(None),
    graphics_context_(nullptr),
    wm_delete_window_(),
    success_(false),
    width_(width),
    height_(height),
    pixel_data_(nullptr),
    window_image_(nullptr)
{
    if(display_ == nullptr)
    {
        display_ = XOpenDisplay(NULL);
        if (NULL == display_)
        {
            std::cerr << "Failed to initialize display" << std::endl;
            return;
        }
    }

    root_ = DefaultRootWindow(display_);
    if (None == root_)
    {
        std::cerr << "No root window found" << std::endl;
        XCloseDisplay(display_);
        return;
    }

    window_ = XCreateSimpleWindow(display_, root_, 0, 0, width_, height_, 0, 0, 0xffffffff);
    if (None == window_)
    {
        fprintf(stderr, "Failed to create window");
        XCloseDisplay(display_);
        return;
    }

    XMapWindow(display_, window_);

    graphics_context_ = XCreateGC(display_, window_, 0, 0);

    Atom wm_delete_window = XInternAtom(display_, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display_, window_, &wm_delete_window, 1);

    const auto inputTypes = StructureNotifyMask | KeyPressMask | KeyRelease | ButtonPressMask | ButtonReleaseMask;
    XSelectInput(display_, window_, inputTypes);

    XEvent event;
    while (1)
    {
        XNextEvent(display_, &event);
        if (event.type == MapNotify)
            break;
    }
    XFlush(display_);

    success_ = true;

    wm_delete_window_ = XInternAtom(display_, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display_, window_, &wm_delete_window_, 1);

    pixel_data_ = new int32_t[width_ * height_];

    window_image_ = XCreateImage(display_, CopyFromParent, DefaultDepth(display_, DefaultScreen(display_)), ZPixmap, 0,
                                (char *)pixel_data_, width_, height_, 32, width_ * sizeof(int32_t));
}

std::unique_ptr<X11Window> X11Window::MakeX11Window(int32_t width, int32_t height)
{
    auto res = std::unique_ptr<X11Window>(new X11Window{width, height});

    if(!res || !res->SuccessfullyCreated())
    {
        return nullptr;
    }

    return res;
}

X11Window::~X11Window()
{
    if(window_image_)
    {
        XDestroyImage(window_image_);
    }

    XCloseDisplay(display_);
}

bool pimentel::X11Window::SuccessfullyCreated() const
{
    return success_;
}

int32_t X11Window::GetWidth() const
{
    return width_;
}

int32_t X11Window::GetHeight() const
{
    return height_;
}

void X11Window::ClearX11Window(const Color& color)
{
    for(auto i = 0; i < width_*height_; i++)
    {
        pixel_data_[i] = color.argb;
    }
}

void X11Window::DrawPixel(int32_t x, int32_t y, const Color& color)
{
    pixel_data_[x + (y * width_)] = color.argb;
}

bool pimentel::X11Window::Step()
{
    XEvent event;

    while (XPending(display_) > 0)
    {
        XNextEvent(display_, &event);

        switch (event.type)
        {
        case ClientMessage:
        {
            if (event.xclient.data.l[0] == (long)wm_delete_window_)
            {
                XDestroyWindow(display_, window_);
                return false;
            }
        }
        break;
        case ButtonPress:
        case ButtonRelease:
        case EnterNotify:
        case MotionNotify:
        case LeaveNotify:
            // if(_mouseHandler)
            //     _mouseHandler->HandleInput(lDisplay, &xEvent);
            break;
        case KeyPress:
        case KeyRelease:
            // if(_keyboardHandler)
            //     _keyboardHandler->HandleInput(lDisplay, &xEvent);
            break;
        default:
            // if(_keyboardHandler)
            //     _keyboardHandler->HandleInput(lDisplay, &xEvent);
            break;
        }
        std::cout << "Got event: " << event.type << std::endl;
    }

    std::ignore = XPutImage(display_, window_, graphics_context_, window_image_, 0, 0, 0, 0, width_ * 4, height_ * 4);

    return true;
}
