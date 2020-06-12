all:
	mpic++ -DADIOS2_USE_MPI -std=gnu++11 /usr/local/lib/libadios2_cxx11_mpi.2.6.0.dylib /usr/local/lib/libadios2_cxx11.2.6.0.dylib Writer.cpp -o writer
	mpic++ -DADIOS2_USE_MPI -std=gnu++11 /usr/local/lib/libadios2_cxx11_mpi.2.6.0.dylib /usr/local/lib/libadios2_cxx11.2.6.0.dylib Reader.cpp -o reader

d:
	rm -rf *.sst job_* core.* *.erf *.job
