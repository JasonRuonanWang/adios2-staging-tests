#include <adios2.h>
#include <iostream>
#include <mpi.h>
#include <numeric>
#include <thread>
#include <vector>
#include <chrono>
#include <map>

std::string ioName = "test";
std::string fileName = "hello";
adios2::Params engineParams = {
    {"IPAddress", "127.0.0.1"},
    {"Verbose", "11"},
};

int writerRank, writerSize;
int worldRank, worldSize;

size_t steps = 100;

std::map<std::string, std::chrono::duration<double>> durations;

void runTest(const std::string &adiosEngine)
{
    MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

    int color=0;

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
    adios2::IO io = adios.DeclareIO(ioName);
    io.SetEngine(adiosEngine);
    io.SetParameters(engineParams);

    adios2::Engine engine = io.Open(fileName, adios2::Mode::Write);
    auto bpFloats = io.DefineVariable<float>("bpFloats", shape, start, count);

    auto timerStart = std::chrono::system_clock::now();
    for (int i = 0; i < steps; ++i)
    {
        engine.BeginStep();
        engine.Put(bpFloats, myFloats.data());
        engine.EndStep();
    }
    auto timerEnd = std::chrono::system_clock::now();
    durations[adiosEngine] = timerEnd - timerStart;

    engine.Close();

}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    runTest("insitumpi");

    MPI_Finalize();

    if(worldRank == 0)
    {
        for(const auto &i : durations)
        {
            std::cout << i.first << " time " << i.second.count() << " seconds" << std::endl;
        }
    }

    return 0;
}
