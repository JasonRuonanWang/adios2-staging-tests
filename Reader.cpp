#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
#include <mpi.h>
#include <adios2.h>
#include "common.h"

int main(int argc, char *argv[])
{

    if(argc >= 2)
    {
        adiosEngine = argv[1];
        if(adiosEngine == "rdma")
        {
            adiosEngine = "ssc";
            engineParams["DataTransport"] = "RDMA";
        }
    }

    int color=1;
    MPI_Init(&argc, &argv);
    int readerRank, readerSize;
    int worldRank, worldSize;
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm readerComm;
    MPI_Comm_split(MPI_COMM_WORLD, color, worldRank, &readerComm);
    MPI_Comm_rank(readerComm, &readerRank);
    MPI_Comm_size(readerComm, &readerSize);

    if(readerRank == 0) std::cout << "Engine " << adiosEngine << " started " << std::endl;

    int writerSize = worldSize - readerSize;
    int wrRatio = writerSize / readerSize;

    adios2::ADIOS adios(readerComm);
    adios2::IO io = adios.DeclareIO("TestIO");
    io.SetEngine(adiosEngine);
    io.SetParameters(engineParams);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    adios2::Engine engine = io.Open("Test" + std::to_string(writerSize), adios2::Mode::Read);

    adios2::Dims shape({(size_t)writerSize, 100000000});
    adios2::Dims start({(size_t)readerRank*wrRatio, 0});
    adios2::Dims count({(size_t)wrRatio, 100000000});

    size_t datasize = std::accumulate(count.begin(), count.end(), 1, std::multiplies<size_t>());

    std::vector<float> vecFloats(datasize);

    adios2::Variable<float> varFloats;

    while(true)
    {
        auto ret = engine.BeginStep();
        if(ret == adios2::StepStatus::EndOfStream)
        {
            break;
        }

        varFloats = io.InquireVariable<float>("varFloats");
        varFloats.SetSelection({start, count});

        engine.Get(varFloats, vecFloats.data());

        if(engine.CurrentStep() == 0)
        {
//            engine.LockReaderSelections();
        }

        engine.EndStep();

    }

    engine.Close();

    MPI_Barrier(MPI_COMM_WORLD);


    VerifyData(vecFloats, 0, start, count, shape, "varFloats");


    MPI_Finalize();
    if(readerRank == 0) std::cout << "Engine " << adiosEngine << " finished " << std::endl;
    return 0;
}
