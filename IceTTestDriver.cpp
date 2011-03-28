// -*- c++ -*-

#include <IceT.h>
#include <IceTMPI.h>

#include <vector>

extern "C" int SimpleTiming(int argc, const char * const *argv);

IceTSizeType SCREEN_WIDTH = 0;
IceTSizeType SCREEN_HEIGHT = 0;

extern "C" int run_test(int (*tf)(void))
{
    return tf();
}

int do_comm_size(const std::vector<char *> &arg)
{
    int result = 0;

    int rank;
    int world_comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_comm_size);

    for (int sub_comm_size = world_comm_size;
         sub_comm_size > world_comm_size/2;
         sub_comm_size--) {
        int in_sub_group = (int)(rank < sub_comm_size);
        MPI_Comm sub_comm;
        MPI_Comm_split(MPI_COMM_WORLD, in_sub_group, rank, &sub_comm);

        IceTCommunicator comm = icetCreateMPICommunicator(sub_comm);
        IceTContext context = icetCreateContext(comm);
        icetDestroyMPICommunicator(comm);

        if (in_sub_group) {
            result += SimpleTiming(arg.size(), &arg.at(0));
        }

        icetDestroyContext(context);

        MPI_Comm_free(&sub_comm);
    }

    return result;
}

int do_strategy(const std::vector<char *> &arg)
{
    std::vector<char *> strategies_to_try;
    strategies_to_try.push_back("-bswap");
    strategies_to_try.push_back("-fold-bswap");
    strategies_to_try.push_back("-radixk");

    for (std::vector<char *>::iterator strategy = strategies_to_try.begin();
         strategy != strategies_to_try.end();
         strategy++) {
        std::vector<char *>new_arg = arg;
        new_arg.push_back(*strategy);
        do_comm_size(new_arg);
    }
}

int do_image_size(const std::vector<char *> &arg)
{
    const IceTSizeType begin_size = 2048;
    const IceTSizeType end_size = 8192;

    for (IceTSizeType dim = begin_size; dim <= end_size; dim *= 2) {
        SCREEN_WIDTH = SCREEN_HEIGHT = dim;
        do_strategy(arg);
    }
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
    arg.push_back("11");
    arg.push_back("-no-collect");
    arg.push_back("-transparent");
    arg.push_back("-sequential");

    int result = do_image_size(arg);

    MPI_Finalize();

    return result;
}
