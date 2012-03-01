* CMPL - Free-MPS - Export
NAME          11_Beispiel_Roemer2.cmpl
* OBJNAME Ziel
* OBJSENSE max
ROWS
 N Ziel
 L Ress_Drehen
 L Ress_Bohren
 L Ress_Fraesen
 L Ress_Montage
 L Ress_Material
 G Diff_Drehen_A
 G Diff_Drehen_B
 G Diff_Drehen_C
 G Diff_Drehen_D
 G Diff_Drehen_E
 G Diff_Drehen_F
 G Diff_Drehen_G
 G Diff_Bohren_A
 G Diff_Bohren_B
 G Diff_Bohren_C
 G Diff_Bohren_D
 G Diff_Bohren_E
 G Diff_Bohren_F
 G Diff_Bohren_G
 G Diff_Fraesen_A
 G Diff_Fraesen_B
 G Diff_Fraesen_C
 G Diff_Fraesen_D
 G Diff_Fraesen_E
 G Diff_Fraesen_F
 G Diff_Fraesen_G
 G Diff_Montage_A
 G Diff_Montage_B
 G Diff_Montage_C
 G Diff_Montage_D
 G Diff_Montage_E
 G Diff_Montage_F
 G Diff_Montage_G
 G Diff_Material_A
 G Diff_Material_B
 G Diff_Material_C
 G Diff_Material_D
 G Diff_Material_E
 G Diff_Material_F
 G Diff_Material_G
COLUMNS
 x[A] Ziel 50 Ress_Drehen 10
 x[A] Ress_Bohren 5 Ress_Fraesen 20
 x[A] Ress_Montage 10 Diff_Drehen_A -1.000000
 x[A] Diff_Bohren_A -0.500000 Diff_Fraesen_A -2.000000
 x[A] Diff_Montage_A -1.000000
 x[B] Ziel 80 Ress_Bohren 8
 x[B] Ress_Fraesen 15 Ress_Montage 16
 x[B] Diff_Bohren_B -0.800000 Diff_Fraesen_B -1.500000
 x[B] Diff_Montage_B -1.600000
 x[C] Ziel 110 Ress_Drehen 20
 x[C] Ress_Fraesen 18 Ress_Montage 17
 x[C] Ress_Material 13 Diff_Drehen_C -2.000000
 x[C] Diff_Fraesen_C -1.800000 Diff_Montage_C -1.700000
 x[C] Diff_Material_C -1.300000
 x[D] Ziel 75 Ress_Drehen 10
 x[D] Ress_Bohren 12 Ress_Montage 18
 x[D] Ress_Material 17 Diff_Drehen_D -1.000000
 x[D] Diff_Bohren_D -1.200000 Diff_Montage_D -1.800000
 x[D] Diff_Material_D -1.700000
 x[E] Ziel 100 Ress_Drehen 18
 x[E] Ress_Bohren 14 Ress_Fraesen 26
 x[E] Ress_Material 18 Diff_Drehen_E -1.800000
 x[E] Diff_Bohren_E -1.400000 Diff_Fraesen_E -2.600000
 x[E] Diff_Material_E -1.800000
 x[F] Ziel 60 Ress_Drehen 25
 x[F] Ress_Fraesen 17 Ress_Montage 20
 x[F] Diff_Drehen_F -2.500000 Diff_Fraesen_F -1.700000
 x[F] Diff_Montage_F -2.000000
 x[G] Ziel 85 Ress_Bohren 19
 x[G] Ress_Fraesen 18 Ress_Montage 21
 x[G] Ress_Material 14 Diff_Bohren_G -1.900000
 x[G] Diff_Fraesen_G -1.800000 Diff_Montage_G -2.100000
 x[G] Diff_Material_G -1.400000
 q[Drehen] Ress_Drehen 3 Diff_Drehen_A 1
 q[Drehen] Diff_Drehen_B 1 Diff_Drehen_C 1
 q[Drehen] Diff_Drehen_D 1 Diff_Drehen_E 1
 q[Drehen] Diff_Drehen_F 1 Diff_Drehen_G 1
 q[Bohren] Ress_Bohren 3 Diff_Bohren_A 1
 q[Bohren] Diff_Bohren_B 1 Diff_Bohren_C 1
 q[Bohren] Diff_Bohren_D 1 Diff_Bohren_E 1
 q[Bohren] Diff_Bohren_F 1 Diff_Bohren_G 1
 q[Fraesen] Ress_Fraesen 3 Diff_Fraesen_A 1
 q[Fraesen] Diff_Fraesen_B 1 Diff_Fraesen_C 1
 q[Fraesen] Diff_Fraesen_D 1 Diff_Fraesen_E 1
 q[Fraesen] Diff_Fraesen_F 1 Diff_Fraesen_G 1
 q[Montage] Ress_Montage 3 Diff_Montage_A 1
 q[Montage] Diff_Montage_B 1 Diff_Montage_C 1
 q[Montage] Diff_Montage_D 1 Diff_Montage_E 1
 q[Montage] Diff_Montage_F 1 Diff_Montage_G 1
 q[Material] Ress_Material 3 Diff_Material_A 1
 q[Material] Diff_Material_B 1 Diff_Material_C 1
 q[Material] Diff_Material_D 1 Diff_Material_E 1
 q[Material] Diff_Material_F 1 Diff_Material_G 1
 p[Drehen,A] Ress_Drehen 1 Diff_Drehen_A 1
 p[Drehen,B] Ress_Drehen 1 Diff_Drehen_B 1
 p[Drehen,C] Ress_Drehen 1 Diff_Drehen_C 1
 p[Drehen,D] Ress_Drehen 1 Diff_Drehen_D 1
 p[Drehen,E] Ress_Drehen 1 Diff_Drehen_E 1
 p[Drehen,F] Ress_Drehen 1 Diff_Drehen_F 1
 p[Drehen,G] Ress_Drehen 1 Diff_Drehen_G 1
 p[Bohren,A] Ress_Bohren 1 Diff_Bohren_A 1
 p[Bohren,B] Ress_Bohren 1 Diff_Bohren_B 1
 p[Bohren,C] Ress_Bohren 1 Diff_Bohren_C 1
 p[Bohren,D] Ress_Bohren 1 Diff_Bohren_D 1
 p[Bohren,E] Ress_Bohren 1 Diff_Bohren_E 1
 p[Bohren,F] Ress_Bohren 1 Diff_Bohren_F 1
 p[Bohren,G] Ress_Bohren 1 Diff_Bohren_G 1
 p[Fraesen,A] Ress_Fraesen 1 Diff_Fraesen_A 1
 p[Fraesen,B] Ress_Fraesen 1 Diff_Fraesen_B 1
 p[Fraesen,C] Ress_Fraesen 1 Diff_Fraesen_C 1
 p[Fraesen,D] Ress_Fraesen 1 Diff_Fraesen_D 1
 p[Fraesen,E] Ress_Fraesen 1 Diff_Fraesen_E 1
 p[Fraesen,F] Ress_Fraesen 1 Diff_Fraesen_F 1
 p[Fraesen,G] Ress_Fraesen 1 Diff_Fraesen_G 1
 p[Montage,A] Ress_Montage 1 Diff_Montage_A 1
 p[Montage,B] Ress_Montage 1 Diff_Montage_B 1
 p[Montage,C] Ress_Montage 1 Diff_Montage_C 1
 p[Montage,D] Ress_Montage 1 Diff_Montage_D 1
 p[Montage,E] Ress_Montage 1 Diff_Montage_E 1
 p[Montage,F] Ress_Montage 1 Diff_Montage_F 1
 p[Montage,G] Ress_Montage 1 Diff_Montage_G 1
 p[Material,A] Ress_Material 1 Diff_Material_A 1
 p[Material,B] Ress_Material 1 Diff_Material_B 1
 p[Material,C] Ress_Material 1 Diff_Material_C 1
 p[Material,D] Ress_Material 1 Diff_Material_D 1
 p[Material,E] Ress_Material 1 Diff_Material_E 1
 p[Material,F] Ress_Material 1 Diff_Material_F 1
 p[Material,G] Ress_Material 1 Diff_Material_G 1
