* CMPL - Free-MPS - Export
NAME          production-mix.cmpl
OBJSENSE
	max
ROWS
 N profit
 L res[1]
 L res[2]
 L line[4]
 L line[5]
 L line[6]
 L line[7]
 L line[8]
 L line[9]
COLUMNS
 x[1] profit 75 res[1] 8
 x[1] res[2] 15 line[4] -1
 x[1] line[7] 1
 x[2] profit 80 res[1] 15
 x[2] res[2] 10 line[5] -1
 x[2] line[8] 1
 x[3] profit 50 res[1] 12
 x[3] res[2] 8 line[6] -1
 x[3] line[9] 1
RHS
 RHS res[1] 1000 res[2] 1000
 RHS line[7] 250 line[8] 240
 RHS line[9] 250
RANGES
BOUNDS
 FR BOUND x[1]
 FR BOUND x[2]
 FR BOUND x[3]
ENDATA
