* CMPL - Free-MPS - Export
NAME 06_SOS-Linear
ROWS
 G __line1
 G __line2
 G __line3
 G __line4
 G __line5
 L __x_sos[1]
 G __line7
 G __line8
 G __line9
 G __line10
 G __line11
 G __line12
 G __line13
 G __line14
 G __line15
 G __line16
 L __testB
 L __line18
 L __line19
 L __line20
 L __line21
 L __line22
 L __line23
 L __line24
 L __line25
 L __line26
 L __line27
 L __testB_sos2
 G __line29
 G __line30
 G __line31
 L __testC
 L __SOS_Row_1
 L __SOS_Row_2
 L __SOS_Row_3
 L __SOS_Col_1
 L __SOS_Col_2
 L __SOS_Col_3
 L __SOS_Col_4
COLUMNS
 x[1] __line1 -1
 x[2] __line2 -1
 x[3] __line3 -1
 x[4] __line4 -1
 x[5] __line5 -1
 y[1] __line7 -1
 y[2] __line8 -1
 y[3] __line9 -1
 y[4] __line10 -1
 y[5] __line11 -1
 y[6] __line12 -1
 y[7] __line13 -1
 y[8] __line14 -1
 y[9] __line15 -1
 y[10] __line16 -1
 GVANF 'MARKER' 'INTORG'
 a __line29 -1
 b __line30 -1
 c __line31 -1
 m[1,1] __SOS_Row_1 1 __SOS_Col_1 1
 m[1,2] __SOS_Row_1 1 __SOS_Col_2 1
 m[1,3] __SOS_Row_1 1 __SOS_Col_3 1
 m[1,4] __SOS_Row_1 1 __SOS_Col_4 1
 m[2,1] __SOS_Row_2 1 __SOS_Col_1 1
 m[2,2] __SOS_Row_2 1 __SOS_Col_2 1
 m[2,3] __SOS_Row_2 1 __SOS_Col_3 1
 m[2,4] __SOS_Row_2 1 __SOS_Col_4 1
 m[3,1] __SOS_Row_3 1 __SOS_Col_1 1
 m[3,2] __SOS_Row_3 1 __SOS_Col_2 1
 m[3,3] __SOS_Row_3 1 __SOS_Col_3 1
 m[3,4] __SOS_Row_3 1 __SOS_Col_4 1
 __x_b[1] __line1 100.000000 __x_sos[1] 1
 __x_b[2] __line2 100.000000 __x_sos[1] 1
 __x_b[3] __line3 100.000000 __x_sos[1] 1
 __x_b[4] __line4 100.000000 __x_sos[1] 1
 __x_b[5] __line5 100.000000 __x_sos[1] 1
 __y_b[1] __line7 10000000000.000000 __testB 1
 __y_b[1] __line18 1 __line19 -1
 __y_b[2] __line8 10000000000.000000 __testB 1
 __y_b[2] __line19 1 __line20 -1
 __y_b[3] __line9 10000000000.000000 __testB 1
 __y_b[3] __line20 1 __line21 -1
 __y_b[4] __line10 10000000000.000000 __testB 1
 __y_b[4] __line21 1 __line22 -1
 __y_b[5] __line11 10000000000.000000 __testB 1
 __y_b[5] __line22 1 __line23 -1
 __y_b[6] __line12 10000000000.000000 __testB 1
 __y_b[6] __line23 1 __line24 -1
 __y_b[7] __line13 10000000000.000000 __testB 1
 __y_b[7] __line24 1 __line25 -1
 __y_b[8] __line14 10000000000.000000 __testB 1
 __y_b[8] __line25 1 __line26 -1
 __y_b[9] __line15 10000000000.000000 __testB 1
 __y_b[9] __line26 1 __line27 -1
 __y_b[10] __line16 10000000000.000000 __testB 1
 __y_b[10] __line27 1
 __y_d[1] __line18 -1 __testB_sos2 1
 __y_d[2] __line19 -1 __testB_sos2 1
 __y_d[3] __line20 -1 __testB_sos2 1
 __y_d[4] __line21 -1 __testB_sos2 1
 __y_d[5] __line22 -1 __testB_sos2 1
 __y_d[6] __line23 -1 __testB_sos2 1
 __y_d[7] __line24 -1 __testB_sos2 1
 __y_d[8] __line25 -1 __testB_sos2 1
 __y_d[9] __line26 -1 __testB_sos2 1
 __y_d[10] __line27 -1 __testB_sos2 1
 __a_b __line29 100.000000 __testC 1
 __b_b __line30 100.000000 __testC 1
 __c_b __line31 100.000000 __testC 1
 GVEND 'MARKER' 'INTEND'
RHS
 RHS __x_sos[1] 1 __testB 2
 RHS __testB_sos2 1 __testC 1
 RHS __SOS_Row_1 1 __SOS_Row_2 1
 RHS __SOS_Row_3 1 __SOS_Col_1 1
 RHS __SOS_Col_2 1 __SOS_Col_3 1
 RHS __SOS_Col_4 1
BOUNDS
 UP BOUND x[1] 100.000000
 UP BOUND x[2] 100.000000
 UP BOUND x[3] 100.000000
 UP BOUND x[4] 100.000000
 UP BOUND x[5] 100.000000
 PL BOUND y[1]
 PL BOUND y[2]
 PL BOUND y[3]
 PL BOUND y[4]
 PL BOUND y[5]
 PL BOUND y[6]
 PL BOUND y[7]
 PL BOUND y[8]
 PL BOUND y[9]
 PL BOUND y[10]
 UP BOUND a 100
 UP BOUND b 100
 UP BOUND c 100
 UP BOUND m[1,1] 1
 UP BOUND m[1,2] 1
 UP BOUND m[1,3] 1
 UP BOUND m[1,4] 1
 UP BOUND m[2,1] 1
 UP BOUND m[2,2] 1
 UP BOUND m[2,3] 1
 UP BOUND m[2,4] 1
 UP BOUND m[3,1] 1
 UP BOUND m[3,2] 1
 UP BOUND m[3,3] 1
 UP BOUND m[3,4] 1
 UP BOUND __x_b[1] 1
 UP BOUND __x_b[2] 1
 UP BOUND __x_b[3] 1
 UP BOUND __x_b[4] 1
 UP BOUND __x_b[5] 1
 UP BOUND __y_b[1] 1
 UP BOUND __y_b[2] 1
 UP BOUND __y_b[3] 1
 UP BOUND __y_b[4] 1
 UP BOUND __y_b[5] 1
 UP BOUND __y_b[6] 1
 UP BOUND __y_b[7] 1
 UP BOUND __y_b[8] 1
 UP BOUND __y_b[9] 1
 UP BOUND __y_b[10] 1
 UP BOUND __y_d[1] 1
 UP BOUND __y_d[2] 1
 UP BOUND __y_d[3] 1
 UP BOUND __y_d[4] 1
 UP BOUND __y_d[5] 1
 UP BOUND __y_d[6] 1
 UP BOUND __y_d[7] 1
 UP BOUND __y_d[8] 1
 UP BOUND __y_d[9] 1
 UP BOUND __y_d[10] 1
 UP BOUND __a_b 1
 UP BOUND __b_b 1
 UP BOUND __c_b 1
ENDATA
