void kernel magnitude(global double *real, global double *imag) {
    ulong i = get_global_id(0);
    real[i] = sqrt((real[i] * real[i]) + (imag[i] * imag[i]));
}