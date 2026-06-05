#ifndef REGISTER_MACROS_H
#define REGISTER_MACROS_H


/*  Macros for setting, clearing, toggling, and getting bits in a register.
    These macros take a register (r) and a bit position (pos) as arguments.
    The SET_2BIT_FIELD macro is used for setting a 2-bit field in a register, where 'val' is the value to set (0-3). */
#define SET_BIT(r, pos) ((r) |= (1U << (pos)))
#define SET_2BIT_FIELD(r, pos, val) ((r) = ((r) & ~(3U << (pos * 2))) | (((val) & 3U) << (pos * 2)))
#define TOGGLE_BIT(r, pos) ((r) ^= (1U << (pos)))
#define CLEAR_BIT(r, pos) ((r) &= (~(1U << (pos))))
#define GET_BIT(r, pos) (((r) >> (pos)) & 1U)


#endif