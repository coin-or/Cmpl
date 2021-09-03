---------------------------------------------------------------------------------------------------------
Problem              transport-multi-commodity.cmpl
Nr. of variables     32
Nr. of constraints   54
Objective name       costs
Solver name          GUROBI
Display variables    (all)
Display constraints  (f*)
---------------------------------------------------------------------------------------------------------

Objective status     optimal solution
Objective value      26175 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[W1,U1,Gravel1]        C                  10                   0                 inf                   0
x[W1,U1,Gravel2]        C                  40                   0                 inf                   0
x[W1,U2,Gravel1]        C                  25                   0                 inf                   0
x[W1,U2,Gravel2]        C                   0                   0                 inf                  25
x[U1,U1a,Gravel1]       C                  35                   0                 inf                   0
x[U1,U1a,Gravel2]       C                  65                   0                 inf                   0
x[U2,U2a,Gravel1]       C                  65                   0                 inf                   0
x[U2,U2a,Gravel2]       C                  55                   0                 inf                   0
x[W2,U1,Gravel1]        C                  20                   0                 inf                   0
x[W2,U1,Gravel2]        C                  25                   0                 inf                   0
x[W2,U2,Gravel1]        C                  20                   0                 inf                   0
x[W2,U2,Gravel2]        C                  25                   0                 inf                   0
x[W3,U1,Gravel1]        C                   5                   0                 inf                   0
x[W3,U1,Gravel2]        C                   0                   0                 inf                  13
x[W3,U2,Gravel1]        C                  20                   0                 inf                   0
x[W3,U2,Gravel2]        C                  30                   0                 inf                   0
x[U1a,B1,Gravel1]       C                  15                   0                 inf                   0
x[U1a,B1,Gravel2]       C                   0                   0                 inf                  36
x[U1a,B2,Gravel1]       C                  20                   0                 inf                   0
x[U1a,B2,Gravel2]       C                  15                   0                 inf                   0
x[U1a,B3,Gravel1]       C                   0                   0                 inf                  36
x[U1a,B3,Gravel2]       C                  50                   0                 inf                   0
x[U1a,B4,Gravel1]       C                   0                   0                 inf                  16
x[U1a,B4,Gravel2]       C                   0                   0                 inf                  27
x[U2a,B1,Gravel1]       C                  10                   0                 inf                   0
x[U2a,B1,Gravel2]       C                  30                   0                 inf                   0
x[U2a,B2,Gravel1]       C                   0                   0                 inf                  53
x[U2a,B2,Gravel2]       C                   0                   0                 inf                  87
x[U2a,B3,Gravel1]       C                  40                   0                 inf                   0
x[U2a,B3,Gravel2]       C                   0                   0                 inf                   0
x[U2a,B4,Gravel1]       C                  15                   0                 inf                   0
x[U2a,B4,Gravel2]       C                  25                   0                 inf                   0
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
flows[W1,Gravel1]       E                  35                  35                  35                  75
flows[W1,Gravel2]       E                  40                  40                  40                  88
flows[U1,Gravel1]       E                   0                   0                   0                   0
flows[U1,Gravel2]       E                   0                   0                   0                  -2
flows[U2,Gravel1]       E                   0                   0                   0                   3
flows[U2,Gravel2]       E                   0                   0                   0                   3
flows[W2,Gravel1]       E                  40                  40                  40                  52
flows[W2,Gravel2]       E                  50                  50                  50                  75
flows[W3,Gravel1]       E                  25                  25                  25                  62
flows[W3,Gravel2]       E                  30                  30                  30                  79
flows[U1a,Gravel1]      E                   0                   0                   0                   0
flows[U1a,Gravel2]      E                   0                   0                   0                  -2
flows[B1,Gravel1]       E                 -25                 -25                 -25                 -86
flows[B1,Gravel2]       E                 -30                 -30                 -30                -100
flows[B2,Gravel1]       E                 -20                 -20                 -20                 -43
flows[B2,Gravel2]       E                 -15                 -15                 -15                 -65
flows[B3,Gravel1]       E                 -40                 -40                 -40                -103
flows[B3,Gravel2]       E                 -50                 -50                 -50                -164
flows[B4,Gravel1]       E                 -15                 -15                 -15                 -21
flows[B4,Gravel2]       E                 -25                 -25                 -25                 -28
flows[U2a,Gravel1]      E                   0                   0                   0                   0
flows[U2a,Gravel2]      E                   0                   0                   0                   0
---------------------------------------------------------------------------------------------------------
