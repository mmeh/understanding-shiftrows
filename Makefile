SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio126/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio126/concert

CCC = g++
CCOPT = -std=c++11 -march=native -Wno-unused-result -O3 -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNDIRS  = -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)
CCLNFLAGS = -lconcert -lilocplex -lcplex -lm -lpthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)

main: heu.o cplex.o brute_force.o sima.o helpers.o diffusion.o
	@$(CCC) $(PARS) $(CCFLAGS) $(CCLNDIRS) heu.o cplex.o brute_force.o sima.o helpers.o diffusion.o -o heu $(CCLNFLAGS)
bf: heu.o cplex.o brute_force.o helpers.o diffusion.o
	@$(CCC) $(PARS) $(CCFLAGS) $(CCLNDIRS) heu.o cplex.o brute_force.o helpers.o diffusion.o -o heu $(CCLNFLAGS)
heu.o: heu.cpp
	@$(CCC) $(PARS) -c $(CCFLAGS) heu.cpp -o heu.o
cplex.o: cplex.cpp
	@$(CCC) $(PARS) -c $(CCFLAGS) cplex.cpp -o cplex.o
brute_force.o: brute_force.cpp
	@$(CCC) $(PARS) -c $(CCFLAGS) brute_force.cpp -o brute_force.o
sima.o: sima.cpp
	@$(CCC) $(PARS) -c $(CCFLAGS) sima.cpp -o sima.o
helpers.o: helpers.cpp
	@$(CCC) $(PARS) -c $(CCFLAGS) helpers.cpp -o helpers.o
diffusion.o: diffusion.cpp
	@$(CCC) $(PARS) -c $(CCFLAGS) diffusion.cpp -o diffusion.o

clean :
	/bin/rm -rf heu *.o
