* CMPL - Free-MPS - Export
NAME          26_transportation.cmpl
ROWS
 N costs
 L supplies[1]
 L supplies[2]
 L supplies[3]
 E demands[1]
 E demands[2]
 E demands[3]
 E demands[4]
COLUMNS
 x[1,1] costs 3 supplies[1] 1
 x[1,1] demands[1] 1
 x[1,2] costs 2 supplies[1] 1
 x[1,2] demands[2] 1
 x[1,3] costs 7 supplies[1] 1
 x[1,3] demands[3] 1
 x[1,4] costs 6 supplies[1] 1
 x[1,4] demands[4] 1
 x[2,1] costs 7 supplies[2] 1
 x[2,1] demands[1] 1
 x[2,2] costs 5 supplies[2] 1
 x[2,2] demands[2] 1
 x[2,3] costs 2 supplies[2] 1
 x[2,3] demands[3] 1
 x[2,4] costs 3 supplies[2] 1
 x[2,4] demands[4] 1
 x[3,1] costs 2 supplies[3] 1
 x[3,1] demands[1] 1
 x[3,2] costs 5 supplies[3] 1
 x[3,2] demands[2] 1
 x[3,3] costs 4 supplies[3] 1
 x[3,3] demands[3] 1
 x[3,4] costs 5 supplies[3] 1
 x[3,4] demands[4] 1
RHS
 RHS supplies[1] 5000 supplies[2] 6000
 RHS supplies[3] 2500 demands[1] 6000
 RHS demands[2] 4000 demands[3] 2000
 RHS demands[4] 1500
RANGES
BOUNDS
 PL BOUND x[1,1]
 PL BOUND x[1,2]
 PL BOUND x[1,3]
 PL BOUND x[1,4]
 PL BOUND x[2,1]
 PL BOUND x[2,2]
 PL BOUND x[2,3]
 PL BOUND x[2,4]
 PL BOUND x[3,1]
 PL BOUND x[3,2]
 PL BOUND x[3,3]
 PL BOUND x[3,4]
ENDATA
