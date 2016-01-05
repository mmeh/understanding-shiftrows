#include <ilcplex/ilocplex.h>

#include "cplex.h"
#include "heu.h"
#include "helpers.h"


// For CPLEX to work
ILOSTLBEGIN

///////////////////////////////
//        CPLEX STUFF        //
///////////////////////////////

//ILOBARRIERCALLBACK1(MyCallback, IloInt, num) {
	//~ cout << "Iteration " << getNiterations() << ": ";
	//~ if ( isFeasible() ) {
	//~ cout << "Objective = " << getObjValue() << endl;
	//~ } else {
	//~ cout << "Infeasibility measure = " << getInfeasibility() << endl;
	//~ }
	//~ cout << "obj val in callback " << getObjValue() << endl;
	//~ cout << "supposed to stop when reaching obj(s) <= " << num <<  endl;
//	if (num != 0 && isFeasible() && getObjValue() >= num)
		//abort();
//}

u16 find_cplex(u8 *pi, u16 lower_bound, u16 best_wt) {
	return find_cplex(pi, lower_bound, best_wt, NULL);
}

/**
 * Interacts with CPLEX by writing a mathematical model
 * using the API and asks it to solve it, returning the objective value
 */
u16 find_cplex(u8 *pi, u16 lower_bound, u16 best_wt, std::vector<trail> *pictureTrails) {
    u16 result = 0;
    size_t t,r,c;
    IloEnv env;

    try {
        IloModel model(env);
        char varName[100];

        /* init state matrix x[T+1][ROWS][COLS] */
        BoolVar3DMatrix x(env, ROUNDS + 1);
        for (t = 0; t <= ROUNDS; ++t) {
            x[t] = BoolVar2DMatrix(env, ROWS);
            for (r = 0; r < ROWS; ++r) {
                x[t][r] = IloBoolVarArray(env, COLS);
                for (c = 0; c < COLS; ++c) {
                    sprintf(varName, "x%lu,%lu,%lu", r, c, t);
                    x[t][r][c].setName(varName);
                }
            }
        }

        /* init matrix a[T][COLS] */
        BoolVar2DMatrix a(env, ROUNDS + 1);
        for (t = 0; t <= ROUNDS; ++t) {
            a[t] = IloBoolVarArray(env, COLS);
            for (c = 0; c < COLS; ++c) {
                sprintf(varName, "a%lu,%lu", c, t);
                a[t][c].setName(varName);
            }
        }

        // (1) \sum_{t,r,c} x^t_{r,c} >= 1
        // This means an active input
        IloIntExpr y(env, 0);
        for (r = 0; r < ROWS; ++r)
           y += IloSum(x[0][r]);

        model.add(y >= 1);
        y.end();

        // (2) \sum_{t,r,c}  x^t_{r,c} + x^{t+1}_{r,c} >= (ROWS+1)*a^t_c
        // This models the MixColumns step
        for (t = 0; t < ROUNDS; ++t) {
            for (c = 0; c < COLS; ++c) {
                IloIntExpr z(env, 0);
                for (r = 0; r < ROWS; ++r)
                    z += x[t][r][c] + x[t+1][r][ (c + pi[ROWS*(t % RHO) + r]) % COLS ];
                model.add(z >= (ROWS+1)*a[t][c]);
                model.add(2*ROWS*a[t][c] >= z); // do we need this?
                z.end();
            }
        }

        // (3) a^t_c >= x^t_{r,c}
        // This ascertains that the auxiliary variable is 1 iff input colimn c
        // in round t is active
        for (t = 0; t < ROUNDS; ++t) {
            for (r = 0; r < ROWS; ++r) {
                for (c = 0; c < COLS; ++c)
                    model.add(a[t][c] >= x[t][r][c]);
            }
        }

		// Initialize minimisation problem
        IloIntExpr weight(env, 0);
        for (t = 0; t < ROUNDS; ++t) {
            for (r = 0; r < ROWS; ++r) {
                for (c = 0; c < COLS; ++c)
                    weight += x[t][r][c];
            }
        }
        model.add(IloMinimize(env, weight));
        
        // Add lower bound on objective value if applicable
        if (lower_bound > 0)
			model.add(weight >= lower_bound);
        
        //~ weight.end();

        // Initiate model to Cplex
        IloCplex cplex(model);

        ////////////////////////
        //   CPLEX SETTINGS   //
        ////////////////////////

        // Set Simplex optimality tolerance
        //~ cplex.setParam(IloCplex::EpOpt, 0.1); // doesn't seem to do anything
        //~ cplex.setParam(IloCplex::RootAlg, IloCplex::Dual);
        //~ cplex.setParam(IloCplex::EpGap, 0.3); // this controls the gap to optimality. dangerous.

        // this controls the optimality proof
        // will skip any potentially new solution which is not at least ARG*100 % better
        //~ cplex.setParam(IloCplex::RelObjDif, 0.1);

		//~ cplex.exportModel("model.lp");		// print model to file

		// Control number of threads
        #if CPLEX_NUM_THREADS > 0
        cplex.setParam(IloCplex::Threads, CPLEX_NUM_THREADS);
        #endif
        
        // Set stopping condition if applicable, i.e. if best solution is far is non-zero
        if (best_wt > 0)
			cplex.setParam(IloCplex::ObjLLim, best_wt);

		// Silence CPLEX
        cplex.setOut(env.getNullStream());
        cplex.setWarning(env.getNullStream());

        //~ IloCplex::Callback mycallback = cplex.use(MyCallback(env, wt_best));
		if (cplex.solve()) {
			result = (u16)cplex.getValue(weight);

			#if COLLECT_TRAILS
			if (result > wt_best)
				pictureTrails.clear();

			if (result >= wt_best) {
				// Add trail to pictureTrails
				trail alpha;

				for (t = 0; t < ROUNDS+1; ++t) {
					for (r = 0; r < ROWS; ++r) {
						for (c = 0; c < COLS; ++c) {
							alpha.pattern[t][r][c] = (int)cplex.getValue(x[t][r][c]);
						}
					}
				}
				pictureTrails.push_back(alpha);
			}
			#endif
		}
    }
    catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
    }
    catch (...) {
      cerr << "Error" << endl;
    }
    env.end();

    return result; /* 0 if no solution was found */
}
