* CMPL - Free-MPS - Export
NAME          11_Beispiel_Roemer2.cmpl
ROWS
 N Ziel
 L Ress[Drehen]
 L Ress[Bohren]
 L Ress[Fraesen]
 L Ress[Montage]
 L Ress[Material]
 G Diff[Drehen,A]
 G Diff[Drehen,B]
 G Diff[Drehen,C]
 G Diff[Drehen,D]
 G Diff[Drehen,E]
 G Diff[Drehen,F]
 G Diff[Drehen,G]
 G Diff[Bohren,A]
 G Diff[Bohren,B]
 G Diff[Bohren,C]
 G Diff[Bohren,D]
 G Diff[Bohren,E]
 G Diff[Bohren,F]
 G Diff[Bohren,G]
 G Diff[Fraesen,A]
 G Diff[Fraesen,B]
 G Diff[Fraesen,C]
 G Diff[Fraesen,D]
 G Diff[Fraesen,E]
 G Diff[Fraesen,F]
 G Diff[Fraesen,G]
 G Diff[Montage,A]
 G Diff[Montage,B]
 G Diff[Montage,C]
 G Diff[Montage,D]
 G Diff[Montage,E]
 G Diff[Montage,F]
 G Diff[Montage,G]
 G Diff[Material,A]
 G Diff[Material,B]
 G Diff[Material,C]
 G Diff[Material,D]
 G Diff[Material,E]
 G Diff[Material,F]
 G Diff[Material,G]
