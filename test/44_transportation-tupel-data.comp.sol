---------------------------------------------------------------------------------------------------------
Problem              44_transportation-tupel-data.cmpl
Nr. of variables     8
Nr. of constraints   7
Objective name       costs
Solver name          COIN-OR clp
---------------------------------------------------------------------------------------------------------

Objective status     optimal
Objective value      39500 (min!)

Variables 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
x[1,1]                  C                3500                   0            Infinity                   0
x[1,2]                  C                1500                   0            Infinity                   0
x[1,4]                  C                   0                   0            Infinity                   6
x[2,2]                  C                2500                   0            Infinity                   0
x[2,3]                  C                2000                   0            Infinity                   0
x[2,4]                  C                1500                   0            Infinity                   0
x[3,1]                  C                2500                   0            Infinity                   0
x[3,3]                  C                   0                   0            Infinity                   6
---------------------------------------------------------------------------------------------------------

Constraints 
Name                 Type            Activity         Lower bound         Upper bound            Marginal
---------------------------------------------------------------------------------------------------------
supplies_1              L                5000           -Infinity                5000                  -3
supplies_2              L                6000           -Infinity                6000                   -
supplies_3              L                2500           -Infinity                2500                  -4
demands_1               E                6000                6000                6000                   6
demands_2               E                4000                4000                4000                   5
demands_3               E                2000                2000                2000                   2
demands_4               E                1500                1500                1500                   3
---------------------------------------------------------------------------------------------------------
