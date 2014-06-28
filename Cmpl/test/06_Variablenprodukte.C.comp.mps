* CMPL - Free-MPS - Export
NAME          06_Variablenprodukte.C.cmpl
ROWS
 N p_real
 E p_real[1]
 E p_real[2]
 E p_real[3]
 L p_real[4]
 L p_real[5]
 G p_real[6]
COLUMNS
 rx p_real[1] 1 p_real[3] -5
 ry p_real[2] 1
 rx_ry p_real 1 p_real[3] 1
 _rx p_real[1] -10 p_real[4] -1
 _rx p_real[6] -1
 _ry p_real[2] -20 p_real[5] -1
 _ry p_real[6] -1
 _rx__ry p_real[3] -200 p_real[4] 1
 _rx__ry p_real[5] 1 p_real[6] 1
RHS
 RHS p_real[2] 5 p_real[6] -1
RANGES
BOUNDS
 UP BOUND rx 10
 LO BOUND ry 5
 UP BOUND ry 25
 UP BOUND rx_ry 250
 UP BOUND _rx 1
 UP BOUND _ry 1
 UP BOUND _rx__ry 1
ENDATA