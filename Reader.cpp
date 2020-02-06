#include <chrono>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>
#include <adios2.h>

std::string ioName = "test";
std::string fileName = "hello";
adios2::Params engineParams = {
    {"IPAddress", "127.0.0.1"},
    {"Verbose", "11"},
};

size_t steps = 10;

int readerRank, readerSize;
int worldRank, worldSize;

void runTest(const std::string &adiosEngine)
{
    std::cout << "Reader begin " << adiosEngine << std::endl;

    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int color=1;

    MPI_Comm readerComm;
    MPI_Comm_split(MPI_COMM_WORLD, color, worldRank, &readerComm);
    MPI_Comm_rank(readerComm, &readerRank);
    MPI_Comm_size(readerComm, &readerSize);

    std::vector<float> myFloats;
    adios2::ADIOS adios(readerComm);
    adios2::IO io = adios.DeclareIO(ioName);
    io.SetEngine(adiosEngine);
    io.SetParameters(engineParams);
    adios2::Engine engine = io.Open(fileName, adios2::Mode::Read);

    auto bpFloats = io.InquireVariable<float>("bpFloats");

    adios2::Dims start({(size_t)readerRank*10, 0});
    adios2::Dims count({10, 1000000});

    size_t datasize = std::accumulate(count.begin(), count.end(), 1, std::multiplies<size_t>());
    bpFloats.SetSelection({start, count});
    myFloats.resize(datasize);

    while(true)
    {
        auto ret = engine.BeginStep();
        if(ret == adios2::StepStatus::EndOfStream)
        {
            break;
        }

        engine.Get(bpFloats, myFloats.data());

        std::cout << "Engine " << adiosEngine << " Step " << engine.CurrentStep() << std::endl;
        engine.EndStep();

        size_t s=0;
        for(auto i : myFloats){
            if(s < 32)
            {
                std::cout << i << "   ";
            }
            ++s;
            if(s == bpFloats.Shape()[1])
            {
                std::cout << std::endl;
                s=0;
            }
        }
        std::cout << std::endl;
    }

    std::cout << "Final Step " << engine.CurrentStep() << std::endl;

    engine.Close();

}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    runTest("insitumpi");

    MPI_Finalize();
    return 0;
}
