#include <libmod/hooks.hh>

#include <sstream>
std::string to_hex(std::size_t number)
{
    std::stringstream stream;
    stream << std::hex << number;
    return stream.str();
}

const char* temp_win_class_name = "lmdx11";

namespace mod::hooks
{
    std::uintptr_t* find_swapchain_vtable()
    {
        WNDCLASSEXA temp_window_class;
        std::memset(&temp_window_class, 0, sizeof temp_window_class);
        temp_window_class.cbSize = sizeof temp_window_class;
        temp_window_class.hInstance = GetModuleHandleA(nullptr);
        temp_window_class.lpszClassName = temp_win_class_name;
        temp_window_class.style = CS_HREDRAW | CS_VREDRAW;
        temp_window_class.lpfnWndProc = DefWindowProcA;

        RegisterClassExA(&temp_window_class);
        auto temp_window_handle = CreateWindowA(temp_win_class_name, temp_win_class_name, WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, temp_window_class.hInstance, nullptr);

        DXGI_SWAP_CHAIN_DESC sc_desc;
        std::memset(&sc_desc, 0, sizeof sc_desc);
        sc_desc.BufferCount = 1;
        sc_desc.BufferDesc.Width = 2;
        sc_desc.BufferDesc.Height = 2;
        sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sc_desc.BufferDesc.RefreshRate.Numerator = 60;
        sc_desc.BufferDesc.RefreshRate.Denominator = 1;
        sc_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sc_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sc_desc.OutputWindow = temp_window_handle;
        sc_desc.SampleDesc.Count = 1;
        sc_desc.SampleDesc.Quality = 0;
        sc_desc.Windowed = TRUE;
        sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        IDXGISwapChain* sc;
        ID3D11Device* dev;
        D3D_FEATURE_LEVEL chosen_level;
        auto res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sc_desc, &sc, &dev, &chosen_level, nullptr);
        if (FAILED(res))
        {
            dbg(to_hex(res).c_str());
            DestroyWindow(temp_window_handle);
            UnregisterClassA(temp_win_class_name, temp_window_class.hInstance);
            return nullptr;
        }

        auto sc_vtable = *reinterpret_cast<uintptr_t**>(sc);

        sc->Release();
        dev->Release();

        DestroyWindow(temp_window_handle);
        UnregisterClassA(temp_win_class_name, temp_window_class.hInstance);

        return sc_vtable;
    }

    dx11_hook_t::dx11_hook_t(dx11_present_hook_t present, dx11_resize_buffers_hook_t resize_buffers)
    {
        m_present = present;
        m_resize_buffers = resize_buffers;
        m_vtable_hook = nullptr;
    }

    void dx11_hook_t::enable()
    {
        uintptr_t* vtable = find_swapchain_vtable();

        m_vtable_hook = new vtable_hook_t(vtable);
        m_o_present = m_vtable_hook->overwrite_entry(8, m_present);
        m_o_resize_buffers = m_vtable_hook->overwrite_entry(13, m_resize_buffers);

        m_vtable_hook->enable();
    }

    void dx11_hook_t::disable()
    {
        if (m_vtable_hook != nullptr)
        {
            m_vtable_hook->disable();
            delete m_vtable_hook;
            m_vtable_hook = nullptr;
        }
    }
}
