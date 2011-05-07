* CMPL - MPS - Export
NAME          02_Standardmodell.mps
ROWS
 N  ziel    
 L  line_2  
 L  line_3  
COLUMNS
    x2        ziel                 2   line_2        7.700000
    x2        line_3        4.200000
    x3        ziel                 3   line_2       10.500000
    x3        line_3       11.100000
    GVANF     'MARKER'                 'INTORG'
    x1        ziel                 1   line_2        5.600000
    x1        line_3        9.800000
    GVEND     'MARKER'                 'INTEND'
RHS
    RHS       line_2              15   line_3              20
RANGES
BOUNDS
ENDATA
