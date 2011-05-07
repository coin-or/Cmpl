* CMPL - MPS - Export
NAME          09_Beispiel_Steglich.mps
ROWS
 N  line_1  
 L  line_2  
 L  line_3  
 L  line_4  
 E  RS_1_1  
 E  RS_1_2  
 E  RS_2_1  
 E  RS_2_2  
 L  line_9  
 L  line_10 
 E  line_11 
 G  line_12 
 L  line_13 
 E  line_14 
 G  line_15 
 G  line_16 
 L  line_17 
 L  line_18 
COLUMNS
    x1        line_1              10   line_2        1.000000
    x1        line_3        1.200000   line_4        1.400000
    x1        line_11              1
    x2        line_1              12   line_2        1.000000
    x2        line_3        1.800000   line_4        1.300000
    x2        line_14              1
    rkn1_1    line_1       -1.300000   RS_1_1               1
    rkn1_1    line_9               1
    rkn1_2    line_1       -1.300000   RS_1_2               1
    rkn1_2    line_9               1
    rkn2_1    line_1       -1.500000   RS_2_1               1
    rkn2_1    line_10              1
    rkn2_2    line_1       -1.500000   RS_2_2               1
    rkn2_2    line_10              1
    xni1_1    RS_1_1       -1.600000   RS_2_1       -2.500000
    xni1_1    line_11             -1   line_12              1
    xni1_2    RS_1_1       -1.200000   RS_2_1       -2.200000
    xni1_2    line_11             -1   line_13              1
    xni2_1    RS_1_2       -2.100000   RS_2_2       -3.200000
    xni2_1    line_14             -1   line_15              1
    xni2_2    RS_1_2       -1.800000   RS_2_2       -2.800000
    xni2_2    line_14             -1   line_16              1
    xni2_2    line_17              1
    xni2_3    RS_1_2       -1.700000   RS_2_2       -2.600000
    xni2_3    line_14             -1   line_18              1
    GVANF     'MARKER'                 'INTORG'
    y1_1      line_12             -3   line_13           -100
    y2_1      line_15             -2   line_17             -4
    y2_2      line_16             -4   line_18           -100
    GVEND     'MARKER'                 'INTEND'
RHS
    RHS       line_2              40   line_3              60
    RHS       line_4              70   line_9              80
    RHS       line_10            100
RANGES
BOUNDS
 UP BOUND     x1                 100
 UP BOUND     x2                 100
 PL BOUND     rkn1_1  
 PL BOUND     rkn1_2  
 PL BOUND     rkn2_1  
 PL BOUND     rkn2_2  
 UP BOUND     xni1_1               3
 UP BOUND     xni1_2             100
 UP BOUND     y1_1                 1
 UP BOUND     xni2_1               2
 UP BOUND     xni2_2               4
 UP BOUND     xni2_3             100
 UP BOUND     y2_1                 1
 UP BOUND     y2_2                 1
ENDATA
