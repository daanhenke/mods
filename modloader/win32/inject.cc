#include <inject.hh>
#include <ui.hh>
#include <Windows.h>

namespace modloader
{
    decltype(OpenProcess)* ntOpenProcess;
    decltype(VirtualAllocEx)* ntVirtualAllocEx;
    decltype(WriteProcessMemory)* ntWriteProcessMemory;
    decltype(CreateRemoteThread)* ntCreateRemoteThread;
    decltype(WaitForSingleObject)* ntWaitForSingleObject;
    decltype(VirtualFreeEx)* ntVirtualFreeEx;
    decltype(LoadLibraryA)* ntLoadLibraryA;

    template <typename T>
    void find_export(std::uintptr_t base, std::uint16_t ordinal, T** result_ptr)
    {
        ordinal -= 1;
        auto dos_hdr = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
        *result_ptr = nullptr;
        if (dos_hdr->e_magic != IMAGE_DOS_SIGNATURE) return;

        auto nt_hdr = reinterpret_cast<IMAGE_NT_HEADERS64*>(base + dos_hdr->e_lfanew);
        if (nt_hdr->Signature != IMAGE_NT_SIGNATURE) return;

        auto exports_dir = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(base + nt_hdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
        auto address_list = reinterpret_cast<std::uint32_t*>(base + exports_dir->AddressOfFunctions);
        auto address_count = exports_dir->NumberOfFunctions;

        if (ordinal >= address_count) return;
        *result_ptr = reinterpret_cast<T*>(base + address_list[ordinal]);
    }

    bool inject(std::uintptr_t pid, std::filesystem::path dll_path)
    {
        static bool hasResolvedFunctions = false;

        if (! hasResolvedFunctions)
        {
            // Get kernel32.dll base and remove lower 12 bits to page align
            auto close_handle_ptr = reinterpret_cast<std::uintptr_t>(CloseHandle);
            close_handle_ptr = close_handle_ptr & (~0xFFF);

            // Find dos header
            while (true)
            {
                auto mz_header = reinterpret_cast<std::uint16_t*>(close_handle_ptr);
                if (*mz_header == IMAGE_DOS_SIGNATURE) break;
                close_handle_ptr -= 0x1000;
            }

            find_export(close_handle_ptr, 235, &ntCreateRemoteThread);
            find_export(close_handle_ptr, 1043, &ntOpenProcess);
            find_export(close_handle_ptr, 1499, &ntVirtualAllocEx);
            find_export(close_handle_ptr, 1502, &ntVirtualFreeEx);
            find_export(close_handle_ptr, 1515, &ntWaitForSingleObject);
            find_export(close_handle_ptr, 1583, &ntWriteProcessMemory);
            find_export(close_handle_ptr, 969, &ntLoadLibraryA);

            hasResolvedFunctions = true;
        }

        if (! std::filesystem::exists(dll_path)) return false;

        auto win_pid = static_cast<DWORD>(pid);
        auto process_handle = ntOpenProcess(PROCESS_ALL_ACCESS, FALSE, win_pid);
        if (process_handle == nullptr) return false;

        auto source_string = dll_path.string();
        auto buffer_size = source_string.size() + 1;

        auto remote_memory = ntVirtualAllocEx(process_handle, nullptr, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (remote_memory == nullptr)
        {
            CloseHandle(process_handle);
            return false;
        }

        if (! ntWriteProcessMemory(process_handle, remote_memory, source_string.data(), buffer_size, nullptr))
        {
            ntVirtualFreeEx(process_handle, remote_memory, buffer_size, MEM_RELEASE);
            CloseHandle(process_handle);
            return false;
        }

        auto thread_handle = ntCreateRemoteThread(process_handle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(ntLoadLibraryA), remote_memory, 0, nullptr);
        if (thread_handle == nullptr)
        {
            ntVirtualFreeEx(process_handle, remote_memory, buffer_size, MEM_RELEASE);
            CloseHandle(process_handle);
            return false;
        }

        ntWaitForSingleObject(thread_handle, INFINITE);
        CloseHandle(thread_handle);
        ntVirtualFreeEx(process_handle, remote_memory, buffer_size, MEM_RELEASE);
        CloseHandle(process_handle);
        return true;
    }
}
