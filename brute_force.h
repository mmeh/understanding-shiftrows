#ifndef BRUTE_FORCE_H_
#define BRUTE_FORCE_H_

u8 combx(u32 *v, u32 *t, u8 *piRow);
u8 next_perm(char *rowPerm);
void next_choice(u8 *buf);
void naive_brute_force_pi();
void brute_force_pi();
void brute_force_pi_noperm();
void n_choose_k();
void bruteforceWithMiddleInverse();
void equivalence_bruteforce();
void test_pi();

#endif
