---------------------------------------------------------------------------------------------------------
Problem              shortest-path.cmpl
Nr. of variables     22
Nr. of constraints   7
Objective name       line[1]
Solver name          CBC
Display variables    nonZeroVariables (all)
Display vonstraints  nonZeroConstraints (all)
---------------------------------------------------------------------------------------------------------

Objective status     optimal
Objective value      22.00                (min!)

Variables           
Name                 Type            Activity          LowerBound          UpperBound            Marginal
---------------------------------------------------------------------------------------------------------
x[1,4]                  C                1.00                0.00            Infinity                0.00
x[4,6]                  C                1.00                0.00            Infinity                0.00
x[6,7]                  C                1.00                0.00            Infinity                0.00
---------------------------------------------------------------------------------------------------------

Constraints         
Name                 Type            Activity          LowerBound          UpperBound            Marginal
---------------------------------------------------------------------------------------------------------
node[1]                 E                1.00                1.00                1.00                8.00
node[7]                 E               -1.00               -1.00               -1.00              -14.00
---------------------------------------------------------------------------------------------------------