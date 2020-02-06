#include <adios2.h>
#include <iostream>
#include <mpi.h>
#include <numeric>
#include <thread>
#include <vector>
#include <chrono>
#include <map>

adios2::Params engineParams = {};

size_t steps = 100;

int main(int argc, char *argv[])
{

    std::string adiosEngine = "ssc";

    if(argc == 2)
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

    adios2::Dims shape({(size_t)writerSize, 1000000});
    adios2::Dims start({(size_t)writerRank, 0});
    adios2::Dims count({1, 1000000});

    size_t datasize = std::accumulate(count.begin(), count.end(), 1, std::multiplies<size_t>());
    std::vector<float> myFloats(datasize);
    for (size_t i = 0; i < count[1]; ++i)
    {
        for (size_t j = 0; j < count[0]; ++j)
        {
            myFloats[i*count[0] +j] = writerRank*10000 + i*count[0] +j;
        }
    }

    adios2::ADIOS adios(writerComm, adios2::DebugON);
    adios2::IO io = adios.DeclareIO("TestIO");
    io.SetEngine(adiosEngine);
    io.SetParameters(engineParams);

    adios2::Engine engine = io.Open("Test", adios2::Mode::Write);
    auto bpFloats = io.DefineVariable<float>("bpFloats", shape, start, count);

    MPI_Barrier(writerComm);
    auto timerStart = std::chrono::system_clock::now();
    for (int i = 0; i < steps; ++i)
    {
        engine.BeginStep();
        engine.Put(bpFloats, myFloats.data());
        engine.EndStep();
    }
    MPI_Barrier(writerComm);
    auto timerEnd = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = timerEnd - timerStart;
    if(worldRank == 0)
    {
        std::cout << adiosEngine << " time " << duration.count() << " seconds" << std::endl;
    }

    engine.Close();

    MPI_Finalize();

    return 0;
}
