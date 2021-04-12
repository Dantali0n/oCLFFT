double c1 = -1.0; double c2 = 0.0; uint32_t l2 = 1;
for (uint8_t l = 0; (l < power); l++) {
	double u1 = 1.0; double u2 = 0.0; uint32_t l1 = l2;
	l2 <<= 1;
	for (j = 0; j < l1; j++) {
		for (uint32_t i = j; i < samples; i += l2) {
			...
		}
		double z = ((u1 * c1) - (u2 * c2));
		u2 = ((u1 * c2) + (u2 * c1));
		u1 = z;
	}
	c2 = -sqrt((1.0 - c1) / 2.0);
	c1 = sqrt((1.0 + c1) / 2.0);
}