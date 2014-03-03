* CMPL - Free-MPS - Export
NAME          09_Beispiel_Steglich.cmpl
ROWS
 N line[1]
 L line[2]
 L line[3]
 L line[4]
 E RS[1,1]
 E RS[1,2]
 E RS[2,1]
 E RS[2,2]
 L line[9]
 L line[10]
 E line[11]
 G line[12]
 L line[13]
 E line[14]
 G line[15]
 G line[16]
 L line[17]
 L line[18]
COLUMNS
 x[1] line[1] 10 line[2] 1.000000
 x[1] line[3] 1.200000 line[4] 1.400000
 x[1] line[11] 1
 x[2] line[1] 12 line[2] 1.000000
 x[2] line[3] 1.800000 line[4] 1.300000
 x[2] line[14] 1
 rkn[1,1] line[1] -1.300000 RS[1,1] 1
 rkn[1,1] line[9] 1
 rkn[1,2] line[1] -1.300000 RS[1,2] 1
 rkn[1,2] line[9] 1
 rkn[2,1] line[1] -1.500000 RS[2,1] 1
 rkn[2,1] line[10] 1
 rkn[2,2] line[1] -1.500000 RS[2,2] 1
 rkn[2,2] line[10] 1
 xni[1,1] RS[1,1] -1.600000 RS[2,1] -2.500000
 xni[1,1] line[11] -1 line[12] 1
 xni[1,2] RS[1,1] -1.200000 RS[2,1] -2.200000
 xni[1,2] line[11] -1 line[13] 1
 xni[2,1] RS[1,2] -2.100000 RS[2,2] -3.200000
 xni[2,1] line[14] -1 line[15] 1
 xni[2,2] RS[1,2] -1.800000 RS[2,2] -2.800000
 xni[2,2] line[14] -1 line[16] 1
 xni[2,2] line[17] 1
 xni[2,3] RS[1,2] -1.700000 RS[2,2] -2.600000
 xni[2,3] line[14] -1 line[18] 1
 GVANF 'MARKER' 'INTORG'
 y[1,1] line[12] -3 line[13] -100
 y[2,1] line[15] -2 line[17] -4
 y[2,2] line[16] -4 line[18] -100
 GVEND 'MARKER' 'INTEND'
RHS
 RHS line[2] 40 line[3] 60
 RHS line[4] 70 line[9] 80
 RHS line[10] 100
RANGES
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