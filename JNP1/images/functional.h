#ifndef MIM_FUNCTIONAL_H
#define MIM_FUNCTIONAL_H

#include <functional>

inline auto compose(){
  return std::identity();
}

template <typename Fun, typename... Args>
auto compose(Fun f, Args... args){
  return [=](auto p){ return compose(args...)(f(p)); };
}

template <typename H, typename... Fs>
auto lift(H h, Fs... fs){
  return [=](auto p){ return h(fs(p)...); };
}

#endif