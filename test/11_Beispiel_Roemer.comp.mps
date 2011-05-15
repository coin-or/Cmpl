* CMPL - MPS - Export
NAME          11_Beispiel_Roemer.mps
* OBJNAME Ziel
* OBJSENSE max
ROWS
 N  Ziel    
 L  line_2  
 L  line_3  
 L  line_4  
 L  line_5  
 L  line_6  
 G  line_7  
 G  line_8  
 G  line_9  
 G  line_10 
 G  line_11 
 G  line_12 
 G  line_13 
 G  line_14 
 G  line_15 
 G  line_16 
 G  line_17 
 G  line_18 
 G  line_19 
 G  line_20 
 G  line_21 
 G  line_22 
 G  line_23 
 G  line_24 
 G  line_25 
 G  line_26 
 G  line_27 
 G  line_28 
 G  line_29 
 G  line_30 
 G  line_31 
 G  line_32 
 G  line_33 
 G  line_34 
 G  line_35 
 G  line_36 
 G  line_37 
 G  line_38 
 G  line_39 
 G  line_40 
 G  line_41 
COLUMNS
    x1        Ziel                50   line_2              10
    x1        line_3               5   line_4              20
    x1        line_5              10   line_7       -1.000000
    x1        line_14      -0.500000   line_21      -2.000000
    x1        line_28      -1.000000
    x2        Ziel                80   line_3               8
    x2        line_4              15   line_5              16
    x2        line_15      -0.800000   line_22      -1.500000
    x2        line_29      -1.600000
    x3        Ziel               110   line_2              20
    x3        line_4              18   line_5              17
    x3        line_6              13   line_9       -2.000000
    x3        line_23      -1.800000   line_30      -1.700000
    x3        line_37      -1.300000
    x4        Ziel                75   line_2              10
    x4        line_3              12   line_5              18
    x4        line_6              17   line_10      -1.000000
    x4        line_17      -1.200000   line_31      -1.800000
    x4        line_38      -1.700000
    x5        Ziel               100   line_2              18
    x5        line_3              14   line_4              26
    x5        line_6              18   line_11      -1.800000
    x5        line_18      -1.400000   line_25      -2.600000
    x5        line_39      -1.800000
    x6        Ziel                60   line_2              25
    x6        line_4              17   line_5              20
    x6        line_12      -2.500000   line_26      -1.700000
    x6        line_33      -2.000000
    x7        Ziel                85   line_3              19
    x7        line_4              18   line_5              21
    x7        line_6              14   line_20      -1.900000
    x7        line_27      -1.800000   line_34      -2.100000
    x7        line_41      -1.400000
    q1        line_2               3   line_7               1
    q1        line_8               1   line_9               1
    q1        line_10              1   line_11              1
    q1        line_12              1   line_13              1
    q2        line_3               3   line_14              1
    q2        line_15              1   line_16              1
    q2        line_17              1   line_18              1
    q2        line_19              1   line_20              1
    q3        line_4               3   line_21              1
    q3        line_22              1   line_23              1
    q3        line_24              1   line_25              1
    q3        line_26              1   line_27              1
    q4        line_5               3   line_28              1
    q4        line_29              1   line_30              1
    q4        line_31              1   line_32              1
    q4        line_33              1   line_34              1
    q5        line_6               3   line_35              1
    q5        line_36              1   line_37              1
    q5        line_38              1   line_39              1
    q5        line_40              1   line_41              1
    p1_1      line_2               1   line_7               1
    p1_2      line_2               1   line_8               1
    p1_3      line_2               1   line_9               1
    p1_4      line_2               1   line_10              1
    p1_5      line_2               1   line_11              1
    p1_6      line_2               1   line_12              1
    p1_7      line_2               1   line_13              1
    p2_1      line_3               1   line_14              1
    p2_2      line_3               1   line_15              1
    p2_3      line_3               1   line_16              1
    p2_4      line_3               1   line_17              1
    p2_5      line_3               1   line_18              1
    p2_6      line_3               1   line_19              1
    p2_7      line_3               1   line_20              1
    p3_1      line_4               1   line_21              1
    p3_2      line_4               1   line_22              1
    p3_3      line_4               1   line_23              1
    p3_4      line_4               1   line_24              1
    p3_5      line_4               1   line_25              1
    p3_6      line_4               1   line_26              1
    p3_7      line_4               1   line_27              1
    p4_1      line_5               1   line_28              1
    p4_2      line_5               1   line_29              1
    p4_3      line_5               1   line_30              1
    p4_4      line_5               1   line_31              1
    p4_5      line_5               1   line_32              1
    p4_6      line_5               1   line_33              1
    p4_7      line_5               1   line_34              1
    p5_1      line_6               1   line_35              1
    p5_2      line_6               1   line_36              1
    p5_3      line_6               1   line_37              1
    p5_4      line_6               1   line_38              1
    p5_5      line_6               1   line_39              1
    p5_6      line_6               1   line_40              1
    p5_7      line_6               1   line_41              1
RHS
    RHS       line_2           16000   line_3           10000
    RHS       line_4           15000   line_5           20000
    RHS       line_6           13000
RANGES
BOUNDS
 LO BOUND     x1                 200
 UP BOUND     x1                 280
 LO BOUND     x2                  50
 UP BOUND     x2                  80
 LO BOUND     x3                 250
 UP BOUND     x3                 300
 LO BOUND     x4                 350
 UP BOUND     x4                 400
 LO BOUND     x5                  50
 UP BOUND     x5                 100
 LO BOUND     x6                  50
 UP BOUND     x6                  85
 LO BOUND     x7                  70
 UP BOUND     x7                 150
 PL BOUND     q1      
 PL BOUND     q2      
 PL BOUND     q3      
 PL BOUND     q4      
 PL BOUND     q5      
 PL BOUND     p1_1    
 PL BOUND     p1_2    
 PL BOUND     p1_3    
 PL BOUND     p1_4    
 PL BOUND     p1_5    
 PL BOUND     p1_6    
 PL BOUND     p1_7    
 PL BOUND     p2_1    
 PL BOUND     p2_2    
 PL BOUND     p2_3    
 PL BOUND     p2_4    
 PL BOUND     p2_5    
 PL BOUND     p2_6    
 PL BOUND     p2_7    
 PL BOUND     p3_1    
 PL BOUND     p3_2    
 PL BOUND     p3_3    
 PL BOUND     p3_4    
 PL BOUND     p3_5    
 PL BOUND     p3_6    
 PL BOUND     p3_7    
 PL BOUND     p4_1    
 PL BOUND     p4_2    
 PL BOUND     p4_3    
 PL BOUND     p4_4    
 PL BOUND     p4_5    
 PL BOUND     p4_6    
 PL BOUND     p4_7    
 PL BOUND     p5_1    
 PL BOUND     p5_2    
 PL BOUND     p5_3    
 PL BOUND     p5_4    
 PL BOUND     p5_5    
 PL BOUND     p5_6    
 PL BOUND     p5_7    
ENDATA
