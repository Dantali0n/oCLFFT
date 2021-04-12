void kernel window(global double *real, global double *imag, ulong size) {
    const size_t n = size;
    const size_t i = get_global_id(0);
    const size_t n_half = n >> 1;
    double samplesMinusOne = (convert_double(n) - 1.0);

    double ratio = ((double)get_global_id(0) / samplesMinusOne);
    double weighingFactor = 0.355768 - 
    	(0.487396 * (cos(TWO_PI * ratio))) +
    	(0.144232 * (cos(FOUR_PI * ratio))) -
    	(0.012604 * (cos(SIX_PI * ratio)));
    real[i] = real[i] * weighingFactor;
    real[n - (i + 1)] = real[n - (i + 1)] * weighingFactor;
}