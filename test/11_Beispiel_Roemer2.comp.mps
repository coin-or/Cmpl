* CMPL - Free-MPS - Export
NAME          11_Beispiel_Roemer2.mps
* OBJNAME Ziel
* OBJSENSE max
ROWS
 N  Ziel
 L  Ress_Drehen
 L  Ress_Bohren
 L  Ress_Fraesen
 L  Ress_Montage
 L  Ress_Material
 G  Diff_Drehen_A
 G  Diff_Drehen_B
 G  Diff_Drehen_C
 G  Diff_Drehen_D
 G  Diff_Drehen_E
 G  Diff_Drehen_F
 G  Diff_Drehen_G
 G  Diff_Bohren_A
 G  Diff_Bohren_B
 G  Diff_Bohren_C
 G  Diff_Bohren_D
 G  Diff_Bohren_E
 G  Diff_Bohren_F
 G  Diff_Bohren_G
 G  Diff_Fraesen_A
 G  Diff_Fraesen_B
 G  Diff_Fraesen_C
 G  Diff_Fraesen_D
 G  Diff_Fraesen_E
 G  Diff_Fraesen_F
 G  Diff_Fraesen_G
 G  Diff_Montage_A
 G  Diff_Montage_B
 G  Diff_Montage_C
 G  Diff_Montage_D
 G  Diff_Montage_E
 G  Diff_Montage_F
 G  Diff_Montage_G
 G  Diff_Material_A
 G  Diff_Material_B
 G  Diff_Material_C
 G  Diff_Material_D
 G  Diff_Material_E
 G  Diff_Material_F
 G  Diff_Material_G
COLUMNS
 x_A  Ziel  50 Ress_Drehen  10
 x_A  Ress_Bohren  5 Ress_Fraesen  20
 x_A  Ress_Montage  10 Diff_Drehen_A  -1.000000
 x_A  Diff_Bohren_A  -0.500000 Diff_Fraesen_A  -2.000000
 x_A  Diff_Montage_A  -1.000000
 x_B  Ziel  80 Ress_Bohren  8
 x_B  Ress_Fraesen  15 Ress_Montage  16
 x_B  Diff_Bohren_B  -0.800000 Diff_Fraesen_B  -1.500000
 x_B  Diff_Montage_B  -1.600000
 x_C  Ziel  110 Ress_Drehen  20
 x_C  Ress_Fraesen  18 Ress_Montage  17
 x_C  Ress_Material  13 Diff_Drehen_C  -2.000000
 x_C  Diff_Fraesen_C  -1.800000 Diff_Montage_C  -1.700000
 x_C  Diff_Material_C  -1.300000
 x_D  Ziel  75 Ress_Drehen  10
 x_D  Ress_Bohren  12 Ress_Montage  18
 x_D  Ress_Material  17 Diff_Drehen_D  -1.000000
 x_D  Diff_Bohren_D  -1.200000 Diff_Montage_D  -1.800000
 x_D  Diff_Material_D  -1.700000
 x_E  Ziel  100 Ress_Drehen  18
 x_E  Ress_Bohren  14 Ress_Fraesen  26
 x_E  Ress_Material  18 Diff_Drehen_E  -1.800000
 x_E  Diff_Bohren_E  -1.400000 Diff_Fraesen_E  -2.600000
 x_E  Diff_Material_E  -1.800000
 x_F  Ziel  60 Ress_Drehen  25
 x_F  Ress_Fraesen  17 Ress_Montage  20
 x_F  Diff_Drehen_F  -2.500000 Diff_Fraesen_F  -1.700000
 x_F  Diff_Montage_F  -2.000000
 x_G  Ziel  85 Ress_Bohren  19
 x_G  Ress_Fraesen  18 Ress_Montage  21
 x_G  Ress_Material  14 Diff_Bohren_G  -1.900000
 x_G  Diff_Fraesen_G  -1.800000 Diff_Montage_G  -2.100000
 x_G  Diff_Material_G  -1.400000
 q_Drehen  Ress_Drehen  3 Diff_Drehen_A  1
 q_Drehen  Diff_Drehen_B  1 Diff_Drehen_C  1
 q_Drehen  Diff_Drehen_D  1 Diff_Drehen_E  1
 q_Drehen  Diff_Drehen_F  1 Diff_Drehen_G  1
 q_Bohren  Ress_Bohren  3 Diff_Bohren_A  1
 q_Bohren  Diff_Bohren_B  1 Diff_Bohren_C  1
 q_Bohren  Diff_Bohren_D  1 Diff_Bohren_E  1
 q_Bohren  Diff_Bohren_F  1 Diff_Bohren_G  1
 q_Fraesen  Ress_Fraesen  3 Diff_Fraesen_A  1
 q_Fraesen  Diff_Fraesen_B  1 Diff_Fraesen_C  1
 q_Fraesen  Diff_Fraesen_D  1 Diff_Fraesen_E  1
 q_Fraesen  Diff_Fraesen_F  1 Diff_Fraesen_G  1
 q_Montage  Ress_Montage  3 Diff_Montage_A  1
 q_Montage  Diff_Montage_B  1 Diff_Montage_C  1
 q_Montage  Diff_Montage_D  1 Diff_Montage_E  1
 q_Montage  Diff_Montage_F  1 Diff_Montage_G  1
 q_Material  Ress_Material  3 Diff_Material_A  1
 q_Material  Diff_Material_B  1 Diff_Material_C  1
 q_Material  Diff_Material_D  1 Diff_Material_E  1
 q_Material  Diff_Material_F  1 Diff_Material_G  1
 p_Drehen_A  Ress_Drehen  1 Diff_Drehen_A  1
 p_Drehen_B  Ress_Drehen  1 Diff_Drehen_B  1
 p_Drehen_C  Ress_Drehen  1 Diff_Drehen_C  1
 p_Drehen_D  Ress_Drehen  1 Diff_Drehen_D  1
 p_Drehen_E  Ress_Drehen  1 Diff_Drehen_E  1
 p_Drehen_F  Ress_Drehen  1 Diff_Drehen_F  1
 p_Drehen_G  Ress_Drehen  1 Diff_Drehen_G  1
 p_Bohren_A  Ress_Bohren  1 Diff_Bohren_A  1
 p_Bohren_B  Ress_Bohren  1 Diff_Bohren_B  1
 p_Bohren_C  Ress_Bohren  1 Diff_Bohren_C  1
 p_Bohren_D  Ress_Bohren  1 Diff_Bohren_D  1
 p_Bohren_E  Ress_Bohren  1 Diff_Bohren_E  1
 p_Bohren_F  Ress_Bohren  1 Diff_Bohren_F  1
 p_Bohren_G  Ress_Bohren  1 Diff_Bohren_G  1
 p_Fraesen_A  Ress_Fraesen  1 Diff_Fraesen_A  1
 p_Fraesen_B  Ress_Fraesen  1 Diff_Fraesen_B  1
 p_Fraesen_C  Ress_Fraesen  1 Diff_Fraesen_C  1
 p_Fraesen_D  Ress_Fraesen  1 Diff_Fraesen_D  1
 p_Fraesen_E  Ress_Fraesen  1 Diff_Fraesen_E  1
 p_Fraesen_F  Ress_Fraesen  1 Diff_Fraesen_F  1
 p_Fraesen_G  Ress_Fraesen  1 Diff_Fraesen_G  1
 p_Montage_A  Ress_Montage  1 Diff_Montage_A  1
 p_Montage_B  Ress_Montage  1 Diff_Montage_B  1
 p_Montage_C  Ress_Montage  1 Diff_Montage_C  1
 p_Montage_D  Ress_Montage  1 Diff_Montage_D  1
 p_Montage_E  Ress_Montage  1 Diff_Montage_E  1
 p_Montage_F  Ress_Montage  1 Diff_Montage_F  1
 p_Montage_G  Ress_Montage  1 Diff_Montage_G  1
 p_Material_A  Ress_Material  1 Diff_Material_A  1
 p_Material_B  Ress_Material  1 Diff_Material_B  1
 p_Material_C  Ress_Material  1 Diff_Material_C  1
 p_Material_D  Ress_Material  1 Diff_Material_D  1
 p_Material_E  Ress_Material  1 Diff_Material_E  1
 p_Material_F  Ress_Material  1 Diff_Material_F  1
 p_Material_G  Ress_Material  1 Diff_Material_G  1
