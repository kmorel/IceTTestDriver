// -*- c++ -*-

#include <IceT.h>
#include <IceTMPI.h>

#include <vector>

extern "C" int SimpleTiming(int argc, char **argv);

IceTSizeType SCREEN_WIDTH = 0;
IceTSizeType SCREEN_HEIGHT = 0;

extern "C" int run_test(int (*tf)(void))
{
    return tf();
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    std::vector<char *>arg;
    arg.push_back("SimpleTiming");
    arg.push_back("-seed");
    arg.push_back("123");
    arg.push_back("-sync-render");
    arg.push_back("-frames");
    arg.push_back("101");
    arg.push_back("-no-collect");
    arg.push_back("-transparent");
    arg.push_back("-sequential");

    SCREEN_WIDTH = SCREEN_HEIGHT = 2048;

    IceTCommunicator comm = icetCreateMPICommunicator(MPI_COMM_WORLD);
    IceTContext context = icetCreateContext(comm);
    icetDestroyMPICommunicator(comm);

    int result = SimpleTiming(arg.size(), &arg.at(0));

    icetDestroyContext(context);

    MPI_Finalize();

    return result;
}
