---------------------------------------------------------------------------------------------------------
Problem              transport-foster-single-source.cmpl
Nr. of variables     12
Nr. of constraints   7
Objective name       cost
Nr. of solutions     2
Solver name          CPLEX
Display variables    nonzero variables (all)
Display constraints  nonzero constraints (all)
---------------------------------------------------------------------------------------------------------

Solution nr.         1
Objective status     integer optimal solution
Objective value      65500 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
y[1,2]                  B                   1                   0                   1                   -
y[2,1]                  B                   1                   0                   1                   -
y[3,3]                  B                   1                   0                   1                   -
y[3,4]                  B                   1                   0                   1                   -
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
supplies[1]             L                4000                -inf                5000                   -
supplies[2]             L                6000                -inf                6000                   -
supplies[3]             L                3500                -inf                5000                   -
demands[1]              E                   1                   1                   1                   -
demands[2]              E                   1                   1                   1                   -
demands[3]              E                   1                   1                   1                   -
demands[4]              E                   1                   1                   1                   -
---------------------------------------------------------------------------------------------------------

Solution nr.         2
Objective status     integer feasible solution
Objective value      65500 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
y[1,2]                  B                   1                   0                   1                   -
y[2,1]                  B                   1                   0                   1                   -
y[3,3]                  B                   1                   0                   1                   -
y[3,4]                  B                   1                   0                   1                   -
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
supplies[1]             L                4000                -inf                5000                   -
supplies[2]             L                6000                -inf                6000                   -
supplies[3]             L                3500                -inf                5000                   -
demands[1]              E                   1                   1                   1                   -
demands[2]              E                   1                   1                   1                   -
demands[3]              E                   1                   1                   1                   -
demands[4]              E                   1                   1                   1                   -
---------------------------------------------------------------------------------------------------------
