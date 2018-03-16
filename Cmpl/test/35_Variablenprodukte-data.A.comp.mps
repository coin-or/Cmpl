* CMPL - MPS - Export
NAME          35_Variablenprodukte-data.A.cmpl
ROWS
 N  p_bin   
 L  p_bin[1]
 L  p_bin[2]
 G  p_bin[3]
COLUMNS
    by        p_bin[2]            -1   p_bin[3]            -1
    bx_by     p_bin                1   p_bin[1]             1
    bx_by     p_bin[2]             1   p_bin[3]             1
    GVANF     'MARKER'                 'INTORG'
    bx        p_bin[1]           -25   p_bin[2]            -5
    bx        p_bin[3]           -25
    GVEND     'MARKER'                 'INTEND'
RHS
    RHS       p_bin[2]            -5   p_bin[3]           -25
RANGES
BOUNDS
 UP BOUND     bx                   1
 LO BOUND     by                   5
 UP BOUND     by                  25
 UP BOUND     bx_by               25
ENDATA