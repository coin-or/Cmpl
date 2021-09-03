* CMPL - Free-MPS - Export
NAME 09_Beispiel_Steglich
* OBJNAME line_1
* OBJSENSE MIN
ROWS
 N line_1
 L line_2
 L line_3
 L line_4
 E line_5
 E line_6
 E line_7
 E line_8
 L line_9
 L line_10
 E line_11
 L line_12
 G line_13
 E line_14
 L line_15
 L line_16
 G line_17
 G line_18
COLUMNS
 x[1] line_1 10 line_2 1
 x[1] line_3 1.200000 line_4 1.400000
 x[1] line_11 -1
 x[2] line_1 12 line_2 1
 x[2] line_3 1.800000 line_4 1.300000
 x[2] line_14 -1
 rkn[1,1] line_1 -1.300000 line_5 -1
 rkn[1,1] line_9 1
 rkn[1,2] line_1 -1.300000 line_6 -1
 rkn[1,2] line_9 1
 rkn[2,1] line_1 -1.500000 line_7 -1
 rkn[2,1] line_10 1
 rkn[2,2] line_1 -1.500000 line_8 -1
 rkn[2,2] line_10 1
 xni[1,1] line_5 1.600000 line_7 2.500000
 xni[1,1] line_11 1 line_12 -1
 xni[1,2] line_5 1.200000 line_7 2.200000
 xni[1,2] line_11 1 line_13 -1
 xni[2,1] line_6 2.100000 line_8 3.200000
 xni[2,1] line_14 1 line_15 -1
 xni[2,2] line_6 1.800000 line_8 2.800000
 xni[2,2] line_14 1 line_16 -1
 xni[2,2] line_17 -1
 xni[2,3] line_6 1.700000 line_8 2.600000
 xni[2,3] line_14 1 line_18 -1
 GVANF 'MARKER' 'INTORG'
 y[1,1] line_12 3 line_13 100
 y[2,1] line_15 2 line_17 4
 y[2,2] line_16 4 line_18 100
 GVEND 'MARKER' 'INTEND'
RHS
 RHS line_2 40 line_3 60
 RHS line_4 70 line_9 80
 RHS line_10 100
BOUNDS
 UP BOUND x[1] 100
 UP BOUND x[2] 100
 PL BOUND rkn[1,1]
 PL BOUND rkn[1,2]
 PL BOUND rkn[2,1]
 PL BOUND rkn[2,2]
 UP BOUND xni[1,1] 3
 UP BOUND xni[1,2] 100
 UP BOUND y[1,1] 1
 UP BOUND xni[2,1] 2
 UP BOUND xni[2,2] 4
 UP BOUND xni[2,3] 100
 UP BOUND y[2,1] 1
 UP BOUND y[2,2] 1
ENDATA
