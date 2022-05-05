#include <libmod/hooks.hh>

namespace mod::hooks
{
    imgui_dx11_hook_t* dx11_hook_ptr;

    HRESULT __stdcall present_hk(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags)
    {
        auto result = dx11_hook_ptr->m_dx11_hook->m_o_present(swap_chain, sync_interval, flags);
        dx11_hook_ptr->m_callback();
        return result;
    }

    HRESULT __stdcall resize_buffers_hk(IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT flags)
    {
        return dx11_hook_ptr->m_dx11_hook->m_o_resize_buffers(swap_chain, buffer_count, width, height, new_format, flags);
    }

    imgui_dx11_hook_t::imgui_dx11_hook_t(imgui_render_callback_t cb)
    {
        m_dx11_hook = nullptr;
        m_callback = cb;
    }

    void imgui_dx11_hook_t::enable()
    {
        m_dx11_hook = new dx11_hook_t(present_hk, resize_buffers_hk);
        dx11_hook_ptr = this;
        m_dx11_hook->enable();
    }

    void imgui_dx11_hook_t::disable()
    {
        if (m_dx11_hook != nullptr)
        {
            m_dx11_hook->disable();
            delete m_dx11_hook;
            m_dx11_hook = nullptr;
        }
    }
}
