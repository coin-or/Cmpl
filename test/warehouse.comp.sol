---------------------------------------------------------------------------------------------------------
Problem              warehouse.cmpl
Nr. of variables     28
Nr. of constraints   10
Objective name       line_1
Nr. of solutions     3
Solver name          GUROBI
Display variables    nonzero variables (all)
Display constraints  nonzero constraints (all)
---------------------------------------------------------------------------------------------------------

Solution nr.         1
Objective status     integer optimal solution
Objective value      284 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[S1,K1]                B                   1                   0                   1                   -
x[S1,K4]                B                   1                   0                   1                   -
x[S3,K2]                B                   1                   0                   1                   -
x[S3,K5]                B                   1                   0                   1                   -
x[S3,K6]                B                   1                   0                   1                   -
x[S4,K3]                B                   1                   0                   1                   -
y[S1]                   B                   1                   0                   1                   -
y[S3]                   B                   1                   0                   1                   -
y[S4]                   B                   1                   0                   1                   -
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
line_2                  E                   1                   1                   1                   -
line_3                  E                   1                   1                   1                   -
line_4                  E                   1                   1                   1                   -
line_5                  E                   1                   1                   1                   -
line_6                  E                   1                   1                   1                   -
line_7                  E                   1                   1                   1                   -
line_8                  L                  -5                -inf                   0                   -
line_11                 L                 -45                -inf                   0                   -
---------------------------------------------------------------------------------------------------------

Solution nr.         2
Objective status     integer feasible solution
Objective value      298 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[S1,K1]                B                   1                   0                   1                   -
x[S1,K4]                B                   1                   0                   1                   -
x[S3,K3]                B                   1                   0                   1                   -
x[S3,K5]                B                   1                   0                   1                   -
x[S3,K6]                B                   1                   0                   1                   -
x[S4,K2]                B                   1                   0                   1                   -
y[S1]                   B                   1                   0                   1                   -
y[S3]                   B                   1                   0                   1                   -
y[S4]                   B                   1                   0                   1                   -
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
line_2                  E                   1                   1                   1                   -
line_3                  E                   1                   1                   1                   -
line_4                  E                   1                   1                   1                   -
line_5                  E                   1                   1                   1                   -
line_6                  E                   1                   1                   1                   -
line_7                  E                   1                   1                   1                   -
line_8                  L                  -5                -inf                   0                   -
line_10                 L                 -35                -inf                   0                   -
line_11                 L                 -10                -inf                   0                   -
---------------------------------------------------------------------------------------------------------

Solution nr.         3
Objective status     integer feasible solution
Objective value      347 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[S1,K4]                B                   1                   0                   1                   -
x[S3,K1]                B                   1                   0                   1                   -
x[S3,K2]                B                   1                   0                   1                   -
x[S3,K3]                B                   1                   0                   1                   -
x[S4,K5]                B                   1                   0                   1                   -
x[S4,K6]                B                   1                   0                   1                   -
y[S1]                   B                   1                   0                   1                   -
y[S3]                   B                   1                   0                   1                   -
y[S4]                   B                   1                   0                   1                   -
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
line_2                  E                   1                   1                   1                   -
line_3                  E                   1                   1                   1                   -
line_4                  E                   1                   1                   1                   -
line_5                  E                   1                   1                   1                   -
line_6                  E                   1                   1                   1                   -
line_7                  E                   1                   1                   1                   -
line_8                  L                 -30                -inf                   0                   -
line_10                 L                 -10                -inf                   0                   -
line_11                 L                 -10                -inf                   0                   -
---------------------------------------------------------------------------------------------------------
