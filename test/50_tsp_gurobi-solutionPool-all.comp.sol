---------------------------------------------------------------------------------------------------------
Problem              50_tsp_gurobi-solutionPool-all.cmpl
Nr. of variables     29
Nr. of constraints   22
Objective name       distance
Nr. of solutions     5
Solver name          Gurobi
---------------------------------------------------------------------------------------------------------

Solution nr.         1
Objective status     integer optimal solution
Objective value      257.945 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[1,1]                  B                   0                   0                   1                   -
x[1,2]                  B                   1                   0                   1                   -
x[1,3]                  B                   0                   0                   1                   -
x[1,4]                  B                   0                   0                   1                   -
x[1,5]                  B                   0                   0                   1                   -
x[2,1]                  B                   0                   0                   1                   -
x[2,2]                  B                   0                   0                   1                   -
x[2,3]                  B                   0                   0                   1                   -
x[2,4]                  B                   0                   0                   1                   -
x[2,5]                  B                   1                   0                   1                   -
x[3,1]                  B                   0                   0                   1                   -
x[3,2]                  B                   0                   0                   1                   -
x[3,3]                  B                   0                   0                   1                   -
x[3,4]                  B                   1                   0                   1                   -
x[3,5]                  B                   0                   0                   1                   -
x[4,1]                  B                   1                   0                   1                   -
x[4,2]                  B                   0                   0                   1                   -
x[4,3]                  B                   0                   0                   1                   -
x[4,4]                  B                   0                   0                   1                   -
x[4,5]                  B                   0                   0                   1                   -
x[5,1]                  B                   0                   0                   1                   -
x[5,2]                  B                   0                   0                   1                   -
x[5,3]                  B                   1                   0                   1                   -
x[5,4]                  B                   0                   0                   1                   -
x[5,5]                  B                   0                   0                   1                   -
u[2]                    C                   0                   0            Infinity                   -
u[3]                    C                   2                   0            Infinity                   -
u[4]                    C                   3                   0            Infinity                   -
u[5]                    C                   1                   0            Infinity                   -
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
sos_i_1                 E                   1                   1                   1                   -
sos_i_2                 E                   1                   1                   1                   -
sos_i_3                 E                   1                   1                   1                   -
sos_i_4                 E                   1                   1                   1                   -
sos_i_5                 E                   1                   1                   1                   -
sos_j_1                 E                   1                   1                   1                   -
sos_j_2                 E                   1                   1                   1                   -
sos_j_3                 E                   1                   1                   1                   -
sos_j_4                 E                   1                   1                   1                   -
sos_j_5                 E                   1                   1                   1                   -
noSubs_2_3              L                  -2           -Infinity                   4                   -
noSubs_2_4              L                  -3           -Infinity                   4                   -
noSubs_2_5              L                   4           -Infinity                   4                   -
noSubs_3_2              L                   2           -Infinity                   4                   -
noSubs_3_4              L                   4           -Infinity                   4                   -
noSubs_3_5              L                   1           -Infinity                   4                   -
noSubs_4_2              L                   3           -Infinity                   4                   -
noSubs_4_3              L                   1           -Infinity                   4                   -
noSubs_4_5              L                   2           -Infinity                   4                   -
noSubs_5_2              L                   1           -Infinity                   4                   -
noSubs_5_3              L                   4           -Infinity                   4                   -
noSubs_5_4              L                  -2           -Infinity                   4                   -
---------------------------------------------------------------------------------------------------------

