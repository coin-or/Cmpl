* CMPL - MPS - Export
NAME          02_Standardmodell
ROWS
 N  ziel    
 L  line[1] 
 L  line[2] 
COLUMNS
    x[2]      ziel                 2   line[1]       7.700000
    x[2]      line[2]       4.200000
    x[3]      ziel                 3   line[1]      10.500000
    x[3]      line[2]      11.100000
    GVANF     'MARKER'                 'INTORG'
    x[1]      ziel                 1   line[1]       5.600000
    x[1]      line[2]       9.800000
    GVEND     'MARKER'                 'INTEND'
RHS
    RHS       line[1]             15   line[2]             20
BOUNDS
 PL BOUND     x[1]    
 PL BOUND     x[2]    
 PL BOUND     x[3]    
ENDATA
