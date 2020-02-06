all:
	mpic++ Writer.cpp -g -o writer -ladios2 -std=c++11
	mpic++ Reader.cpp -g -o reader -ladios2 -std=c++11

d:
	rm -rf *.sst job_* core.* *.erf *.job
