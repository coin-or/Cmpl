* CMPL - Free-MPS - Export
NAME          tsp.cmpl
OBJSENSE
	min
ROWS
 N distance
 E sos_i_1
 E sos_i_2
 E sos_i_3
 E sos_j_1
 E sos_j_2
 E sos_j_3
 L noSubs_2_3
 L noSubs_3_2
COLUMNS
 u[2] noSubs_2_3 1 noSubs_3_2 -1
 u[3] noSubs_2_3 -1 noSubs_3_2 1
 GVANF 'MARKER' 'INTORG'
 x[1,1] distance 10000 sos_i_1 1
 x[1,1] sos_j_1 1
 x[1,2] distance 76.106591 sos_i_2 1
 x[1,2] sos_j_1 1
 x[1,3] distance 75.531039 sos_i_3 1
 x[1,3] sos_j_1 1
 x[2,1] distance 75.106591 sos_i_1 1
 x[2,1] sos_j_2 1
 x[2,2] distance 10000 sos_i_2 1
 x[2,2] sos_j_2 1
 x[2,3] distance 98.672924 sos_i_3 1
 x[2,3] sos_j_2 1 noSubs_2_3 3
 x[3,1] distance 76.531039 sos_i_1 1
 x[3,1] sos_j_3 1
 x[3,2] distance 97.672924 sos_i_2 1
 x[3,2] sos_j_3 1 noSubs_3_2 3
 x[3,3] distance 10000 sos_i_3 1
 x[3,3] sos_j_3 1
 GVEND 'MARKER' 'INTEND'
RHS
 RHS sos_i_1 1 sos_i_2 1
 RHS sos_i_3 1 sos_j_1 1
 RHS sos_j_2 1 sos_j_3 1
 RHS noSubs_2_3 2 noSubs_3_2 2
RANGES
BOUNDS
 UP BOUND x[1,1] 1
 UP BOUND x[1,2] 1
 UP BOUND x[1,3] 1
 UP BOUND x[2,1] 1
 UP BOUND x[2,2] 1
 UP BOUND x[2,3] 1
 UP BOUND x[3,1] 1
 UP BOUND x[3,2] 1
 UP BOUND x[3,3] 1
 PL BOUND u[2]
 PL BOUND u[3]
ENDATA
