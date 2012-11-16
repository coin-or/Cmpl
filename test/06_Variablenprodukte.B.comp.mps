* CMPL - Free-MPS - Export
NAME          06_Variablenprodukte.B.cmpl
ROWS
 N p_int
 E p_int1
 L p_int2
 L p_int3
 G p_int4
 L p_int5
 L p_int6
 G p_int7
 L p_int8
 L p_int9
 G p_int10
 L p_int11
 L p_int12
 G p_int13
 E p_int14
COLUMNS
 ix p_int1 1
 iy p_int3 -1 p_int4 -1
 iy p_int6 -1 p_int7 -1
 iy p_int9 -1 p_int10 -1
 iy p_int12 -1 p_int13 -1
 ix_iy p_int 1 p_int14 1
 _ix_iy[1] p_int2 1 p_int3 1
 _ix_iy[1] p_int4 1 p_int14 -1
 _ix_iy[2] p_int5 1 p_int6 1
 _ix_iy[2] p_int7 1 p_int14 -2
 _ix_iy[3] p_int8 1 p_int9 1
 _ix_iy[3] p_int10 1 p_int14 -4
 _ix_iy[4] p_int11 1 p_int12 1
 _ix_iy[4] p_int13 1 p_int14 -8
 GVANF 'MARKER' 'INTORG'
 _ix[1] p_int1 -1 p_int2 -25
 _ix[1] p_int3 -5 p_int4 -25
 _ix[2] p_int1 -2 p_int5 -25
 _ix[2] p_int6 -5 p_int7 -25
 _ix[3] p_int1 -4 p_int8 -25
 _ix[3] p_int9 -5 p_int10 -25
 _ix[4] p_int1 -8 p_int11 -25
 _ix[4] p_int12 -5 p_int13 -25
 GVEND 'MARKER' 'INTEND'
RHS
 RHS p_int3 -5 p_int4 -25
 RHS p_int6 -5 p_int7 -25
 RHS p_int9 -5 p_int10 -25
 RHS p_int12 -5 p_int13 -25
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