RHS
 RHS  Ress_Drehen  16000 Ress_Bohren 10000
 RHS  Ress_Fraesen  15000 Ress_Montage 20000
 RHS  Ress_Material  13000
RANGES
BOUNDS
 LO BOUND  x_A 200
 UP BOUND x_A 280
 LO BOUND  x_B 50
 UP BOUND x_B 80
 LO BOUND  x_C 250
 UP BOUND x_C 300
 LO BOUND  x_D 350
 UP BOUND x_D 400
 LO BOUND  x_E 50
 UP BOUND x_E 100
 LO BOUND  x_F 50
 UP BOUND x_F 85
 LO BOUND  x_G 70
 UP BOUND x_G 150
 PL BOUND q_Drehen
 PL BOUND q_Bohren
 PL BOUND q_Fraesen
 PL BOUND q_Montage
 PL BOUND q_Material
 PL BOUND p_Drehen_A
 PL BOUND p_Drehen_B
 PL BOUND p_Drehen_C
 PL BOUND p_Drehen_D
 PL BOUND p_Drehen_E
 PL BOUND p_Drehen_F
 PL BOUND p_Drehen_G
 PL BOUND p_Bohren_A
 PL BOUND p_Bohren_B
 PL BOUND p_Bohren_C
 PL BOUND p_Bohren_D
 PL BOUND p_Bohren_E
 PL BOUND p_Bohren_F
 PL BOUND p_Bohren_G
 PL BOUND p_Fraesen_A
 PL BOUND p_Fraesen_B
 PL BOUND p_Fraesen_C
 PL BOUND p_Fraesen_D
 PL BOUND p_Fraesen_E
 PL BOUND p_Fraesen_F
 PL BOUND p_Fraesen_G
 PL BOUND p_Montage_A
 PL BOUND p_Montage_B
 PL BOUND p_Montage_C
 PL BOUND p_Montage_D
 PL BOUND p_Montage_E
 PL BOUND p_Montage_F
 PL BOUND p_Montage_G
 PL BOUND p_Material_A
 PL BOUND p_Material_B
 PL BOUND p_Material_C
 PL BOUND p_Material_D
 PL BOUND p_Material_E
 PL BOUND p_Material_F
 PL BOUND p_Material_G
ENDATA
