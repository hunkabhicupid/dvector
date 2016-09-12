#pragma once

#include <windows.h>

typedef DWORD PlatformThreadId;
typedef void *PlatformThreadHandle; // HANDLE
const PlatformThreadHandle kNullThreadHandle = nullptr;
const PlatformThreadId kInvalidThreadId = 0;

// A namespace for low-level thread functions.
class PlatformThread {
  public:
    // ThreadMain method will be called on the newly created thread.
    class Delegate {
      public:
        virtual ~Delegate() {}
        virtual void ThreadMain() = 0;
    };

    // Gets the current thread id
    static PlatformThreadId CurrentId();

    // Sleeps for the specified duration (units are milliseconds).
    static void Sleep(int duration_ms);

    // Creates a new thread.  The |stack_size| parameter can be 0 to indicate
    // that the default stack size should be used.  Upon success,
    // |*thread_handle| will be assigned a handle to the newly created thread,
    // and |delegate|'s ThreadMain method will be executed on the newly created
    // thread.
    // NOTE: When you are done with the thread handle, you must call Join to
    // release system resources associated with the thread.  You must ensure
    // that
    // the Delegate object outlives the thread.
    static bool Create(size_t stack_size, Delegate *delegate,
                       PlatformThreadHandle *thread_handle);

    // Joins with a thread created via the Create function.  This function
    // blocks
    // the caller until the designated thread exits.  This will invalidate
    // |thread_handle|.
    static void Join(PlatformThreadHandle thread_handle);
};
