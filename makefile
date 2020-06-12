all:
	mpic++ -DADIOS2_USE_MPI -std=gnu++11 -ladios2_cxx11_mpi -ladios2_cxx11 Writer.cpp -o writer
	mpic++ -DADIOS2_USE_MPI -std=gnu++11 -ladios2_cxx11_mpi -ladios2_cxx11 Reader.cpp -o reader

d:
	rm -rf *.sst job_* core.* *.erf *.job
