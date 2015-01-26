#ifndef HEU_H_
#define HEU_H_

#include <stdint.h>
#include <algorithm>
#include <vector>


/**
 * MACROS
 */

// PROBLEM PARAMETERS
#ifndef ROWS
#define ROWS    (4)
#endif

#ifndef COLS
#define COLS    (4)
#endif

#ifndef RHO
#define RHO     (1)
#endif

#ifndef ROUNDS
#define ROUNDS  (4)
#endif

// Branch number
#define BRANCH_NUM (ROWS+1)

// Should we collect and print trails to PDFs?
#define COLLECT_TRAILS (0)

// Use filtering for diffusion before calling CPLEX?
#ifndef USE_DIFFUSION_FILTER
#define USE_DIFFUSION_FILTER (0)
#endif

// Print every improvement or only the best result?
#ifndef PRINT_ONLY_BEST
#define PRINT_ONLY_BEST (1)
#endif

// Lower bound on trail weight for CPLEX
#ifndef BF_MIN
#define BF_MIN (0)
#endif

// Number of threads used in CPLEX (0 = dynamic behavior, i.e. use as many threads as possible)
#ifndef CPLEX_NUM_THREADS
#define CPLEX_NUM_THREADS (0)
#endif

// SIMULATED ANNEALING SEARCH PARAMETERS
#define SA_INNER_LOOPS (10000)
#define MAX_INPUT_DIFF_WEIGHT ((ROWS*COLS)/2)

// HELPER MACROS
#define WORD_MASK ((0x1ull << COLS) - 1)
#define ROR(x,j) (( (x >> j) | (x << (COLS-j)) ) & WORD_MASK)


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

extern std::vector<u8> piRow;

#endif