Solution nr.         2
Objective status     integer feasible solution
Objective value      257.945 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[1,1]                  B                   0                   0                   1                   -
x[1,2]                  B                   1                   0                   1                   -
x[1,3]                  B                   0                   0                   1                   -
x[1,4]                  B                   0                   0                   1                   -
x[1,5]                  B                   0                   0                   1                   -
x[2,1]                  B                   0                   0                   1                   -
x[2,2]                  B                   0                   0                   1                   -
x[2,3]                  B                   0                   0                   1                   -
x[2,4]                  B                   0                   0                   1                   -
x[2,5]                  B                   1                   0                   1                   -
x[3,1]                  B                   0                   0                   1                   -
x[3,2]                  B                   0                   0                   1                   -
x[3,3]                  B                   0                   0                   1                   -
x[3,4]                  B                   1                   0                   1                   -
x[3,5]                  B                   0                   0                   1                   -
x[4,1]                  B                   1                   0                   1                   -
x[4,2]                  B                   0                   0                   1                   -
x[4,3]                  B                   0                   0                   1                   -
x[4,4]                  B                   0                   0                   1                   -
x[4,5]                  B                   0                   0                   1                   -
x[5,1]                  B                   0                   0                   1                   -
x[5,2]                  B                   0                   0                   1                   -
x[5,3]                  B                   1                   0                   1                   -
x[5,4]                  B                   0                   0                   1                   -
x[5,5]                  B                   0                   0                   1                   -
u[2]                    C                   0                   0            Infinity                   -
u[3]                    C                   2                   0            Infinity                   -
u[4]                    C                   3                   0            Infinity                   -
u[5]                    C                   1                   0            Infinity                   -
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
sos_i_1                 E                   1                   1                   1                   -
sos_i_2                 E                   1                   1                   1                   -
sos_i_3                 E                   1                   1                   1                   -
sos_i_4                 E                   1                   1                   1                   -
sos_i_5                 E                   1                   1                   1                   -
sos_j_1                 E                   1                   1                   1                   -
sos_j_2                 E                   1                   1                   1                   -
sos_j_3                 E                   1                   1                   1                   -
sos_j_4                 E                   1                   1                   1                   -
sos_j_5                 E                   1                   1                   1                   -
noSubs_2_3              L                  -2           -Infinity                   4                   -
noSubs_2_4              L                  -3           -Infinity                   4                   -
noSubs_2_5              L                   4           -Infinity                   4                   -
noSubs_3_2              L                   2           -Infinity                   4                   -
noSubs_3_4              L                   4           -Infinity                   4                   -
noSubs_3_5              L                   1           -Infinity                   4                   -
noSubs_4_2              L                   3           -Infinity                   4                   -
noSubs_4_3              L                   1           -Infinity                   4                   -
noSubs_4_5              L                   2           -Infinity                   4                   -
noSubs_5_2              L                   1           -Infinity                   4                   -
noSubs_5_3              L                   4           -Infinity                   4                   -
noSubs_5_4              L                  -2           -Infinity                   4                   -
---------------------------------------------------------------------------------------------------------

