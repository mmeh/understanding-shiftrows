#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#define R 4
#define C 4
#define RHO 1

typedef uint8_t u8;
typedef uint32_t u32;

void print_tuple(u8 *t) {
    size_t i;
    for (i = 0; i < R; i++) {
        printf("%d ", t[i]);
    }
    printf("\n");
}

void swap(u8 *a, u8 *b) {
    u8 tmp = *a;
    *a = *b;
    *b = tmp;
}

void permutations(u8 *in, size_t curr, size_t size) {

    size_t i;
    if(curr == size-1)
    {
        print_tuple(in);
    } else {
        for(i = curr; i < size; i++)
        {
            swap(&in[curr], &in[i]);
            permutations(in, curr+1, size);
            swap(&in[curr], &in[i]);
        }
    }
}

void next_choice(u8 *buf) {
	size_t i, t = R - 1;
	while (t != 0 && buf[t] == C - R + t) {
		t--;
	}
	buf[t]++;
	for (i = t + 1; i < R; i++) {
		buf[i] = buf[i - 1] + 1;
	}
}

void combinations() {
    u8 buf[RHO * R];
    u8 pi[RHO * R];

    size_t i, j, ctr = 0;

    for (i = 0; i < R; i++) {
		buf[i] = i;
	}
    while (buf[R - 1] < C) {
        memcpy(pi, buf, R*sizeof(u8));
        do {

			for (i = 0; i < R; i++) {
				(buf+R)[i] = i;
			}

			while ((buf+R)[R - 1] < C) {
				memcpy(pi+R, buf+R, R*sizeof(u8));
				do {

					print_tuple(pi);
					print_tuple(pi+R);
					printf("\n");
					ctr++;

				} while (std::next_permutation(pi+R,pi+2*R));
				next_choice(buf+R);
			}




        } while (std::next_permutation(pi,pi+R));
		next_choice(buf);
    }

    printf("ctr = %d\n", ctr);
}

void n_choose_k() {
	u8 i, j[RHO];
	u32 v[RHO], t[RHO];
	u32 endMask = ((1 << R) - 1) << (C - R);

	for (i = 0; i < RHO; ++i)
		v[i] = (1 << R) - 1;

	u8 pi[RHO*R];

	do {
		t[0] = v[0];
		for (j[0] = 0; j[0] < R; ++j[0]) {
			pi[j[0]] = __builtin_ctz(t[0]);
			t[0] ^= (1 << __builtin_ctz(t[0]));
		}
#if RHO > 1
		do {
			t[1] = v[1];
			for (j[1] = 0; j[1] < R; ++j[1]) {
				pi[R+j[1]] = __builtin_ctz(t[1]);
				t[1] ^= (1 << __builtin_ctz(t[1]));
			}
#endif
#if RHO > 2
			do {
				t[2] = v[2];
				for (j[2] = 0; j[2] < R; ++j[2]) {
					pi[2*R+j[2]] = __builtin_ctz(t[2]);
					t[2] ^= (1 << __builtin_ctz(t[2]));
				}
#endif
#if RHO > 3
				do {
					t[3] = v[3];
					for (j[3] = 0; j[3] < R; ++j[3]) {
						pi[3*R+j[3]] = __builtin_ctz(t[3]);
						t[3] ^= (1 << __builtin_ctz(t[3]));
					}
#endif

					// Do something




#if RHO > 3
					t[3] = v[3] | (v[3] - 1);
					v[3] = (t[3] + 1) | (((~t[3] & -~t[3]) - 1) >> (__builtin_ctz(v[3]) + 1));
				} while (v[3] != endMask);
#endif
#if RHO > 3
				t[2] = v[2] | (v[2] - 1);
				v[2] = (t[2] + 1) | (((~t[2] & -~t[2]) - 1) >> (__builtin_ctz(v[2]) + 1));
			} while (v[2] != endMask);
#endif
#if RHO > 3
			t[1] = v[1] | (v[1] - 1);
			v[1] = (t[1] + 1) | (((~t[1] & -~t[1]) - 1) >> (__builtin_ctz(v[1]) + 1));
		} while (v[1] != endMask);
#endif
		t[0] = v[0] | (v[0] - 1);
		v[0] = (t[0] + 1) | (((~t[0] & -~t[0]) - 1) >> (__builtin_ctz(v[0]) + 1));
	} while (v[0] != endMask);
}


int main()
{
    //~ combinations();
    n_choose_k();
	return 0;
}
