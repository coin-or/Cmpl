* CMPL - MPS - Export
NAME          07_Mehrkriteriell.cmpl
ROWS
 N  Ziel[1] 
 G  Ziel[2] 
 L  Restr[3]
 L  Restr[4]
 L  Restr[5]
COLUMNS
    Wunschpk  Ziel[1]             11   Ziel[2]             11
    Wichtung  Ziel[1]              1   Ziel[2]              1
    GVANF     'MARKER'                 'INTORG'
    x[1]      Ziel[1]              1   Ziel[2]              2
    x[1]      Restr[3]             3   Restr[4]             1
    x[1]      Restr[5]             5
    x[2]      Ziel[1]              3   Ziel[2]              1
    x[2]      Restr[3]             1   Restr[4]             1
    x[2]      Restr[5]             8
    GVEND     'MARKER'                 'INTEND'
RHS
    RHS       Ziel[2]          1e-12   Restr[3]     16.500000
    RHS       Restr[4]      6.500000   Restr[5]            40
RANGES
BOUNDS
 UP BOUND     Wunschpk            15
 UP BOUND     Wichtung             0
 UP BOUND     x[1]                 7
 UP BOUND     x[2]                 7
ENDATA