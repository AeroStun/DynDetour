/*
 * Copyright 2023 AeroStun
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef DYNDETOUR_DETOUR_HXX
#define DYNDETOUR_DETOUR_HXX

#include <cstdint>
#include <functional>
#include <utility>

#include <dlhook.hxx>

namespace impl {

template <class Sig, Sig* Func>
class Hook {
  public:
    template <class Tap>
    static void tap(Tap&&) = delete;

    static void trap() = delete;

    static void enable() = delete;

    static void disable() = delete;
};

template <class R, class... Args, R (*Func)(Args...)>
class Hook<R(Args...), Func> {
    struct Fields {
        const std::uintptr_t real;
        const std::uintptr_t hook;
        std::function<R(R (*)(Args...), Args...)> tap;
    };

    static Fields m_fields;

    [[noreturn]] static R blackhole_trap(R (*)(Args...), Args&&...) { __builtin_trap(); }

  public:
    template <class Tap>
    static void tap(Tap&& tap) {
        m_fields.tap = tap;
    }

    static void trap() { m_fields.tap = blackhole_trap; }

    static void enable() {
        dlhook_addr_all(reinterpret_cast<void*>(m_fields.real), reinterpret_cast<void*>(m_fields.hook));
    }

    static void disable() {
        dlhook_addr_all(reinterpret_cast<void*>(m_fields.hook), reinterpret_cast<void*>(m_fields.real));
    }
};

template <class R, class... Args, R (*Func)(Args...)>
typename Hook<R(Args...), Func>::Fields Hook<R(Args...), Func>::m_fields{
    reinterpret_cast<std::uintptr_t>(Func), reinterpret_cast<std::uintptr_t>(+[](Args... args) -> R {
        return Hook<R(Args...), Func>::m_fields.tap(
            reinterpret_cast<R (*)(Args...)>(Hook<R(Args...), Func>::m_fields.real), std::forward<Args>(args)...);
    }),
    &Hook<R(Args...), Func>::blackhole_trap};

template <class... Hooks>
struct HooksGroup;

template <>
struct HooksGroup<> {
    template <class F>
    static void tap(const F&) {}
    static void trap() {}
    static void enable() {}
    static void disable() {}
};

template <class Hook, class... Hooks>
struct HooksGroup<Hook, Hooks...> {
    template <class F>
    static void tap(const F& f) {
        Hook::tap(f);
        HooksGroup<Hooks...>::tap(f);
    }
    static void trap() {
        Hook::trap();
        HooksGroup<Hooks...>::trap();
    }
    static void enable() {
        Hook::enable();
        HooksGroup<Hooks...>::enable();
    }
    static void disable() {
        Hook::disable();
        HooksGroup<Hooks...>::disable();
    }
};

} // namespace impl

template <class Sig, Sig* Func>
using Detour = impl::Hook<Sig, Func>;

template <class... Hooks>
using DetourGroup = impl::HooksGroup<Hooks...>;

#endif // DYNDETOUR_DETOUR_HXX
