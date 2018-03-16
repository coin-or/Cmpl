---------------------------------------------------------------------------------------------------------
Problem              assignment.cmpl
Nr. of variables     11
Nr. of constraints   7
Objective name       costs
Solver name          CBC
Display variables    nonZeroVariables (all)
Display constraints  nonZeroConstraints (all)
---------------------------------------------------------------------------------------------------------

Objective status     optimal
Objective value      29.00               (min!)

Variables           
Name                 Type            Activity          LowerBound          UpperBound            Marginal
---------------------------------------------------------------------------------------------------------
x[1,4]                  B                   1                0.00                1.00                   -
x[2,3]                  B                   1                0.00                1.00                   -
x[3,1]                  B                   1                0.00                1.00                   -
---------------------------------------------------------------------------------------------------------

Constraints         
Name                 Type            Activity          LowerBound          UpperBound            Marginal
---------------------------------------------------------------------------------------------------------
sos_m[1]                E                1.00                1.00                1.00                   -
sos_m[2]                E                1.00                1.00                1.00                   -
sos_m[3]                E                1.00                1.00                1.00                   -
sos_l[1]                L                1.00                -inf                1.00                   -
sos_l[3]                L                1.00                -inf                1.00                   -
sos_l[4]                L                1.00                -inf                1.00                   -
---------------------------------------------------------------------------------------------------------