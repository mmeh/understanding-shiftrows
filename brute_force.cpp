#include "heu.h"
#include "brute_force.h"
#include "cplex.h"
#include "helpers.h"
#include "diffusion.h"
#include <vector>

///////////////////////////////
//   BRUTE FORCING PI MATRIX //
///////////////////////////////

u8 next_perm(char *rowPerm) {
	char i;
	for (i = ROWS - 1; i >= 0; --i) {
		rowPerm[i]++;
		if (rowPerm[i] < COLS)
			return 1;
		rowPerm[i] = 0;
	}
	return 0;
}

/**
 * Function for naively brute forcing the Pi matrices.
 * We consider _all_ RHO x R matrices over Z/CZ
 */
void naive_brute_force_pi() {
	printf("=== Searching using naïve brute force ===\n");
	u8 pi[RHO * ROWS] = { 0 }, best_pi[RHO * ROWS];

	u16 this_wt, best_wt = 0;

	char rowPerm[RHO*ROWS] = { 0 };
	for (int i = 1; i <= RHO; ++i)
		rowPerm[i*ROWS-1] = -1;

	while (next_perm(&rowPerm[0])) {
		#if RHO > 1
		while (next_perm(&rowPerm[ROWS])) {
		#endif
			#if RHO > 2
			while (next_perm(&rowPerm[2*ROWS])) {
			#endif
				#if RHO > 3
				while (next_perm(&rowPerm[ROWS])) {
				#endif
					memcpy(pi, rowPerm, RHO * ROWS * sizeof(u8));

					this_wt = find_cplex(pi, (u16)BF_MIN, best_wt);
					if (this_wt > best_wt) {
						memcpy(best_pi, pi, RHO * ROWS * sizeof(u8));
						best_wt = this_wt;

						printf("%d\t%d\t%d\t%d\t%d\t", ROWS, COLS, RHO, ROUNDS, best_wt);
						print_pi(best_pi); printf(",\n");
					}
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
}

void next_choice(u8 *buf) {
	size_t i, t = ROWS - 1;
	while (t != 0 && buf[t] == COLS - ROWS + t) {
		t--;
	}
	buf[t]++;
	for (i = t + 1; i < ROWS; i++) {
		buf[i] = buf[i - 1] + 1;
	}
}

/**
 * Method that iterates over all Pi matrices where
 * row entries are distinct
 */
void brute_force_pi() {
	printf("=== Searching using brute force ===\n");
	u8 buf[RHO * ROWS];
    u8 pi[RHO * ROWS], best_pi[RHO * ROWS];

    size_t i, j, ctr = 0;
    u16 this_wt, best_wt = 0;

    u8 stdRow[ROWS];
    for (i = 0; i < ROWS; ++i)
		stdRow[i] = i;

    // row 0
    memcpy(buf, stdRow, ROWS*sizeof(u8));
    while (buf[ROWS - 1] < COLS) {
        memcpy(pi, buf, ROWS*sizeof(u8));
        do {

			#if RHO > 1
			// row 1
			memcpy((buf+ROWS), stdRow, ROWS*sizeof(u8));
			while ((buf+ROWS)[ROWS - 1] < COLS) {
				memcpy(pi+ROWS, buf+ROWS, ROWS*sizeof(u8));
				do {
			#endif
					#if RHO > 2
					// row 2
					memcpy((buf+2*ROWS), stdRow, ROWS*sizeof(u8));
					while ((buf+2*ROWS)[ROWS - 1] < COLS) {
						memcpy(pi+2*ROWS, buf+2*ROWS, ROWS*sizeof(u8));
						do {
					#endif
							#if RHO > 3
							// row 3
							memcpy((buf+3*ROWS), stdRow, ROWS*sizeof(u8));
							while ((buf+3*ROWS)[ROWS - 1] < COLS) {
								memcpy(pi+3*ROWS, buf+3*ROWS, ROWS*sizeof(u8));
								do {
							#endif

									this_wt = find_cplex(pi, (u16)BF_MIN, best_wt);

									if (this_wt > best_wt) {
										best_wt = this_wt;
										memcpy(best_pi, pi, sizeof(pi));

										printf("%d\t%d\t%d\t%d\t%d\t", ROWS, COLS, RHO, ROUNDS, best_wt);
										print_pi(best_pi); printf(",\n");
									}

							#if RHO > 3
								} while (std::next_permutation(pi+3*ROWS,pi+4*ROWS));
								next_choice(buf+3*ROWS);
							}
							#endif

					#if RHO > 2
						} while (std::next_permutation(pi+2*ROWS,pi+3*ROWS));
						next_choice(buf+2*ROWS);
					}
					#endif

			#if RHO > 1
				} while (std::next_permutation(pi+ROWS,pi+2*ROWS));
				next_choice(buf+ROWS);
			}
			#endif

        } while (std::next_permutation(pi,pi+ROWS));
		next_choice(buf);
    }
}

/**
 * Method that iterates over all Pi matrices where
 * row entries are distinct and
 * they are sorted in increasing order
 */
void brute_force_pi_noperm() {
	printf("=== Searching using brute force (with no row permutation) ===\n");
	u8 buf[RHO * ROWS];
    u8 pi[RHO * ROWS], best_pi[RHO * ROWS];

    size_t i, j, ctr = 0;
    u16 this_wt, best_wt = 0;
    u16 diff, diff_best = (1 << 15);

    // make the standard row 0 1 .... R-1
    u8 stdRow[ROWS];
    for (i = 0; i < ROWS; ++i)
		stdRow[i] = i;

    // row 0
    memcpy(buf, stdRow, ROWS*sizeof(u8));
    while (buf[ROWS - 1] < COLS) {
        memcpy(pi, buf, ROWS*sizeof(u8));

#if RHO > 1
		// row 1
		memcpy((buf+ROWS), stdRow, ROWS*sizeof(u8));
		while ((buf+ROWS)[ROWS - 1] < COLS) {
			memcpy(pi+ROWS, buf+ROWS, ROWS*sizeof(u8));
#endif
#if RHO > 2
			// row 2
			memcpy((buf+2*ROWS), stdRow, ROWS*sizeof(u8));
			while ((buf+2*ROWS)[ROWS - 1] < COLS) {
				memcpy(pi+2*ROWS, buf+2*ROWS, ROWS*sizeof(u8));
#endif
#if RHO > 3
				// row 3
				memcpy((buf+3*ROWS), stdRow, ROWS*sizeof(u8));
				while ((buf+3*ROWS)[ROWS - 1] < COLS) {
					memcpy(pi+3*ROWS, buf+3*ROWS, ROWS*sizeof(u8));
#endif
					#if USE_DIFFUSION_FILTER
                    // Determine rounds required for full diffusion
                    diff = diffusion_rounds(pi);

					// If at least as good as the best, move on to trails
                    if (diff <= diff_best) {
                        diff_best = diff;
                    #endif
						// do the call
						this_wt = find_cplex(pi, (u16)BF_MIN, best_wt);

						if (this_wt > best_wt) {
							best_wt = this_wt;
							memcpy(best_pi, pi, sizeof(pi));

							printf("%d\t%d\t%d\t%d\t%d\t", ROWS, COLS, RHO, ROUNDS, best_wt);
							print_pi(best_pi); printf(",\n");
						}
					#if USE_DIFFUSION_FILTER
					}
					#endif

#if RHO > 3
					next_choice(buf+3*ROWS);
				}
#endif
#if RHO > 2
				next_choice(buf+2*ROWS);
			}
#endif
#if RHO > 1
			next_choice(buf+ROWS);
		}
#endif
		next_choice(buf);
    }
}

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




std::vector<trail> pictureTrails;

/**
 * Method that iterates over all Pi matrices where
 * row entries are distinct and
 * they are sorted in increasing order
 */
void n_choose_k() {
	printf("=== Searching using brute force (with no row permutation, bit hack method) ===\n");
	u8 pi[RHO * ROWS], best_pi[RHO * ROWS];
	u8 i, j;
    u32 t[RHO], v[RHO];
    u16 this_wt, best_wt = 0;
    u16 diff, diff_best = 0xFFFF;

    for (i = 0; i < RHO; ++i)
        v[i] = (1 << ROWS) - 1;

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
#if RHO > 4
	while(combx(&v[4], &t[4], &pi[4*ROWS])) {
#endif
#if RHO > 5
	while(combx(&v[5], &t[5], &pi[5*ROWS])) {
#endif
#if RHO > 6
	while(combx(&v[6], &t[6], &pi[6*ROWS])) {
#endif
#if RHO > 7
	while(combx(&v[7], &t[7], &pi[7*ROWS])) {
#endif


			#if USE_DIFFUSION_FILTER
			// Determine rounds required for full diffusion
			diff = diffusion_rounds(pi);

			// If at least as good as the best, move on to trails
			if (diff <= diff_best) {
				diff_best = diff;
			#endif

				//~ print_pi(pi); printf("\n");

				// Get lowest trail wt possible with cplex
				this_wt = find_cplex(pi, (u16)BF_MIN, best_wt, &pictureTrails);

				if (this_wt > best_wt) {
					best_wt = this_wt;
					memcpy(best_pi, pi, sizeof(pi));

					printf("%d\t%d\t%d\t%d\t%d\t", ROWS, COLS, RHO, ROUNDS, best_wt);
					print_pi(best_pi); printf(",\n");
				}
			#if USE_DIFFUSION_FILTER
			}
			#endif


#if RHO > 7
	}
#endif
#if RHO > 6
	}
#endif
#if RHO > 5
	}
#endif
#if RHO > 4
	}
#endif
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

	#if COLLECT_TRAILS
	writeTrailsToPDF(pictureTrails);
	#endif
}


/**
 * This is for bruteforcing with RHO = 3 for some geometry, where we check the bound
 * using CPLEX for pi and piOther, where piOther has the middle row inversed
 */
void bruteforceWithMiddleInverse() {
	printf("=== Searching for counterexample to conjecture that one can change rotation direction ===\n");
	u8 pi[RHO * ROWS], best_pi[RHO * ROWS];
	u8 piOther[RHO * ROWS];

	u16 piWt, piOtherWt;

	u8 i,j;
    u32 t[RHO], v[RHO];

    for (i = 0; i < RHO; ++i)
        v[i] = (1 << ROWS) - 1;

    while(combx(&v[0], &t[0], &pi[0])) {
        while(combx(&v[1], &t[1], &pi[ROWS])) {
			//~ while(combx(&v[2], &t[2], &pi[2*ROWS])) {
				// Copy to piOther and inverse middle rotation set
				memcpy(piOther, pi, (RHO * ROWS) * sizeof(u8));
				for (j = 0; j < ROWS; ++j)
					piOther[ROWS+j] = (2*COLS - piOther[ROWS+j]) % COLS;

				//~ print_pi(pi); 		printf("\n");
				//~ print_pi(piOther); 	printf("\n\n");
				piWt = find_cplex(pi, 0, 0);
				piOtherWt = find_cplex(piOther, 0, 0);

				if (piWt != piOtherWt) {
					printf("COUNTEREXAMPLE FOUND!\n");
					printf("%d\t%d\t%d\t%d\t%d != %d\n", ROWS, COLS, RHO, ROUNDS, piWt, piOtherWt);
					print_pi(pi); 		printf("\n");
					print_pi(piOther); 	printf("\n\n");
				}
		//~ }
		}
	}
}




u8 next_pirow(u32 *v, u32 *t, u8 *piRow) {
    u8 i;
    if (*v > (((1 << (ROWS-1)) - 1) << (COLS - ROWS))) {
        *v = (1 << (ROWS-1)) - 1; /* reset for another iteration of an outer loop */
        return 0;
    }
    *t = *v;
    for (i = 0; i < ROWS-1; ++i) {
        piRow[i+1] = (u8)__builtin_ctz(*t) + 1;
        *t ^= (1 << __builtin_ctz(*t));
    }
    *t = *v | (*v - 1);
    *v = (*t + 1) | (((~(*t) & -~(*t)) - 1) >> (__builtin_ctz(*v) + 1));

    return 1;
}


void test_pi() {

	//u8 pi[7] = {0,1,2,3,4,5,7};// best_pi[RHO * ROWS];
    u8 pi[5] = {0,1,2,4,7};// best_pi[RHO * ROWS];
    //u32 t[RHO], v[RHO];
    u16 this_wt, best_wt = 0;
    //u16 diff, diff_best = 0xFFFF;

    //for (u8 i = 0; i < RHO; ++i) {
    //    v[i] = (1 << (ROWS-1)) - 1;
    //    pi[ROWS*i] = 0;
    //}

	this_wt = find_cplex(pi, (u16)BF_MIN, best_wt, NULL);
    printf("%d\t%d\t%d\t%d\t%d\t", ROWS, COLS, RHO, ROUNDS, this_wt);
    print_pi(pi); printf(",\n");

}

void equivalence_bruteforce() {
	//~ printf("=== Searching using brute force (with no row permutation, bit hack method) ===\n");
	u8 pi[RHO * ROWS], best_pi[RHO * ROWS];
    u32 t[RHO], v[RHO];
    u16 this_wt, best_wt = 0;
    u16 diff, diff_best = 0xFFFF;

    for (u8 i = 0; i < RHO; ++i) {
        v[i] = (1 << (ROWS-1)) - 1;
        pi[ROWS*i] = 0;
    }

	while(next_pirow(&v[0], &t[0], &pi[0])) {
#if RHO > 1
	while(next_pirow(&v[1], &t[1], &pi[ROWS])) {
#endif
#if RHO > 2
	while(next_pirow(&v[2], &t[2], &pi[2*ROWS])) {
#endif
#if RHO > 3
	while(next_pirow(&v[3], &t[3], &pi[3*ROWS])) {
#endif
#if RHO > 4
	while(next_pirow(&v[4], &t[4], &pi[4*ROWS])) {
#endif
#if RHO > 5
	while(next_pirow(&v[5], &t[5], &pi[5*ROWS])) {
#endif
#if RHO > 6
	while(next_pirow(&v[6], &t[6], &pi[6*ROWS])) {
#endif
#if RHO > 7
	while(next_pirow(&v[7], &t[7], &pi[7*ROWS])) {
#endif

			// Check if second entry in each row is <= N/2
			u8 goodPi = 1;
			for (int j = 0; j < RHO; ++j) {
				if (pi[j*ROWS+1] > COLS/2) {
					goodPi = 0;
					break;
				}
			}
			// If it is, continue
			if (goodPi) {
				#if USE_DIFFUSION_FILTER
				// Determine rounds required for full diffusion
				diff = diffusion_rounds(pi);

				// If at least as good as the best, move on to trails
				if (diff <= diff_best) {
					diff_best = diff;
				#endif

					// Get lowest trail wt possible with cplex
					this_wt = find_cplex(pi, (u16)BF_MIN, best_wt, NULL);

					//~ if (this_wt == best_wt) {
						//~ printf("equivalent matrix : ");
						//~ print_pi(pi); printf("\n");
					//~ }
					if (this_wt >= best_wt) {
						best_wt = this_wt;
						memcpy(best_pi, pi, sizeof(pi));

						#if PRINT_ONLY_BEST == 0
						printf("%d\t%d\t%d\t%d\t%d\t", ROWS, COLS, RHO, ROUNDS, best_wt);
						print_pi(best_pi); printf(",\n");
						#endif
					}

				#if USE_DIFFUSION_FILTER
				}
				#endif
			}

#if RHO > 7
	}
#endif
#if RHO > 6
	}
#endif
#if RHO > 5
	}
#endif
#if RHO > 4
	}
#endif
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

	#if COLLECT_TRAILS
	writeTrailsToPDF(pictureTrails);
	#endif

	#if PRINT_ONLY_BEST
	printf("%d\t%d\t%d\t%d\t%d\t", ROWS, COLS, RHO, ROUNDS, best_wt);
	print_pi(best_pi); printf(",\n");
	#endif
}






























































