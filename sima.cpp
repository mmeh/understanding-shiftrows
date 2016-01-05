#include "heu.h"
#include "sima.h"
#include "helpers.h"
#include "cplex.h"
#include "diffusion.h"
#include <string.h>
#include <stdio.h>


///////////////////////////////
// SIMULATED ANNEALING STUFF //
///////////////////////////////

std::vector<u8> piRow;

/**
 * Get random feasible output difference from MixColumns
 * using the given input difference alpha.
 */
void get_mix_outdiff(word *x, word *y) {
	u8 i, j, c_wt, new_col_wt, npos;

	// Reset y
	memset(y, 0, ROWS*sizeof(word));

	//// Get active cols
	//active_cols.clear();
	//word z = 0;
	//for (i = 0; i < ROWS; ++i)
		//z |= x[i];
	//c_wt = wt(z);
	//for (i = 0; i < c_wt; ++i) {
		//active_cols.insert(__builtin_ctz(z));
		//z ^= (1 << __builtin_ctz(z));
	//}

	//for (auto ac = active_cols.cbegin(); ac != active_cols.cend(); ++ac) {
		////printf("%d\n", *ac);
		//c_wt = col_wt(x, *ac);
		//new_col_wt = (BRANCH_NUM - c_wt) + (rand() % (ROWS - BRANCH_NUM - c_wt + 1));

		//npos = rand() & ((1 << ROWS)-1);
		//while (wt(npos) != new_col_wt)
			//npos = rand() & ((1 << ROWS)-1);

		//for (i = 0; i < new_col_wt; ++i) {
			//y[__builtin_ctz(npos)] |= (1 << *ac);
			//npos ^= (1 << __builtin_ctz(npos));
		//}
	//}

	for (i = 0; i < COLS; ++i) {
		c_wt = col_wt(x, i);
		if (c_wt == 0)
			continue;
		new_col_wt = (BRANCH_NUM - c_wt) + (rand() % (ROWS - BRANCH_NUM - c_wt + 1));
		npos = rand() & ((1 << ROWS)-1);

		while (wt(npos) != new_col_wt)
			npos = rand() & ((1 << ROWS)-1);

		for (j = 0; j < new_col_wt; ++j) {
			y[__builtin_ctz(npos)] |= (1 << i);
			npos ^= (1 << __builtin_ctz(npos));
		}
	}
}

/**
 * For finding a trail of low weight using some given input difference
 * for some specific # of rounds, using a specified Pi matrix
 */
u16 find_low_trail(word *alpha, u8 *pi, u8 num_rounds) {
	u8 i, j, r, npos;
	u16 trail_wt=0, c_wt, new_col_wt;
	word x[ROWS], y[ROWS];

	// Copy input difference to x
	memcpy(x, alpha, ROWS*sizeof(word));

	for (r = 0; r < num_rounds; ++r) {
		// Add weight before MixColumns to trail weight
		trail_wt += diff_wt(x);

		// MixColumns
		// Pick arbitrary feasible output and store in y
		get_mix_outdiff(x, y);

		// ShiftRows
		// Deterministic
		for (i = 0; i < ROWS; ++i)
			x[i] = ROR(y[i], pi[(r % RHO)*ROWS + i]);
	}

	return trail_wt;
}

/**
 * Construct a random difference
 */
//~ inline void set_random_difference(word *x) {
	//~ int i;
	//~ for (i = 0; i < ROWS; ++i)
		//~ x[i] = rand() & WORD_MASK;
//~ }

/**
 * Modify Pi matrix slightly (for SA)
 */
void modify_pi(u8 *pi) {
	u8 r = rand() % RHO;
	u8 c = r > 0 ? rand() % ROWS : 1 + rand() % (ROWS-1);
	u8 i,j,x;

	// Insert an element between (a;b) where a is the element left of
	// Pi_[r,c] and b is the element to the right
	// If c = 0 or c = R-1 then bounds are 0 and C-1 respectively
	u8 lb, ub;

	// Get upper/lower bounds on the number to insert
	switch (c) {
		case 0:
			lb = 0;
			ub = pi[r*ROWS + c + 1] - 1;
			break;
		case ROWS-1:
			lb = pi[r*ROWS + c - 1] + 1;
			ub = COLS - 1;
			break;
		default:
			lb = pi[r*ROWS + c - 1] + 1;
			ub = pi[r*ROWS + c + 1] - 1;
			break;
	}

	//~ printf("r,c,lb,ub = %d, %d, %d, %d\n", r,c,lb,ub);

	pi[r*ROWS + c] = (rand() % (ub-lb+1)) + lb;

	//~ do {
		//~ x = rand() % COLS;
		//~ i = 0;
		//~ for (j = 0; j < ROWS; ++j) {
			//~ if (pi[r*ROWS + j] == x)
				//~ break;
			//~ i++;
		//~ }
	//~ } while (i < ROWS);
	//~ pi[r*RHO + c] = x;
	//pi[rand() % (RHO*R)] = rand() % COLS;
}

/**
 * Determine a randomly chosen input difference with weight between 1
 * and MAX_INPUT_DIFF_WEIGHT.
 */
