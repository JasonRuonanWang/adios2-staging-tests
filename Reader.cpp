#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
#include <adios2.h>
#include "common.h"

int main(int argc, char *argv[])
{

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

    int writerSize = worldSize - readerSize;
    int wrRatio = writerSize / readerSize;

    adios2::ADIOS adios(readerComm);
    adios2::IO io = adios.DeclareIO("TestIO");
    io.SetEngine(adiosEngine);
    io.SetParameters(engineParams);
    adios2::Engine engine = io.Open("Test" + std::to_string(writerSize), adios2::Mode::Read);

    adios2::Dims shape({(size_t)writerSize, 1000000});
    adios2::Dims start({(size_t)readerRank*wrRatio, 0});
    adios2::Dims count({(size_t)wrRatio, 1000000});

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

    adios2::Variable<signed char> varChars;
    adios2::Variable<unsigned char> varUchars;
    adios2::Variable<signed short> varShorts;
    adios2::Variable<unsigned short> varUshorts;
    adios2::Variable<signed int> varInts;
    adios2::Variable<unsigned int> varUints;
    adios2::Variable<signed long> varLongs;
    adios2::Variable<unsigned long> varUlongs;
    adios2::Variable<float> varFloats;
    adios2::Variable<double> varDoubles;
    adios2::Variable<std::complex<float>> varCfloats;
    adios2::Variable<std::complex<double>> varCdoubles;

    if(adiosEngine == "ssc")
    {
        varChars = io.InquireVariable<signed char>("varChars");
        varUchars = io.InquireVariable<unsigned char>("varUchars");
        varShorts = io.InquireVariable<signed short>("varShorts");
        varUshorts = io.InquireVariable<unsigned short>("varUshorts");
        varInts = io.InquireVariable<signed int>("varInts");
        varUints = io.InquireVariable<unsigned int>("varUints");
        varLongs = io.InquireVariable<signed long>("varLongs");
        varUlongs = io.InquireVariable<unsigned long>("varUlongs");
        varFloats = io.InquireVariable<float>("varFloats");
        varDoubles = io.InquireVariable<double>("varDoubles");
        varCfloats = io.InquireVariable<std::complex<float>>("varCfloats");
        varCdoubles = io.InquireVariable<std::complex<double>>("varCdoubles");

        varChars.SetSelection({start, count});
        varUchars.SetSelection({start, count});
        varShorts.SetSelection({start, count});
        varUshorts.SetSelection({start, count});
        varInts.SetSelection({start, count});
        varUints.SetSelection({start, count});
        varLongs.SetSelection({start, count});
        varUlongs.SetSelection({start, count});
        varFloats.SetSelection({start, count});
        varDoubles.SetSelection({start, count});
        varCfloats.SetSelection({start, count});
        varCdoubles.SetSelection({start, count});
    }

    while(true)
    {
        auto ret = engine.BeginStep();
        if(ret == adios2::StepStatus::EndOfStream)
        {
            break;
        }
        if(engine.CurrentStep() == 0)
        {
            varChars = io.InquireVariable<signed char>("varChars");
            varUchars = io.InquireVariable<unsigned char>("varUchars");
            varShorts = io.InquireVariable<signed short>("varShorts");
            varUshorts = io.InquireVariable<unsigned short>("varUshorts");
            varInts = io.InquireVariable<signed int>("varInts");
            varUints = io.InquireVariable<unsigned int>("varUints");
            varLongs = io.InquireVariable<signed long>("varLongs");
            varUlongs = io.InquireVariable<unsigned long>("varUlongs");
            varFloats = io.InquireVariable<float>("varFloats");
            varDoubles = io.InquireVariable<double>("varDoubles");
            varCfloats = io.InquireVariable<std::complex<float>>("varCfloats");
            varCdoubles = io.InquireVariable<std::complex<double>>("varCdoubles");

            varChars.SetSelection({start, count});
            varUchars.SetSelection({start, count});
            varShorts.SetSelection({start, count});
            varUshorts.SetSelection({start, count});
            varInts.SetSelection({start, count});
            varUints.SetSelection({start, count});
            varLongs.SetSelection({start, count});
            varUlongs.SetSelection({start, count});
            varFloats.SetSelection({start, count});
            varDoubles.SetSelection({start, count});
            varCfloats.SetSelection({start, count});
            varCdoubles.SetSelection({start, count});
        }

        engine.Get(varChars, vecChars.data());
        engine.Get(varUchars, vecUchars.data());
        engine.Get(varShorts, vecShorts.data());
        engine.Get(varUshorts, vecUshorts.data());
        engine.Get(varInts, vecInts.data());
        engine.Get(varUints, vecUints.data());
        engine.Get(varLongs, vecLongs.data());
        engine.Get(varUlongs, vecUlongs.data());
        engine.Get(varFloats, vecFloats.data());
        engine.Get(varDoubles, vecDoubles.data());
        engine.Get(varCfloats, vecCfloats.data());
        engine.Get(varCdoubles, vecCdoubles.data());

        if(engine.CurrentStep() == 0)
        {
            engine.LockReaderSelections();
        }

        engine.EndStep();
    }

    engine.Close();

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    VerifyData(vecChars, 0, start, count, shape, "varChars");
    VerifyData(vecUchars, 0, start, count, shape, "varUchars");
    VerifyData(vecShorts, 0, start, count, shape, "varShorts");
    VerifyData(vecUshorts, 0, start, count, shape, "varUshorts");
    VerifyData(vecInts, 0, start, count, shape, "varInts");
    VerifyData(vecUints, 0, start, count, shape, "varUints");
    VerifyData(vecLongs, 0, start, count, shape, "varLongs");
    VerifyData(vecUlongs, 0, start, count, shape, "varUlongs");
    VerifyData(vecFloats, 0, start, count, shape, "varFloats");
    VerifyData(vecDoubles, 0, start, count, shape, "varDoubles");
    VerifyData(vecCfloats, 0, start, count, shape, "varCfloats");
    VerifyData(vecCdoubles, 0, start, count, shape, "varCdoubles");

    /*
    for(int r=0; r<readerSize; ++r)
    {
        MPI_Barrier(readerComm);
        if(r == readerRank)
        {
            size_t s=0;
            for(auto i : vecFloats){
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
    */

    MPI_Finalize();
    if(readerRank == 0) std::cout << "Engine " << adiosEngine << " finished " << std::endl;
    return 0;
}
