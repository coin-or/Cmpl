* CMPL - MPS - Export
NAME          06_Variablenprodukte.C.cmpl
ROWS
 N  p_real  
 E  p_real1 
 E  p_real2 
 E  p_real3 
 L  p_real4 
 L  p_real5 
 G  p_real6 
COLUMNS
    rx        p_real1              1   p_real3             -5
    ry        p_real2              1
    rx_ry     p_real               1   p_real3              1
    _rx       p_real1            -10   p_real4             -1
    _rx       p_real6             -1
    _ry       p_real2            -20   p_real5             -1
    _ry       p_real6             -1
    _rx__ry   p_real3           -200   p_real4              1
    _rx__ry   p_real5              1   p_real6              1
RHS
    RHS       p_real2              5   p_real6             -1
RANGES
BOUNDS
 UP BOUND     rx                  10
 LO BOUND     ry                   5
 UP BOUND     ry                  25
 UP BOUND     rx_ry              250
 UP BOUND     _rx                  1
 UP BOUND     _ry                  1
 UP BOUND     _rx__ry              1
ENDATA
