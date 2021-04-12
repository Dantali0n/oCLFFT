void kernel fft(global double *real, global double *imag, ulong size, ulong power) {
	double c1 = -1.0;
	double c2 = 0.0;
	long l2 = 1;

	for (uint l = 0; l < power; l++) {
		uint l1 = l2;
		l2 <<= 1;
		double u1 = 1.0;
		double u2 = 0.0;

		for (uint j = 0; j < l1; j++) {
			for (uint i = j; i < size; i += l2) {
				uint i1 = i + l1;
				double t1 = u1 * real[i1] - u2 * imag[i1];
				double t2 = u1 * imag[i1] + u2 * real[i1];

				real[i1] = real[i] - t1;
				imag[i1] = imag[i] - t2;
				real[i] += t1;
				imag[i] += t2;
			}
			double z = ((u1 * c1) - (u2 * c2));
			u2 = ((u1 * c2) + (u2 * c1));
			u1 = z;
		}

		c2 = sqrt((1.0 - c1) / 2.0);
		c1 = sqrt((1.0 + c1) / 2.0);

		c2 = -c2;
	}
}