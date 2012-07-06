* CMPL - MPS - Export
NAME          37_Mehrkriteriell-data.cmpl
ROWS
 N  Ziel1   
 N  Ziel2   
 L  Restr3  
 L  Restr4  
 L  Restr5  
COLUMNS
    Wunschpk  Ziel1               11   Ziel2               11
    Wichtung  Ziel1                1   Ziel2                1
    GVANF     'MARKER'                 'INTORG'
    x[1]      Ziel1                1   Ziel2                2
    x[1]      Restr3               3   Restr4               1
    x[1]      Restr5               5
    x[2]      Ziel1                3   Ziel2                1
    x[2]      Restr3               1   Restr4               1
    x[2]      Restr5               8
    GVEND     'MARKER'                 'INTEND'
RHS
    RHS       Restr3       16.500000   Restr4        6.500000
    RHS       Restr5              40
RANGES
BOUNDS
 UP BOUND     Wunschpk            15
 UP BOUND     Wichtung             0
 UP BOUND     x[1]                 7
 UP BOUND     x[2]                 7
ENDATA
