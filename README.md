# DynDetour

A C++ hooking library for functions from Linux DSOs.

Usage:
```cpp
#include <detour.hxx>

int main() {
    using NewHook = Detour<void*(std::size_t), ::operator new>;
    
    // Enables the hook; the default tap is a trap
    NewHook::enable();
    
    // Set a custom tap, which takes the original function and the params
    std::size_t new_count{0U};
    NewHook::tap([&](auto original, const std::size_t arg){
        ++new_count;
        return original(arg);
    });
    
    // Erase the tap and replace it with a trap
    NewHook::trap();
    
    // Disables the hook
    NewHook::disable();
}
```
