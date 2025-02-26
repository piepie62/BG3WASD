#pragma once

class LoadStringHook
{
public:
    static void Enable();
    static bool Prepare();

private:
    static DWORD* OverrideFunc(DWORD* a1, int64_t a2);
    static inline std::add_pointer_t<decltype(OverrideFunc)> OriginalFunc;
    static inline std::array<uintptr_t, 1> addresses;
    static inline bool all_found = false;
};
