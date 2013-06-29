---------------------------------------------------------------------------------------------------------
Problem              /Users/mike/Documents/Projekte/Projekte/CMPL/cmpl/examples/transportation-tupel.cmpl
Nr. of variables     8
Nr. of constraints   7
Status               optimal
Solver name          COIN-OR clp
Objective name       costs
Objective value      39500 (min!)
---------------------------------------------------------------------------------------------------------

Nonzero variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[1,1]                  C                3500                   0            Infinity                   0
x[1,2]                  C                1500                   0            Infinity                   0
x[2,2]                  C                2500                   0            Infinity                   0
x[2,3]                  C                2000                   0            Infinity                   0
x[2,4]                  C                1500                   0            Infinity                   0
x[3,1]                  C                2500                   0            Infinity                   0
---------------------------------------------------------------------------------------------------------

Nonzero constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
supplies_1              E                5000                5000                5000                   3
supplies_2              E                6000                6000                6000                   6
supplies_3              E                2500                2500                2500                   2
demands_1               L                6000           -Infinity                6000                   -
demands_2               L                4000           -Infinity                4000                  -1
demands_3               L                2000           -Infinity                2000                  -4
demands_4               L                1500           -Infinity                1500                  -3
---------------------------------------------------------------------------------------------------------
