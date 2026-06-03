#ifndef REGISTER_MACROS_H
#define REGISTER_MACROS_H

#define SET_BIT(r, pos) ((r) |= (1U << (pos)))
#define SET_2BIT_FIELD(r, pos, val) ((r) = ((r) & ~(3U << (pos * 2))) | (((val) & 3U) << (pos * 2)))
#define TOGGLE_BIT(r, pos) ((r) ^= (1U << (pos)))
#define CLEAR_BIT(r, pos) ((r) &= (~(1U << (pos))))
#define GET_BIT(r, pos) (((r) >> (pos)) & 1U)


#endif