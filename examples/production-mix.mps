* CMPL - Free-MPS - Export
NAME          production-mix.cmpl
OBJSENSE
	max
ROWS
 N profit
 L res_1
 L res_2
COLUMNS
 x[1] profit 75 res_1 8
 x[1] res_2 15
 x[2] profit 80 res_1 15
 x[2] res_2 10
 x[3] profit 50 res_1 12
 x[3] res_2 8
RHS
 RHS res_1 1000 res_2 1000
RANGES
BOUNDS
 UP BOUND x[1] 250
 UP BOUND x[2] 240
 UP BOUND x[3] 250
ENDATA
