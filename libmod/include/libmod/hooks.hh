#pragma once

#include <cstdint>
#include <map>
#include <string>

#ifdef WIN32
    #include <Windows.h>
    #include <d3d11.h>
#endif

#define dbg(msg) MessageBoxA(nullptr, msg, "dbg", MB_OK) // TEMP

namespace mod::hooks
{
    class base_hook_t
    {
    public:
        virtual void enable() = 0;
        virtual void disable() = 0;
    };

    class trampoline_hook_t : public base_hook_t
    {
    public:
        void enable();
        void disable();

        void* get_original();

        template <typename T>
        T* get_original()
        {
            return reinterpret_cast<T*>(get_original());
        }

    protected:
        void* m_function_to_hook;
    };

    class vtable_hook_t : public base_hook_t
    {
    public:
        vtable_hook_t(uintptr_t* vtable);
        vtable_hook_t(uintptr_t** class_instance) : vtable_hook_t(*class_instance) {};

        template <typename T>
        vtable_hook_t(T* class_instance) : vtable_hook_t(reinterpret_cast<uintptr_t**>(class_instance)) {};

        void enable();
        void disable();

        std::uintptr_t overwrite_entry(std::uint32_t index, std::uintptr_t pointer);

        template <typename T>
        T* overwrite_entry(std::uint32_t index, T* pointer)
        {
            return reinterpret_cast<T*>(overwrite_entry(index, reinterpret_cast<std::uintptr_t>(pointer)));
        }

    protected:
        void swap();

        std::uintptr_t* m_vtable;
        std::map<std::uint32_t, std::uintptr_t> m_changed_entries;
    };

    #ifdef WIN32
        typedef HRESULT(__stdcall* dx11_present_hook_t)(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags);
        typedef HRESULT(__stdcall* dx11_resize_buffers_hook_t)(IDXGISwapChain* swap_chain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT flags);
        class dx11_hook_t : public base_hook_t
        {
        public:
            dx11_hook_t(dx11_present_hook_t present, dx11_resize_buffers_hook_t resize_buffers);
            void enable();
            void disable();

            dx11_present_hook_t m_o_present;
            dx11_resize_buffers_hook_t m_o_resize_buffers;
        protected:
            dx11_present_hook_t m_present;
            dx11_resize_buffers_hook_t m_resize_buffers;
            vtable_hook_t* m_vtable_hook;
        };

        typedef void (*imgui_render_callback_t)();
        class imgui_dx11_hook_t : public base_hook_t
        {
        public:
            imgui_dx11_hook_t(imgui_render_callback_t cb);
            void enable();
            void disable();

            dx11_hook_t* m_dx11_hook;
            imgui_render_callback_t m_callback;
        };
    #endif
}