Solution nr.         3
Objective status     integer feasible solution
Objective value      10256.1 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[1,1]                  B                   0                   0                   1                   -
x[1,2]                  B                   0                   0                   1                   -
x[1,3]                  B                   0                   0                   1                   -
x[1,4]                  B                   0                   0                   1                   -
x[1,5]                  B                   1                   0                   1                   -
x[2,1]                  B                   0                   0                   1                   -
x[2,2]                  B                   0                   0                   1                   -
x[2,3]                  B                   0                   0                   1                   -
x[2,4]                  B                   1                   0                   1                   -
x[2,5]                  B                   0                   0                   1                   -
x[3,1]                  B                   0                   0                   1                   -
x[3,2]                  B                   0                   0                   1                   -
x[3,3]                  B                   1                   0                   1                   -
x[3,4]                  B                   0                   0                   1                   -
x[3,5]                  B                   0                   0                   1                   -
x[4,1]                  B                   1                   0                   1                   -
x[4,2]                  B                   0                   0                   1                   -
x[4,3]                  B                   0                   0                   1                   -
x[4,4]                  B                   0                   0                   1                   -
x[4,5]                  B                   0                   0                   1                   -
x[5,1]                  B                   0                   0                   1                   -
x[5,2]                  B                   1                   0                   1                   -
x[5,3]                  B                   0                   0                   1                   -
x[5,4]                  B                   0                   0                   1                   -
x[5,5]                  B                   0                   0                   1                   -
u[2]                    C                   1                   0            Infinity                   -
u[3]                    C                   0                   0            Infinity                   -
u[4]                    C                   2                   0            Infinity                   -
u[5]                    C                   0                   0            Infinity                   -
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
sos_i_1                 E                   1                   1                   1                   -
sos_i_2                 E                   1                   1                   1                   -
sos_i_3                 E                   1                   1                   1                   -
sos_i_4                 E                   1                   1                   1                   -
sos_i_5                 E                   1                   1                   1                   -
sos_j_1                 E                   1                   1                   1                   -
sos_j_2                 E                   1                   1                   1                   -
sos_j_3                 E                   1                   1                   1                   -
sos_j_4                 E                   1                   1                   1                   -
sos_j_5                 E                   1                   1                   1                   -
noSubs_2_3              L                   1           -Infinity                   4                   -
noSubs_2_4              L                   4           -Infinity                   4                   -
noSubs_2_5              L                   1           -Infinity                   4                   -
noSubs_3_2              L                  -1           -Infinity                   4                   -
noSubs_3_4              L                  -2           -Infinity                   4                   -
noSubs_3_5              L                   0           -Infinity                   4                   -
noSubs_4_2              L                   1           -Infinity                   4                   -
noSubs_4_3              L                   2           -Infinity                   4                   -
noSubs_4_5              L                   2           -Infinity                   4                   -
noSubs_5_2              L                   4           -Infinity                   4                   -
noSubs_5_3              L                   0           -Infinity                   4                   -
noSubs_5_4              L                  -2           -Infinity                   4                   -
---------------------------------------------------------------------------------------------------------

Solution nr.         4
Objective status     integer feasible solution
Objective value      10258.1 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[1,1]                  B                   0                   0                   1                   -
x[1,2]                  B                   0                   0                   1                   -
x[1,3]                  B                   0                   0                   1                   -
x[1,4]                  B                   1                   0                   1                   -
x[1,5]                  B                   0                   0                   1                   -
x[2,1]                  B                   0                   0                   1                   -
x[2,2]                  B                   0                   0                   1                   -
x[2,3]                  B                   0                   0                   1                   -
x[2,4]                  B                   0                   0                   1                   -
x[2,5]                  B                   1                   0                   1                   -
x[3,1]                  B                   0                   0                   1                   -
x[3,2]                  B                   0                   0                   1                   -
x[3,3]                  B                   1                   0                   1                   -
x[3,4]                  B                   0                   0                   1                   -
x[3,5]                  B                   0                   0                   1                   -
x[4,1]                  B                   0                   0                   1                   -
x[4,2]                  B                   1                   0                   1                   -
x[4,3]                  B                   0                   0                   1                   -
x[4,4]                  B                   0                   0                   1                   -
x[4,5]                  B                   0                   0                   1                   -
x[5,1]                  B                   1                   0                   1                   -
x[5,2]                  B                   0                   0                   1                   -
x[5,3]                  B                   0                   0                   1                   -
x[5,4]                  B                   0                   0                   1                   -
x[5,5]                  B                   0                   0                   1                   -
u[2]                    C                   1                   0            Infinity                   -
u[3]                    C                   0                   0            Infinity                   -
u[4]                    C                   0                   0            Infinity                   -
u[5]                    C                   2                   0            Infinity                   -
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
sos_i_1                 E                   1                   1                   1                   -
sos_i_2                 E                   1                   1                   1                   -
sos_i_3                 E                   1                   1                   1                   -
sos_i_4                 E                   1                   1                   1                   -
sos_i_5                 E                   1                   1                   1                   -
sos_j_1                 E                   1                   1                   1                   -
sos_j_2                 E                   1                   1                   1                   -
sos_j_3                 E                   1                   1                   1                   -
sos_j_4                 E                   1                   1                   1                   -
sos_j_5                 E                   1                   1                   1                   -
noSubs_2_3              L                   1           -Infinity                   4                   -
noSubs_2_4              L                   1           -Infinity                   4                   -
noSubs_2_5              L                   4           -Infinity                   4                   -
noSubs_3_2              L                  -1           -Infinity                   4                   -
noSubs_3_4              L                   0           -Infinity                   4                   -
noSubs_3_5              L                  -2           -Infinity                   4                   -
noSubs_4_2              L                   4           -Infinity                   4                   -
noSubs_4_3              L                   0           -Infinity                   4                   -
noSubs_4_5              L                  -2           -Infinity                   4                   -
noSubs_5_2              L                   1           -Infinity                   4                   -
noSubs_5_3              L                   2           -Infinity                   4                   -
noSubs_5_4              L                   2           -Infinity                   4                   -
---------------------------------------------------------------------------------------------------------

