---------------------------------------------------------------------------------------------------------
Problem              46_tsp_cbc-all.cmpl
Nr. of variables     29
Nr. of constraints   22
Objective name       distance
Solver name          COIN-OR cbc
Display variables    (all)
Display constraints  (all)
---------------------------------------------------------------------------------------------------------

Objective status     optimal
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
sos_i[1]                E                   1                   1                   1                   -
sos_i[2]                E                   1                   1                   1                   -
sos_i[3]                E                   1                   1                   1                   -
sos_i[4]                E                   1                   1                   1                   -
sos_i[5]                E                   1                   1                   1                   -
sos_j[1]                E                   1                   1                   1                   -
sos_j[2]                E                   1                   1                   1                   -
sos_j[3]                E                   1                   1                   1                   -
sos_j[4]                E                   1                   1                   1                   -
sos_j[5]                E                   1                   1                   1                   -
noSubs[2,3]             L                  -2           -Infinity                   4                   -
noSubs[2,4]             L                  -3           -Infinity                   4                   -
noSubs[2,5]             L                   4           -Infinity                   4                   -
noSubs[3,2]             L                   2           -Infinity                   4                   -
noSubs[3,4]             L                   4           -Infinity                   4                   -
noSubs[3,5]             L                   1           -Infinity                   4                   -
noSubs[4,2]             L                   3           -Infinity                   4                   -
noSubs[4,3]             L                   1           -Infinity                   4                   -
noSubs[4,5]             L                   2           -Infinity                   4                   -
noSubs[5,2]             L                   1           -Infinity                   4                   -
noSubs[5,3]             L                   4           -Infinity                   4                   -
noSubs[5,4]             L                  -2           -Infinity                   4                   -
---------------------------------------------------------------------------------------------------------