* CMPL - Free-MPS - Export
NAME 07_Variablenprodukte
* OBJNAME p_bin
* OBJSENSE MAX
ROWS
 N p_bin
 G p_int
 G p_real
 L __line4
 L __line5
 G __line6
 E __line7
 L __line8
 L __line9
 G __line10
 L __line11
 L __line12
 G __line13
 L __line14
 L __line15
 G __line16
 L __line17
 L __line18
 G __line19
 E __line20
 E __line21
 E __line22
 L __line23
 L __line24
 G __line25
 E __line26
COLUMNS
 by __line5 -1 __line6 -1
 iy __line9 -1 __line10 -1
 iy __line12 -1 __line13 -1
 iy __line15 -1 __line16 -1
 iy __line18 -1 __line19 -1
 rx __line21 -1 __line26 -5.000000
 ry __line22 -1
 __bx_by p_bin 1 __line4 1
 __bx_by __line5 1 __line6 1
 __ix_iy p_int 1 __line20 -1
 __ix_b_iy[1] __line8 1 __line9 1
 __ix_b_iy[1] __line10 1 __line20 1
 __ix_b_iy[2] __line11 1 __line12 1
 __ix_b_iy[2] __line13 1 __line20 2
 __ix_b_iy[3] __line14 1 __line15 1
 __ix_b_iy[3] __line16 1 __line20 4
 __ix_b_iy[4] __line17 1 __line18 1
 __ix_b_iy[4] __line19 1 __line20 8
 __rx_n __line21 10.000000 __line23 -1
 __rx_n __line25 -1
 __ry_n __line22 20.000000 __line24 -1
 __ry_n __line25 -1
 __rx_ry_n __line23 1 __line24 1
 __rx_ry_n __line25 1 __line26 -200.000000
 __rx_ry p_real 1 __line26 1
 GVANF 'MARKER' 'INTORG'
 bx __line4 -25.000000 __line5 -5.000000
 bx __line6 -25.000000
 ix __line7 -1
 __ix_b[1] __line7 1 __line8 -25.000000
 __ix_b[1] __line9 -5.000000 __line10 -25.000000
 __ix_b[2] __line7 2 __line11 -25.000000
 __ix_b[2] __line12 -5.000000 __line13 -25.000000
 __ix_b[3] __line7 4 __line14 -25.000000
 __ix_b[3] __line15 -5.000000 __line16 -25.000000
 __ix_b[4] __line7 8 __line17 -25.000000
 __ix_b[4] __line18 -5.000000 __line19 -25.000000
 GVEND 'MARKER' 'INTEND'
RHS
 RHS p_int -1.79769e+308 p_real -1.79769e+308
 RHS __line5 -5.000000 __line6 -25.000000
 RHS __line9 -5.000000 __line10 -25.000000
 RHS __line12 -5.000000 __line13 -25.000000
 RHS __line15 -5.000000 __line16 -25.000000
 RHS __line18 -5.000000 __line19 -25.000000
 RHS __line22 -5.000000 __line25 -1
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
 UP BOUND __ix_b[1] 1
 UP BOUND __ix_b[2] 1
 UP BOUND __ix_b[3] 1
 UP BOUND __ix_b[4] 1
 UP BOUND __ix_iy 250.000000
 UP BOUND __ix_b_iy[1] 25.000000
 UP BOUND __ix_b_iy[2] 25.000000
 UP BOUND __ix_b_iy[3] 25.000000
 UP BOUND __ix_b_iy[4] 25.000000
 UP BOUND __rx_n 1
 UP BOUND __ry_n 1
 UP BOUND __rx_ry_n 1
 UP BOUND __rx_ry 250.000000
ENDATA
