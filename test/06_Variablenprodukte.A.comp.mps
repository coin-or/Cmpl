* CMPL - MPS - Export
NAME          06_Variablenprodukte.A.cmpl
ROWS
 N  p_bin   
 L  p_bin1  
 L  p_bin2  
 G  p_bin3  
COLUMNS
    by        p_bin2              -1   p_bin3              -1
    bx_by     p_bin                1   p_bin1               1
    bx_by     p_bin2               1   p_bin3               1
    GVANF     'MARKER'                 'INTORG'
    bx        p_bin1             -25   p_bin2              -5
    bx        p_bin3             -25
    GVEND     'MARKER'                 'INTEND'
RHS
    RHS       p_bin2              -5   p_bin3             -25
RANGES
BOUNDS
 UP BOUND     bx                   1
 LO BOUND     by                   5
 UP BOUND     by                  25
 UP BOUND     bx_by               25
ENDATA
