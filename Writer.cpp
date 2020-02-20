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
    std::vector<unsigned long> vecUlongs(datasize);
    std::vector<float> vecFloats(datasize);
    std::vector<double> vecDoubles(datasize);
    std::vector<std::complex<float>> vecCfloats(datasize);
    std::vector<std::complex<double>> vecCdoubles(datasize);

    GenData(vecChars, 0, start, count, shape);
    GenData(vecUchars, 0, start, count, shape);
    GenData(vecShorts, 0, start, count, shape);
    GenData(vecUshorts, 0, start, count, shape);
    GenData(vecInts, 0, start, count, shape);
    GenData(vecUints, 0, start, count, shape);
    GenData(vecLongs, 0, start, count, shape);
    GenData(vecUlongs, 0, start, count, shape);
    GenData(vecFloats, 0, start, count, shape);
    GenData(vecDoubles, 0, start, count, shape);
    GenData(vecCfloats, 0, start, count, shape);
    GenData(vecCdoubles, 0, start, count, shape);

    auto timerStart = std::chrono::system_clock::now();
    auto timerNow = std::chrono::system_clock::now();
    std::chrono::duration<double> duration;

    adios2::ADIOS adios(writerComm, adios2::DebugON);
    adios2::IO io = adios.DeclareIO("TestIO");
    io.SetEngine(adiosEngine);
    io.SetParameters(engineParams);

    adios2::Engine engine = io.Open("Test", adios2::Mode::Write);
    auto varChars = io.DefineVariable<signed char>("varChars", shape, start, count);
    auto varUchars = io.DefineVariable<unsigned char>("varUchars", shape, start, count);
    auto varShorts = io.DefineVariable<signed short>("varShorts", shape, start, count);
    auto varUshorts = io.DefineVariable<unsigned short>("varUshorts", shape, start, count);
    auto varInts = io.DefineVariable<signed int>("varInts", shape, start, count);
    auto varUints = io.DefineVariable<unsigned int>("varUints", shape, start, count);
    auto varLongs = io.DefineVariable<signed long>("varLongs", shape, start, count);
    auto varUlongs = io.DefineVariable<unsigned long>("varUlongs", shape, start, count);
    auto varFloats = io.DefineVariable<float>("varFloats", shape, start, count);
    auto varDoubles = io.DefineVariable<double>("varDoubles", shape, start, count);
    auto varCfloats = io.DefineVariable<std::complex<float>>("varCfloats", shape, start, count);
    auto varCdoubles = io.DefineVariable<std::complex<double>>("varCdoubles", shape, start, count);

    engine.LockWriterDefinitions();

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
        engine.Put(varChars, vecChars.data());
        engine.Put(varUchars, vecUchars.data());
        engine.Put(varShorts, vecShorts.data());
        engine.Put(varUshorts, vecUshorts.data());
        engine.Put(varInts, vecInts.data());
        engine.Put(varUints, vecUints.data());
        engine.Put(varLongs, vecLongs.data());
        engine.Put(varUlongs, vecUlongs.data());
        engine.Put(varFloats, vecFloats.data());
        engine.Put(varDoubles, vecDoubles.data());
        engine.Put(varCfloats, vecCfloats.data());
        engine.Put(varCdoubles, vecCdoubles.data());
        engine.EndStep();
    }

    engine.Close();

    size_t stepDatasize = sizeof(signed char)
         +sizeof(unsigned char)
         +sizeof(signed short)
         +sizeof(unsigned short)
         +sizeof(signed int)
         +sizeof(unsigned int)
         +sizeof(signed long)
         +sizeof(unsigned long)
         +sizeof(float)
         +sizeof(double)
         +sizeof(std::complex<float>)
         +sizeof(std::complex<double>);

    size_t totalDatasize = step * writerSize * shape[1] * stepDatasize;
    timerNow = std::chrono::system_clock::now();
    duration = timerNow - timerStart;

    MPI_Barrier(MPI_COMM_WORLD);

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
