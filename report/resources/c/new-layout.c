ulong j = get_global_id(0);
double u1 = (U1[power])[j];

ulong i = (get_global_id(1) * l2) + j; 
double u2 = (U2[power])[j];