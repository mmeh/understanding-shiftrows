#include "heu.h"
#include <string.h>
#include <stddef.h>
#include <stdint.h>

u8 dr(u8 *pi) {
    size_t t,r,c,i;
    u8 active_cols[COLS] = {0};
    u8 tmp[COLS] = {0};
    u8 done = 0;
    
    // round 0
    for (r = 0; r < ROWS; ++r) {
        active_cols[ pi[r] ] = 1;
    }

	t = 1;
    while (!done) {
        memset(tmp, 0, sizeof(tmp));
        for (c = 0; c < COLS; ++c) {
            if (active_cols[c]) {
                for (r = 0; r < ROWS; ++r) {
                    i = (c + pi[(t % RHO)*ROWS + r]) % COLS;
                    tmp[i] = 1;
                }
            }
        }
        memcpy(active_cols, tmp, sizeof(tmp));

        done = 1;
        for (c = 0; c < COLS; ++c) {
            done &= active_cols[c];
        }
        t++;
    }
    return t + 1;
}


u8 diffusion_rounds(u8 *pi) {
    size_t t, r, c, i, wt;
    u64 active_cols = 0, tmp = 0;
    u8 last_wt, this_wt, non_improved = 0;

	// set initially active columns
    for (r = 0; r < ROWS; ++r)
        active_cols |= (1 << (pi[r] % COLS));
	
	t = 1;
	
	// run as long as not all columns are active or until we have not seen improvement for 3 rounds
    while (__builtin_popcount(active_cols) < COLS && non_improved < 3) {		
        tmp = active_cols;
        last_wt = __builtin_popcount(tmp);
        
        // set the updated active_cols
        active_cols = 0;
        for (c = 0; c < last_wt; ++c) {
			i = __builtin_ctz(tmp);			
			
			for (r = 0; r < ROWS; ++r)
				active_cols |= (1 << ((i + pi[(t % RHO)*ROWS + r]) % COLS));
			
			tmp ^= (1 << i);
		}
		
		// check for improvement
		if (__builtin_popcount(active_cols) == last_wt)
			non_improved++;
		
        t++;
    }
    return t + 1;
}
