#ifndef SETTING_H_
#define SETTING_H_

/**
 * MACROS
 */

#ifndef ROWS
#define ROWS    (4)
#endif

#ifndef COLS
#define COLS   (10)
#endif

#ifndef RHO
#define RHO     (2)
#endif

#ifndef ROUNDS
#define ROUNDS  (6)
#endif


#define BRANCH_NUM (ROWS+1)

#define SA_INNER_LOOPS (600)
#define MAX_INPUT_DIFF_WEIGHT ((ROWS*COLS)/2)

#define WORD_MASK ((0x1ull << COLS) - 1)
#define ROR(x,j) (( (x >> j) | (x << (COLS-j)) ) & WORD_MASK)

#define DEBUG 0

/**
 * TYPEDEFS
 */
#if COLS <= 16
typedef uint16_t word;
#elif COLS <= 32
typedef uint32_t word;
#elif COLS <= 64
typedef uint64_t word;
#endif

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#endif
