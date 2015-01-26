#ifndef CPLEX_H_
#define CPLEX_H_

#include "heu.h"
#include "helpers.h"
#include <ilcplex/ilocplex.h>
#include <vector>

typedef IloArray<IloBoolVarArray> BoolVar2DMatrix; /* 2-dim matrix of binary variables */
typedef IloArray<BoolVar2DMatrix> BoolVar3DMatrix; /* 3-dim matrix of binary variables */

u16 find_cplex(u8 *pi, u16 wt_best, std::vector<trail> *pictureTrails);
u16 find_cplex(u8 *pi, u16 wt_best);

#endif
