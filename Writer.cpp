#include <adios2.h>
#include <iostream>
#include <mpi.h>
#include <numeric>
#include <thread>
#include <vector>
#include <chrono>
#include <map>
#include "common.h"



adios2::Params engineParams = {
    {"DataTransport","RDMA"},
    {"ControlTransport","Scalable"},
    {"QueueLimit","1"},
    {"QueueFullPolicy","Block"}};

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
    std::vector<signed char> vecChars(datasize);
    std::vector<unsigned char> vecUchars(datasize);
    std::vector<signed short> vecShorts(datasize);
    std::vector<unsigned short> vecUshorts(datasize);
    std::vector<signed int> vecInts(datasize);
    std::vector<unsigned int> vecUints(datasize);
    std::vector<signed long> vecLongs(datasize);
    std::vector<unsigned long> vecULongs(datasize);
    std::vector<float> vecFloats(datasize);
    std::vector<double> vecDoubles(datasize);
    std::vector<std::complex<float>> vecCfloats(datasize);
    std::vector<std::complex<double>> vecDfloats(datasize);

    GenData(vecFloats, 0, start, count, shape);

    auto timerStart = std::chrono::system_clock::now();
    auto timerNow = std::chrono::system_clock::now();
    std::chrono::duration<double> duration;

    adios2::ADIOS adios(writerComm, adios2::DebugON);
    adios2::IO io = adios.DeclareIO("TestIO");
    io.SetEngine(adiosEngine);
    io.SetParameters(engineParams);

    adios2::Engine engine = io.Open("Test", adios2::Mode::Write);
    auto varFloats = io.DefineVariable<float>("varFloats", shape, start, count);

    engine.LockWriterDefinitions();

    MPI_Barrier(writerComm);

    size_t step;
    for(step = 0; step < 2000; ++step)
    {
        MPI_Barrier(writerComm);
        timerNow = std::chrono::system_clock::now();
        duration = timerNow - timerStart;
        if(writerRank == 0)
        {
            std::cout << "Engine " << adiosEngine << " Step " << step << " Duration " << duration.count() << std::endl;
        }
        engine.BeginStep();
        engine.Put(varFloats, vecFloats.data());
        engine.EndStep();
    }

    engine.Close();

    size_t totalDatasize = 4000000 * step * writerSize;
    timerNow = std::chrono::system_clock::now();
    duration = timerNow - timerStart;

    if(writerRank == 0)
    {
        std::cout << "===============================================================" << std::endl;
        std::cout << adiosEngine << " time " << duration.count() << " seconds, " << step << " steps, " << "total data size " << totalDatasize / 1000000000 << " GB, data rate " <<  totalDatasize / duration.count() / 1000000000 << " GB/s" << std::endl;
        std::cout << "===============================================================" << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();

    return 0;
}
