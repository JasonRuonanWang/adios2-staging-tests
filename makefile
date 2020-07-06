all:
	mpic++ $(shell adios2-config --cxx) Reader.cpp -o reader
	mpic++ $(shell adios2-config --cxx) Writer.cpp -o writer

d:
	rm -rf *.sst job_* core.* *.erf *.job
