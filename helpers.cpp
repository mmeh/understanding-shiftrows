#include "heu.h"
#include "helpers.h"
#include <vector>
#include <sys/stat.h>
#include <stdio.h>


///////////////////////////////
//        HELPERS            //
///////////////////////////////

/**
 * Make a black/white grid picture of a trail and store in file
 * using idx number and path
 */
void makeTrailPDF(trail alpha, char path[400]) {
	FILE *f;
    f = fopen(path, "w");
    if (f != NULL) {
		fprintf(f, "\\documentclass{standalone}\n");
		fprintf(f, "\\usepackage{tikz}\n");
		fprintf(f, "\\begin{document}\n");
		fprintf(f, "\\begin{tikzpicture}\n");
		
		for (int t = 0; t < ROUNDS+1; ++t) {
			fprintf(f, "\\begin{scope}[shift={(0,%d)}]\n", -t*(ROWS+1));
			fprintf(f, "\\draw (0,0) grid (%d,-%d);\n", COLS, ROWS);
			
			for (int r = 0; r < ROWS; ++r) {
				for (int c = 0; c < COLS; ++c) {
					if (alpha.pattern[t][r][c])
						fprintf(f, "\\draw[fill] (%d,%d) rectangle (%d,%d);\n", c, -r, c+1, -r-1);
					else
						fprintf(f, "\\draw (%d,%d) rectangle (%d,%d);\n", c, -r, c+1, -r-1);
				}
			}
			
			fprintf(f, "\\end{scope}\n");
		}
		
		fprintf(f, "\\end{tikzpicture}\n");	
		fprintf(f, "\\end{document}\n");
		fclose(f);
		
		// Make PDF
		
	}
}

void writeTrailsToPDF(std::vector<trail> &trails) {
	// Make directory
	char dir[400], cmd[400];
	sprintf(dir, "pictureTrails/%dx%d", ROWS, COLS);
	mkdir(dir, S_IRWXU);
	sprintf(dir, "pictureTrails/%dx%d/%d_rounds", ROWS, COLS, ROUNDS);
	mkdir(dir, S_IRWXU);
	
	for (int trailNum = 0; trailNum < trails.size(); ++trailNum) {
		sprintf(dir, "pictureTrails/%dx%d/%d_rounds/%d_rho_%d.tex", ROWS, COLS, ROUNDS, RHO, trailNum);
		makeTrailPDF(trails[trailNum], dir);
		
		// Make PDF
		sprintf(cmd, "pdflatex -output-directory pictureTrails/%dx%d/%d_rounds pictureTrails/%dx%d/%d_rounds/%d_rho_%d.tex > /dev/null", ROWS, COLS, ROUNDS, ROWS, COLS, ROUNDS, RHO, trailNum);
		system(cmd);
	}
	
	// Remove temp stuff
	sprintf(cmd, "rm pictureTrails/%dx%d/%d_rounds/*.aux pictureTrails/%dx%d/%d_rounds/*.log", ROWS,COLS,ROUNDS,ROWS,COLS,ROUNDS);
	system(cmd);
	
	// Join all PDFs
	sprintf(cmd, "pdfjoin --outfile pictureTrails/%dx%d/%d_rounds/%d_rho.pdf pictureTrails/%dx%d/%d_rounds/%d_rho_*.pdf > /dev/null", ROWS,COLS,ROUNDS,RHO,ROWS, COLS, ROUNDS, RHO);
	system(cmd);
}


/**
 * For printing a difference
 */
void print_diff(word *x) {
	int i,j;
	for (i = 0; i < ROWS; ++i) {
		for (j = 0; j < COLS; ++j)
			printf("%d ", (x[i] >> (COLS-j)) & 1);
		printf("\n");
	}
}

/**
 * For printing Pi
 */
void print_pi(u8 *x) {
	int i,j;
	printf("[");
	for (i = 0; i < RHO; ++i) {
		printf("[");
		for (j = 0; j < ROWS-1; ++j)
			printf("%d,", x[i*ROWS + j]);
		printf("%d]", x[i*ROWS + j]);
		if (i < RHO-1)
			printf(",");
	}
	printf("]");
}

/**
 * Compute the Hamming weight
 */
int wt(word x) {
	return __builtin_popcount(x);
}

/**
 * Get weight of whole difference
 */
int diff_wt(word *x) {
	int i, w = 0;
	for (i = 0; i < ROWS; ++i)
		w += wt(x[i]);
	return w;
}

/**
 * Get weight of particular column in a difference
 */
u8 col_wt(word *x, u8 col) {
	u8 i, w = 0;
	for (i = 0; i < ROWS; ++i)
		w += (x[i] >> col) & 1;
	return w;
}
