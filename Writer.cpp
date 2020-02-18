#include <adios2.h>
#include <iostream>
#include <mpi.h>
#include <numeric>
#include <thread>
#include <vector>
#include <chrono>
#include <map>

adios2::Params engineParams = {{"QueueLimit","1"}, {"QueueFullPolicy","Block"}};

double walltime = 1;

int main(int argc, char *argv[])
{

    std::string adiosEngine = "ssc";

    if(argc >= 2)
    {
        adiosEngine = argv[1];
    }

    if(argc >= 3)
    {
        walltime = 60 * std::stoi(argv[2]);
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

    engine.LockWriterDefinitions();

    MPI_Barrier(writerComm);
    auto timerStart = std::chrono::system_clock::now();
    auto timerNow = std::chrono::system_clock::now();
    std::chrono::duration<double> duration;

    size_t step = 0;
    while (duration.count() < walltime)
    {
        if(writerRank == 0)
        {
            std::cout << "Engine " << adiosEngine << " Step " << step << " Duration " << duration.count() << std::endl;
        }
        engine.BeginStep();
        engine.Put(bpFloats, myFloats.data());
        engine.EndStep();
        timerNow = std::chrono::system_clock::now();
        duration = timerNow - timerStart;
        ++ step;
    }

    size_t totalDatasize = 4000000 * step * writerSize;

    MPI_Barrier(writerComm);
    if(writerRank == 0)
    {
        std::cout << "===============================================================" << std::endl;
        std::cout << adiosEngine << " time " << duration.count() << " seconds, " << step << " steps, data rate " <<  totalDatasize / duration.count() / 1000000000 << " GB/s" << std::endl;
        std::cout << "===============================================================" << std::endl;
    }

    engine.Close();

    MPI_Finalize();

    return 0;
}
