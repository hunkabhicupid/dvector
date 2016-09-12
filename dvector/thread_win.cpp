#include "stdafx.h"

namespace {
struct ThreadParams {
    PlatformThread::Delegate *delegate;
    bool joinable;
};

DWORD __stdcall ThreadFunc(void *params) {
    ThreadParams *thread_params = static_cast<ThreadParams *>(params);
    PlatformThread::Delegate *delegate = thread_params->delegate;
    delete thread_params;
    delegate->ThreadMain();
    return NULL;
}

// CreateThreadInternal() matches PlatformThread::Create(), except that
// |out_thread_handle| may be NULL, in which case a non-joinable thread is
// created.
bool CreateThreadInternal(size_t stack_size, PlatformThread::Delegate *delegate,
                          PlatformThreadHandle *out_thread_handle) {
    PlatformThreadHandle thread_handle;
    unsigned int flags = 0;
    if (stack_size > 0) {
        flags = STACK_SIZE_PARAM_IS_A_RESERVATION;
    } else {
        stack_size = 0;
    }

    ThreadParams *params = new ThreadParams;
    params->delegate = delegate;
    params->joinable = out_thread_handle != nullptr;

    thread_handle =
        CreateThread(nullptr, stack_size, ThreadFunc, params, flags, nullptr);
    if (!thread_handle) {
        delete params;
        return false;
    }

    if (out_thread_handle)
        *out_thread_handle = thread_handle;
    else
        CloseHandle(thread_handle);
    return true;
}

} // namespace

// static
PlatformThreadId PlatformThread::CurrentId() { return GetCurrentThreadId(); }

// static
void PlatformThread::Sleep(int duration_ms) { ::Sleep(duration_ms); }

// static
bool PlatformThread::Create(size_t stack_size, Delegate *delegate,
                            PlatformThreadHandle *thread_handle) {
    return CreateThreadInternal(stack_size, delegate, thread_handle);
}

// static
void PlatformThread::Join(PlatformThreadHandle thread_handle) {
    // Wait for the thread to exit.  It should already have terminated but make
    // sure this assumption is valid.
    DWORD result = WaitForSingleObject(thread_handle, INFINITE);
    CloseHandle(thread_handle);
}
