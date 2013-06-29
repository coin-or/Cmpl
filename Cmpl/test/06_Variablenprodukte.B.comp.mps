* CMPL - Free-MPS - Export
NAME          06_Variablenprodukte.B.cmpl
ROWS
 N p_int
 E p_int[1]
 L p_int[2]
 L p_int[3]
 G p_int[4]
 L p_int[5]
 L p_int[6]
 G p_int[7]
 L p_int[8]
 L p_int[9]
 G p_int[10]
 L p_int[11]
 L p_int[12]
 G p_int[13]
 E p_int[14]
COLUMNS
 ix p_int[1] 1
 iy p_int[3] -1 p_int[4] -1
 iy p_int[6] -1 p_int[7] -1
 iy p_int[9] -1 p_int[10] -1
 iy p_int[12] -1 p_int[13] -1
 ix_iy p_int 1 p_int[14] 1
 _ix_iy[1] p_int[2] 1 p_int[3] 1
 _ix_iy[1] p_int[4] 1 p_int[14] -1
 _ix_iy[2] p_int[5] 1 p_int[6] 1
 _ix_iy[2] p_int[7] 1 p_int[14] -2
 _ix_iy[3] p_int[8] 1 p_int[9] 1
 _ix_iy[3] p_int[10] 1 p_int[14] -4
 _ix_iy[4] p_int[11] 1 p_int[12] 1
 _ix_iy[4] p_int[13] 1 p_int[14] -8
 GVANF 'MARKER' 'INTORG'
 _ix[1] p_int[1] -1 p_int[2] -25
 _ix[1] p_int[3] -5 p_int[4] -25
 _ix[2] p_int[1] -2 p_int[5] -25
 _ix[2] p_int[6] -5 p_int[7] -25
 _ix[3] p_int[1] -4 p_int[8] -25
 _ix[3] p_int[9] -5 p_int[10] -25
 _ix[4] p_int[1] -8 p_int[11] -25
 _ix[4] p_int[12] -5 p_int[13] -25
 GVEND 'MARKER' 'INTEND'
RHS
 RHS p_int[3] -5 p_int[4] -25
 RHS p_int[6] -5 p_int[7] -25
 RHS p_int[9] -5 p_int[10] -25
 RHS p_int[12] -5 p_int[13] -25
RANGES
BOUNDS
 UP BOUND ix 10
 LO BOUND iy 5
 UP BOUND iy 25
 UP BOUND ix_iy 250
 UP BOUND _ix[1] 1
 UP BOUND _ix[2] 1
 UP BOUND _ix[3] 1
 UP BOUND _ix[4] 1
 UP BOUND _ix_iy[1] 25
 UP BOUND _ix_iy[2] 25
 UP BOUND _ix_iy[3] 25
 UP BOUND _ix_iy[4] 25
ENDATA