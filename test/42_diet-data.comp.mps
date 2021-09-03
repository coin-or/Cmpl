* CMPL - MPS - Export
NAME          42_diet-data
ROWS
 N  cost    
 G  line_2  
 G  line_3  
 G  line_4  
 G  line_5  
COLUMNS
    GVANF     'MARKER'                 'INTORG'
    x[BEEF]   cost          3.190000   line_2              60
    x[BEEF]   line_3              20   line_4              10
    x[BEEF]   line_5              15
    x[CHK]    cost          2.590000   line_2               8
    x[CHK]    line_4              20   line_5              20
    x[FISH]   cost          2.290000   line_2               8
    x[FISH]   line_3              10   line_4              15
    x[FISH]   line_5              10
    x[HAM]    cost          2.890000   line_2              40
    x[HAM]    line_3              40   line_4              35
    x[HAM]    line_5              10
    x[MCH]    cost          1.890000   line_2              15
    x[MCH]    line_3              35   line_4              15
    x[MCH]    line_5              15
    x[MTL]    cost          1.990000   line_2              70
    x[MTL]    line_3              30   line_4              15
    x[MTL]    line_5              15
    x[SPG]    cost          1.990000   line_2              25
    x[SPG]    line_3              50   line_4              25
    x[SPG]    line_5              15
    x[TUR]    cost          2.490000   line_2              60
    x[TUR]    line_3              20   line_4              15
    x[TUR]    line_5              10
    GVEND     'MARKER'                 'INTEND'
RHS
    RHS       line_2             700   line_3             700
    RHS       line_4             700   line_5             700
BOUNDS
 LO BOUND     x[BEEF]              2
 UP BOUND     x[BEEF]             10
 LO BOUND     x[CHK]               2
 UP BOUND     x[CHK]              10
 LO BOUND     x[FISH]              2
 UP BOUND     x[FISH]             10
 LO BOUND     x[HAM]               2
 UP BOUND     x[HAM]              10
 LO BOUND     x[MCH]               2
 UP BOUND     x[MCH]              10
 LO BOUND     x[MTL]               2
 UP BOUND     x[MTL]              10
 LO BOUND     x[SPG]               2
 UP BOUND     x[SPG]              10
 LO BOUND     x[TUR]               2
 UP BOUND     x[TUR]              10
ENDATA
