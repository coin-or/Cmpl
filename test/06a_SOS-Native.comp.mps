* CMPL - Free-MPS - Export
NAME 06a_SOS-Native
ROWS
COLUMNS
 GVANF 'MARKER' 'INTORG'
 GVEND 'MARKER' 'INTEND'
RHS
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
SOS
 S1 x_sos[1]
    x[1] 1
    x[2] 2
    x[3] 3
    x[4] 4
    x[5] 5
 S1 testC
    a 1
    b 2
    c 3
 S1 SOS Row 1
    m[1,1] 1
    m[1,2] 2
    m[1,3] 3
    m[1,4] 4
 S1 SOS Row 2
    m[2,1] 1
    m[2,2] 2
    m[2,3] 3
    m[2,4] 4
 S1 SOS Row 3
    m[3,1] 1
    m[3,2] 2
    m[3,3] 3
    m[3,4] 4
 S1 SOS Col 1
    m[1,1] 1
    m[2,1] 2
    m[3,1] 3
 S1 SOS Col 2
    m[1,2] 1
    m[2,2] 2
    m[3,2] 3
 S1 SOS Col 3
    m[1,3] 1
    m[2,3] 2
    m[3,3] 3
 S1 SOS Col 4
    m[1,4] 1
    m[2,4] 2
    m[3,4] 3
 S2 testB
    y[1] 1
    y[2] 2
    y[3] 3
    y[4] 4
    y[5] 5
    y[6] 6
    y[7] 7
    y[8] 8
    y[9] 9
    y[10] 10
ENDATA
