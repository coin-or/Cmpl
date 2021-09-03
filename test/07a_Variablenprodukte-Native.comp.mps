* CMPL - Free-MPS - Export
NAME 07a_Variablenprodukte-Native
OBJSENSE
	MAX
OBJNAME
	p_bin
ROWS
 N p_bin
 G p_int
 G p_real
 L __line4
 L __line5
 G __line6
COLUMNS
 by __line5 -1 __line6 -1
 __bx_by p_bin 1 __line4 1
 __bx_by __line5 1 __line6 1
 GVANF 'MARKER' 'INTORG'
 bx __line4 -25.000000 __line5 -5.000000
 bx __line6 -25.000000
 GVEND 'MARKER' 'INTEND'
RHS
 RHS p_int -1.79769e+308 p_real -1.79769e+308
 RHS __line5 -5.000000 __line6 -25.000000
BOUNDS
 UP BOUND bx 1
 LO BOUND by 5.000000
 UP BOUND by 25.000000
 UP BOUND ix 10
 LO BOUND iy 5.000000
 UP BOUND iy 25.000000
 UP BOUND rx 10.000000
 LO BOUND ry 5.000000
 UP BOUND ry 25.000000
 UP BOUND __bx_by 25.000000
QCMATRIX p_int
 ix iy 1
 iy ix 1
QCMATRIX p_real
 rx ry 1
 ry rx 1
ENDATA
