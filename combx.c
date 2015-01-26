#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define ROWS 3
#define COLS 4
#define RHO 2
#define ENDMASK (((1 << ROWS) - 1) << (COLS - ROWS))

typedef uint8_t u8;
typedef uint32_t u32;

u8 combx(u32 *v, u32 *t, u8 *piRow) {
    u8 i;
    if (*v > ((1 << ROWS) - 1) << (COLS - ROWS)) {
        *v = (1 << ROWS) - 1; /* reset for another iteration of an outer loop */
        return 0;
    }
    *t = *v;
    for (i = 0; i < ROWS; ++i) {
        piRow[i] = (u8)__builtin_ctz(*t);
        *t ^= (1 << __builtin_ctz(*t));
    }
    *t = *v | (*v - 1);
    *v = (*t + 1) | (((~(*t) & -~(*t)) - 1) >> (__builtin_ctz(*v) + 1));

    return 1;
}

int main()
{
	int ctr = 0;
	
    u8 i,j;
    u32 t[RHO], v[RHO];
    for (i = 0; i < RHO; ++i)
        v[i] = (1 << ROWS) - 1;
        
    u8 pi[RHO * ROWS] = {0};
    
    while(combx(&v[0], &t[0], &pi[0])) {
#if RHO > 1
        while(combx(&v[1], &t[1], &pi[ROWS])) {
#endif
#if RHO > 2
			while(combx(&v[2], &t[2], &pi[2*ROWS])) {
#endif
#if RHO > 3
				while(combx(&v[3], &t[3], &pi[3*ROWS])) {
#endif    
					for (i = 0; i < RHO; ++i) {
						for (j = 0; j < ROWS; ++j)
							printf("%d ", pi[i*ROWS + j]);
						printf("\n");
					}
					printf("\n");
					ctr++;
#if RHO > 3
				}
#endif
#if RHO > 2
			}
#endif
#if RHO > 1
		}
#endif
	}
	
	printf("ctr %d\n", ctr);
}
