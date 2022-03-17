#include <adios2.h>
#include <iostream>
#include <mpi.h>
#include <numeric>
#include <thread>
#include <vector>
#include <chrono>
#include <map>
#include "common.h"


int main(int argc, char *argv[])
{

    if(argc >= 2)
    {
        adiosEngine = argv[1];
    }

    int color=0;
    MPI_Init(&argc, &argv);
    int writerRank, writerSize;
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm writerComm;
    MPI_Comm_split(MPI_COMM_WORLD, color, worldRank, &writerComm);
    MPI_Comm_rank(writerComm, &writerRank);
    MPI_Comm_size(writerComm, &writerSize);

    adios2::Dims shape({(size_t)writerSize, 1000});
    adios2::Dims start({(size_t)writerRank, 0});
    adios2::Dims count({1, 1000});

    auto timerStart = std::chrono::system_clock::now();
    auto timerNow = std::chrono::system_clock::now();
    std::chrono::duration<double> duration;

    adios2::ADIOS adios(writerComm);
    adios2::IO io = adios.DeclareIO("TestIO");
    io.SetEngine(adiosEngine);
    io.SetParameters(engineParams);

    adios2::Engine engine = io.Open("Test" + std::to_string(writerSize), adios2::Mode::Write);
    auto varFloats = io.DefineVariable<float>("varFloats", shape, start, count);

//    engine.LockWriterDefinitions();

    size_t datasize = std::accumulate(count.begin(), count.end(), 1, std::multiplies<size_t>());
    std::vector<float> vecFloats(datasize);

    GenData(vecFloats, 0, start, count, shape);

    size_t step;
    for(step = 0; step < 20; ++step)
    {
        MPI_Barrier(writerComm);
        timerNow = std::chrono::system_clock::now();
        duration = timerNow - timerStart;
        engine.BeginStep();
        engine.Put(varFloats, vecFloats.data());
        engine.EndStep();
//        std::this_thread::sleep_for(std::chrono::seconds(sleepSeconds));
    }

    engine.Close();

    size_t stepDatasize = sizeof(float) * shape[1];
    size_t totalDatasize = step * writerSize * stepDatasize;

    timerNow = std::chrono::system_clock::now();
    duration = timerNow - timerStart;

    MPI_Barrier(MPI_COMM_WORLD);

    if(writerRank == 0)
    {
        std::cout << "===============================================================" << std::endl;
        std::cout << adiosEngine << " " << writerSize <<  " Writers, " << " time " << duration.count() << " seconds, " << step << " steps, " << "total data size " << totalDatasize / 1000000000 << " GB, data rate " <<  totalDatasize / duration.count() / 1000000000 << " GB/s" << std::endl;
        std::cout << "===============================================================" << std::endl;
    }


    MPI_Finalize();

    return 0;
}