Solution nr.         5
Objective status     integer feasible solution
Objective value      30082.7 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[1,1]                  B                   0                   0                   1                   -
x[1,2]                  B                   0                   0                   1                   -
x[1,3]                  B                   0                   0                   1                   -
x[1,4]                  B                   1                   0                   1                   -
x[1,5]                  B                   0                   0                   1                   -
x[2,1]                  B                   0                   0                   1                   -
x[2,2]                  B                   1                   0                   1                   -
x[2,3]                  B                   0                   0                   1                   -
x[2,4]                  B                   0                   0                   1                   -
x[2,5]                  B                   0                   0                   1                   -
x[3,1]                  B                   0                   0                   1                   -
x[3,2]                  B                   0                   0                   1                   -
x[3,3]                  B                   1                   0                   1                   -
x[3,4]                  B                   0                   0                   1                   -
x[3,5]                  B                   0                   0                   1                   -
x[4,1]                  B                   1                   0                   1                   -
x[4,2]                  B                   0                   0                   1                   -
x[4,3]                  B                   0                   0                   1                   -
x[4,4]                  B                   0                   0                   1                   -
x[4,5]                  B                   0                   0                   1                   -
x[5,1]                  B                   0                   0                   1                   -
x[5,2]                  B                   0                   0                   1                   -
x[5,3]                  B                   0                   0                   1                   -
x[5,4]                  B                   0                   0                   1                   -
x[5,5]                  B                   1                   0                   1                   -
u[2]                    C                   0                   0            Infinity                   -
u[3]                    C                   0                   0            Infinity                   -
u[4]                    C                   0                   0            Infinity                   -
u[5]                    C                   0                   0            Infinity                   -
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
sos_i_1                 E                   1                   1                   1                   -
sos_i_2                 E                   1                   1                   1                   -
sos_i_3                 E                   1                   1                   1                   -
sos_i_4                 E                   1                   1                   1                   -
sos_i_5                 E                   1                   1                   1                   -
sos_j_1                 E                   1                   1                   1                   -
sos_j_2                 E                   1                   1                   1                   -
sos_j_3                 E                   1                   1                   1                   -
sos_j_4                 E                   1                   1                   1                   -
sos_j_5                 E                   1                   1                   1                   -
noSubs_2_3              L                   0           -Infinity                   4                   -
noSubs_2_4              L                   0           -Infinity                   4                   -
noSubs_2_5              L                   0           -Infinity                   4                   -
noSubs_3_2              L                   0           -Infinity                   4                   -
noSubs_3_4              L                   0           -Infinity                   4                   -
noSubs_3_5              L                   0           -Infinity                   4                   -
noSubs_4_2              L                   0           -Infinity                   4                   -
noSubs_4_3              L                   0           -Infinity                   4                   -
noSubs_4_5              L                   0           -Infinity                   4                   -
noSubs_5_2              L                   0           -Infinity                   4                   -
noSubs_5_3              L                   0           -Infinity                   4                   -
noSubs_5_4              L                   0           -Infinity                   4                   -
---------------------------------------------------------------------------------------------------------
