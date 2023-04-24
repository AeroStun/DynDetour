#include <cstdio>
#include <vector>

#include <detour.hxx>

int main() {
    using Hooks = DetourGroup<Detour<decltype(std::putchar), std::putchar>>;
    Hooks::tap([](auto original, auto&&... args) {
        std::puts("putchar called!");
        return original(std::forward<decltype(args)>(args)...);
    });

    std::putchar(0);

    Hooks::enable();

    std::putchar(0);

    std::putchar(0);

    Hooks::disable();

    std::putchar(0);
}
