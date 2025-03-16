#include <cstdio>
#include <cstdlib>

#include <detour.hxx>

int main() {
    using Hooks = DetourGroup<Detour<decltype(std::putchar), std::putchar>, //
                              Detour<decltype(std::putc), std::putc>>;

    std::size_t call_count{0U};
    Hooks::tap([&call_count](auto original, auto&&... args) {
        ++call_count;
        std::puts("putc{,har} called!");
        return original(std::forward<decltype(args)>(args)...);
    });

    std::putchar(0);

    Hooks::enable();

    std::putchar(0);

    std::putchar(0);

    Hooks::disable();

    std::putchar(0);

    if (call_count != 2U) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