void low_weight_input_difference(word *x) {
	u8 in_wt = 1 + rand() % (MAX_INPUT_DIFF_WEIGHT - 1);
	u8 pr, pc, set = 0;
	memset(x, 0, ROWS*sizeof(word));
	while (set < in_wt) {
		pr = rand() % ROWS;
		pc = rand() % COLS;

		if (!(x[pr] >> pc & 1)) {
			x[pr] |= (1 << pc);
			set++;
		}
	}
}

/**
 * Construct random Pi matrix.
 * We makre sure to have Pi_0,0 = 0
 * and also that all elements in a row a different,
 * and also that they are just in increasing order
 */
void set_random_pi_matrix(u8 *pi) {
	u8 i,j;

	random_shuffle(piRow.begin(), piRow.end());
	sort(piRow.begin(), piRow.begin() + ROWS);

	// Row 0
	// Always keep a 0 in Pi_[0,0]
	i = 0;
	for (j = 1; j < ROWS; ++j)
		pi[ROWS*i + j] = piRow[j] == 0 ? piRow[COLS-j] : piRow[j];

	// Row 1..RHO-1
	for (i = 1; i < RHO; ++i) {
		// Shufle vector with elements 0..COLS-1
		random_shuffle(piRow.begin(), piRow.end());
		sort(piRow.begin(), piRow.begin() + ROWS);

		// Pick row Pi_i as first R elements of shuffled vector
		for (j = 0; j < ROWS; ++j)
			pi[ROWS*i + j] = piRow[j];
	}
}

void new_input_difference(word *x) {
	low_weight_input_difference(x);
}

/**
 * Run simulated annealing.
 * Start with a random Pi matrix, then run some kind of algorithm
 * to minimize trail weight for some # of rounds using this Pi matrix.
 * If the one found was better, store this Pi matrix. Then modify the
 * Pi matrix slightly and run again.
 */
void simulated_annealing() {
    printf("=== Simulated Annealing (Standard) ===\n");
	// Initialize variables
	int i,j;
	u8 pi[RHO*ROWS] = { 0 }, pi_best[RHO*ROWS];
	word alpha[ROWS];
	u16 wt, wtx, wt_best = 0,
		diff, diff_best = (1 << 15); // some large value

	u64 ctr_inner, ctr_outer = 0;

	// Pick random Pi matrix
	set_random_pi_matrix(pi);

	// Just run forever, man...
	while (1) {
		// Determine rounds required for full diffusion
		diff = diffusion_rounds(pi);

		// If diffusion is at least as good as the current best, continue
		if (diff <= diff_best) {
			diff_best = diff;

			// wtx will hold the lowest trail weight across
			// many input differences for a fixed Pi.
			// Initialize to the MAX possible value
			wtx = ROWS * COLS * ROUNDS;

			// Using a given Pi matrix, try tunning with many
			// input differences
			for (ctr_inner = 0; ctr_inner < SA_INNER_LOOPS; ++ctr_inner) {
				// Pick input difference alpha
				new_input_difference(alpha);

				// Find trail weight for this particular input difference
				wt = find_low_trail(alpha, pi, ROUNDS);

				// If trail is lighter, store the weight in wtx as the best so far
				wtx = std::min(wt, wtx);
			}

			// Check results
			if (wtx > wt_best) {
				// Store best results
				wt_best = wtx;
				memcpy(pi_best, pi, sizeof(pi));

				// Inform us
				printf("%d\t%d\t%d\t%d\t%d\t", ROWS, COLS, ROUNDS, RHO, wt_best);
				print_pi(pi_best); printf(",\n");
			}
		}

		// Flip entry in Pi
		modify_pi(pi);

		//ctr_outer++;
	}
}


void simulated_annealing_cplex() {
    printf("=== Simulated Annealing (CPLEX) ===\n");
	// Initialize variables
	int i,j;
	u8 pi[RHO*ROWS] = {0}, pi_best[RHO*ROWS];
	word alpha[ROWS];
	u16 wt, wtx, wt_best = 0,
		diff, diff_best = (1 << 15); // some large value
    size_t ctr = 0;

	// Pick random Pi matrix
	set_random_pi_matrix(pi);

	// Run
	while (1) {
		// Choose completely fresh Pi every now and then..
        if (!(ctr & 0xFF)) {
	        set_random_pi_matrix(pi);
            printf("pi switched\n");
        }

        // Determine rounds required for full diffusion
		diff = diffusion_rounds(pi);

		// If diffusion is at least as good as the current best, continue
		if (diff <= diff_best) {
			diff_best = diff;

			// Find the weight of lightest trail using this Pi matrix by running CPLEX
			// providing a lower bound as input to the model
			wtx = find_cplex(pi, (u16)BF_MIN, wt_best);

			// Check results
			if (wtx > wt_best) {
				// Store best results
				wt_best = wtx;
				memcpy(pi_best, pi, sizeof(pi));

				// Inform us
				printf("%d\t%d\t%d\t%d\t%d\t", ROWS, COLS, ROUNDS, RHO, wt_best);
				print_pi(pi_best); printf(",\n");
			}
		}

		// Flip entry in Pi
		modify_pi(pi);
        ctr++;
	}
}
