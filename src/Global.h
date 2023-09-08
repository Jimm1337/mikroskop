#ifndef MIKROSKOP_GLOBAL_H
#define MIKROSKOP_GLOBAL_H

#include <cstdint>
#include <tuple>

namespace Global {

using PosTupleXYZ = std::tuple< int_fast16_t, int_fast16_t, int_fast16_t >;

inline constexpr auto CHAR_STOP = '#';
inline constexpr auto CHAR_MOVE = 's';
inline constexpr auto CHAR_INFO = 'i';

void setup();

}

#endif // MIKROSKOP_GLOBAL_H
