#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
#include <adios2.h>

adios2::Params engineParams = {};

int main(int argc, char *argv[])
{
    std::string adiosEngine = "ssc";

    if(argc >= 2)
    {
        adiosEngine = argv[1];
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

    int wrRatio = (worldSize - readerSize) / readerSize;

    std::vector<float> myFloats;
    adios2::ADIOS adios(readerComm);
    adios2::IO io = adios.DeclareIO("TestIO");
    io.SetEngine(adiosEngine);
    io.SetParameters(engineParams);
    adios2::Engine engine = io.Open("Test", adios2::Mode::Read);

    adios2::Dims start({(size_t)readerRank*wrRatio, 0});
    adios2::Dims count({(size_t)wrRatio, 1000000});
    size_t datasize = std::accumulate(count.begin(), count.end(), 1, std::multiplies<size_t>());
    myFloats.resize(datasize);

    adios2::Variable<float> varFloats;
    if(adiosEngine == "ssc")
    {
        varFloats = io.InquireVariable<float>("varFloats");
        varFloats.SetSelection({start, count});
    }

    while(true)
    {
        auto ret = engine.BeginStep();
        if(ret == adios2::StepStatus::EndOfStream)
        {
            break;
        }
        varFloats = io.InquireVariable<float>("varFloats");
        varFloats.SetSelection({start, count});
        engine.Get(varFloats, myFloats.data());
        engine.LockReaderSelections();
        engine.EndStep();
    }

    engine.Close();

    MPI_Barrier(MPI_COMM_WORLD);

    for(int r=0; r<readerSize; ++r)
    {
        MPI_Barrier(readerComm);
        if(r == readerRank)
        {
            size_t s=0;
            for(auto i : myFloats){
                if(s < 16)
                {
                    std::cout << i << "   ";
                }
                ++s;
                if(s == count[1])
                {
                    std::cout << std::endl;
                    s=0;
                }
            }
            std::cout << std::endl;
        }
    }


    MPI_Finalize();
    if(readerRank == 0) std::cout << "Engine " << adiosEngine << " finished " << std::endl;
    return 0;
}