COLUMNS
 x[A] Ziel 50 Ress[Drehen] 10
 x[A] Ress[Bohren] 5 Ress[Fraesen] 20
 x[A] Ress[Montage] 10 Diff[Drehen,A] -1.000000
 x[A] Diff[Bohren,A] -0.500000 Diff[Fraesen,A] -2.000000
 x[A] Diff[Montage,A] -1.000000
 x[B] Ziel 80 Ress[Bohren] 8
 x[B] Ress[Fraesen] 15 Ress[Montage] 16
 x[B] Diff[Bohren,B] -0.800000 Diff[Fraesen,B] -1.500000
 x[B] Diff[Montage,B] -1.600000
 x[C] Ziel 110 Ress[Drehen] 20
 x[C] Ress[Fraesen] 18 Ress[Montage] 17
 x[C] Ress[Material] 13 Diff[Drehen,C] -2.000000
 x[C] Diff[Fraesen,C] -1.800000 Diff[Montage,C] -1.700000
 x[C] Diff[Material,C] -1.300000
 x[D] Ziel 75 Ress[Drehen] 10
 x[D] Ress[Bohren] 12 Ress[Montage] 18
 x[D] Ress[Material] 17 Diff[Drehen,D] -1.000000
 x[D] Diff[Bohren,D] -1.200000 Diff[Montage,D] -1.800000
 x[D] Diff[Material,D] -1.700000
 x[E] Ziel 100 Ress[Drehen] 18
 x[E] Ress[Bohren] 14 Ress[Fraesen] 26
 x[E] Ress[Material] 18 Diff[Drehen,E] -1.800000
 x[E] Diff[Bohren,E] -1.400000 Diff[Fraesen,E] -2.600000
 x[E] Diff[Material,E] -1.800000
 x[F] Ziel 60 Ress[Drehen] 25
 x[F] Ress[Fraesen] 17 Ress[Montage] 20
 x[F] Diff[Drehen,F] -2.500000 Diff[Fraesen,F] -1.700000
 x[F] Diff[Montage,F] -2.000000
 x[G] Ziel 85 Ress[Bohren] 19
 x[G] Ress[Fraesen] 18 Ress[Montage] 21
 x[G] Ress[Material] 14 Diff[Bohren,G] -1.900000
 x[G] Diff[Fraesen,G] -1.800000 Diff[Montage,G] -2.100000
 x[G] Diff[Material,G] -1.400000
 q[Drehen] Ress[Drehen] 3 Diff[Drehen,A] 1
 q[Drehen] Diff[Drehen,B] 1 Diff[Drehen,C] 1
 q[Drehen] Diff[Drehen,D] 1 Diff[Drehen,E] 1
 q[Drehen] Diff[Drehen,F] 1 Diff[Drehen,G] 1
 q[Bohren] Ress[Bohren] 3 Diff[Bohren,A] 1
 q[Bohren] Diff[Bohren,B] 1 Diff[Bohren,C] 1
 q[Bohren] Diff[Bohren,D] 1 Diff[Bohren,E] 1
 q[Bohren] Diff[Bohren,F] 1 Diff[Bohren,G] 1
 q[Fraesen] Ress[Fraesen] 3 Diff[Fraesen,A] 1
 q[Fraesen] Diff[Fraesen,B] 1 Diff[Fraesen,C] 1
 q[Fraesen] Diff[Fraesen,D] 1 Diff[Fraesen,E] 1
 q[Fraesen] Diff[Fraesen,F] 1 Diff[Fraesen,G] 1
 q[Montage] Ress[Montage] 3 Diff[Montage,A] 1
 q[Montage] Diff[Montage,B] 1 Diff[Montage,C] 1
 q[Montage] Diff[Montage,D] 1 Diff[Montage,E] 1
 q[Montage] Diff[Montage,F] 1 Diff[Montage,G] 1
 q[Material] Ress[Material] 3 Diff[Material,A] 1
 q[Material] Diff[Material,B] 1 Diff[Material,C] 1
 q[Material] Diff[Material,D] 1 Diff[Material,E] 1
 q[Material] Diff[Material,F] 1 Diff[Material,G] 1
 p[Drehen,A] Ress[Drehen] 1 Diff[Drehen,A] 1
 p[Drehen,B] Ress[Drehen] 1 Diff[Drehen,B] 1
 p[Drehen,C] Ress[Drehen] 1 Diff[Drehen,C] 1
 p[Drehen,D] Ress[Drehen] 1 Diff[Drehen,D] 1
 p[Drehen,E] Ress[Drehen] 1 Diff[Drehen,E] 1
 p[Drehen,F] Ress[Drehen] 1 Diff[Drehen,F] 1
 p[Drehen,G] Ress[Drehen] 1 Diff[Drehen,G] 1
 p[Bohren,A] Ress[Bohren] 1 Diff[Bohren,A] 1
 p[Bohren,B] Ress[Bohren] 1 Diff[Bohren,B] 1
 p[Bohren,C] Ress[Bohren] 1 Diff[Bohren,C] 1
 p[Bohren,D] Ress[Bohren] 1 Diff[Bohren,D] 1
 p[Bohren,E] Ress[Bohren] 1 Diff[Bohren,E] 1
 p[Bohren,F] Ress[Bohren] 1 Diff[Bohren,F] 1
 p[Bohren,G] Ress[Bohren] 1 Diff[Bohren,G] 1
 p[Fraesen,A] Ress[Fraesen] 1 Diff[Fraesen,A] 1
 p[Fraesen,B] Ress[Fraesen] 1 Diff[Fraesen,B] 1
 p[Fraesen,C] Ress[Fraesen] 1 Diff[Fraesen,C] 1
 p[Fraesen,D] Ress[Fraesen] 1 Diff[Fraesen,D] 1
 p[Fraesen,E] Ress[Fraesen] 1 Diff[Fraesen,E] 1
 p[Fraesen,F] Ress[Fraesen] 1 Diff[Fraesen,F] 1
 p[Fraesen,G] Ress[Fraesen] 1 Diff[Fraesen,G] 1
 p[Montage,A] Ress[Montage] 1 Diff[Montage,A] 1
 p[Montage,B] Ress[Montage] 1 Diff[Montage,B] 1
 p[Montage,C] Ress[Montage] 1 Diff[Montage,C] 1
 p[Montage,D] Ress[Montage] 1 Diff[Montage,D] 1
 p[Montage,E] Ress[Montage] 1 Diff[Montage,E] 1
 p[Montage,F] Ress[Montage] 1 Diff[Montage,F] 1
 p[Montage,G] Ress[Montage] 1 Diff[Montage,G] 1
 p[Material,A] Ress[Material] 1 Diff[Material,A] 1
 p[Material,B] Ress[Material] 1 Diff[Material,B] 1
 p[Material,C] Ress[Material] 1 Diff[Material,C] 1
 p[Material,D] Ress[Material] 1 Diff[Material,D] 1
 p[Material,E] Ress[Material] 1 Diff[Material,E] 1
 p[Material,F] Ress[Material] 1 Diff[Material,F] 1
 p[Material,G] Ress[Material] 1 Diff[Material,G] 1
RHS
 RHS Ress[Drehen] 16000 Ress[Bohren] 10000
 RHS Ress[Fraesen] 15000 Ress[Montage] 20000
 RHS Ress[Material] 13000
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