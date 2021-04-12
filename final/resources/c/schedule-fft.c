for(size_t i = 0; i < power; i++) {
	kernel.setArg(2, cl_size_t, &i);
	kernel.setArg(3, cl_size_t, &L[i]);
	kernel.setArg(4, cl_size_t, &L[i + 1]);
	kernel.setArg(5, cl_size_t, &C1[i]);
	kernel.setArg(6, cl_size_t, &C2[i]);
	cl_queue.enqueueNDRangeKernel(
		kernel, cl::NullRange, 
		cl::NDRange(size/L[i + 1], L[i]),
		cl::NullRange
	);
}
cl_queue.finish();