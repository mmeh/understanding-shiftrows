#ifndef HELPERS_H_
#define HELPERS_H_

struct trail {
	int pattern[ROUNDS+1][ROWS][COLS];
};

void print_diff(word *x);
void print_pi(u8 *x);
int wt(word x);
int diff_wt(word *x);
u8 col_wt(word *x, u8 col);
void writeTrailsToPDF(std::vector<trail> &trails);
void makeTrailPDF(trail t, char path[100]);



#endif