RHS
 RHS Ress_Drehen 16000 Ress_Bohren 10000
 RHS Ress_Fraesen 15000 Ress_Montage 20000
 RHS Ress_Material 13000
RANGES
BOUNDS
 LO BOUND x[A] 200
 UP BOUND x[A] 280
 LO BOUND x[B] 50
 UP BOUND x[B] 80
 LO BOUND x[C] 250
 UP BOUND x[C] 300
 LO BOUND x[D] 350
 UP BOUND x[D] 400
 LO BOUND x[E] 50
 UP BOUND x[E] 100
 LO BOUND x[F] 50
 UP BOUND x[F] 85
 LO BOUND x[G] 70
 UP BOUND x[G] 150
 PL BOUND q[Drehen]
 PL BOUND q[Bohren]
 PL BOUND q[Fraesen]
 PL BOUND q[Montage]
 PL BOUND q[Material]
 PL BOUND p[Drehen,A]
 PL BOUND p[Drehen,B]
 PL BOUND p[Drehen,C]
 PL BOUND p[Drehen,D]
 PL BOUND p[Drehen,E]
 PL BOUND p[Drehen,F]
 PL BOUND p[Drehen,G]
 PL BOUND p[Bohren,A]
 PL BOUND p[Bohren,B]
 PL BOUND p[Bohren,C]
 PL BOUND p[Bohren,D]
 PL BOUND p[Bohren,E]
 PL BOUND p[Bohren,F]
 PL BOUND p[Bohren,G]
 PL BOUND p[Fraesen,A]
 PL BOUND p[Fraesen,B]
 PL BOUND p[Fraesen,C]
 PL BOUND p[Fraesen,D]
 PL BOUND p[Fraesen,E]
 PL BOUND p[Fraesen,F]
 PL BOUND p[Fraesen,G]
 PL BOUND p[Montage,A]
 PL BOUND p[Montage,B]
 PL BOUND p[Montage,C]
 PL BOUND p[Montage,D]
 PL BOUND p[Montage,E]
 PL BOUND p[Montage,F]
 PL BOUND p[Montage,G]
 PL BOUND p[Material,A]
 PL BOUND p[Material,B]
 PL BOUND p[Material,C]
 PL BOUND p[Material,D]
 PL BOUND p[Material,E]
 PL BOUND p[Material,F]
 PL BOUND p[Material,G]
ENDATA
