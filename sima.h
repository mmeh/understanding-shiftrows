#ifndef SIMA_H_
#define SIMA_H_

void get_mix_outdiff(word *x, word *y);
void low_weight_input_difference(word *x);
void modify_pi(u8 *pi);
u16 find_low_trail(word *alpha, u8 *pi, u8 num_rounds);
//~ inline void set_random_difference(word *x);
void set_random_pi_matrix(u8 *pi);
void simulated_annealing();
void simulated_annealing_cplex();
//u8 diffusion_rounds(u8 *pi);


#endif
