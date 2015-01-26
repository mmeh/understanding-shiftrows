#include "heu.h"
#include "brute_force.h"
#include "sima.h"
#include "cplex.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>

int main() {
	// Seed rand()
	srand(time(NULL));

	// Make a vector with elements 0..COLS-1
	u8 i;
	for (i = 0; i < COLS; ++i)
		piRow.push_back(i);

	//~ simulated_annealing_cplex();
	//~ simulated_annealing();

	//~ n_choose_k();
	//~ equivalence_bruteforce();
    //test_pi();

	//~ brute_force_pi_noperm();
	//~ brute_force_pi();
	//~ naive_brute_force_pi();

	//~ u8 pi[RHO*ROWS] = {0,1,0,8,0,8,0,4,0,4,0,8,0,8,0,2,0,2,0,8,0,8,0,4,0,4,0,8,0,8,0,1};
	//~ u8 pi[RHO*ROWS] = {0,1,2,3,4,5,6,7};
	u8 pi[RHO*ROWS] = {0,1,2,3};
	printf("%d\n", find_cplex(pi, (u16)BF_MIN));

	//~ bruteforceWithMiddleInverse();
}
