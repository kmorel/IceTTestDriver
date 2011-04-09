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

int do_comm(const std::vector<char *> &arg, MPI_Comm mpi_comm)
{
    IceTCommunicator comm = icetCreateMPICommunicator(mpi_comm);
    IceTContext context = icetCreateContext(comm);
    icetDestroyMPICommunicator(comm);

    int result = SimpleTiming(arg.size(), &arg.at(0));

    icetDestroyContext(context);

    return result;
}

int do_comm_size(const std::vector<char *> &arg)
{
    int result = 0;

    result += do_comm(arg, MPI_COMM_WORLD);

    int rank;
    int world_comm_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_comm_size);

    int my_size = 0;
    int current_size = world_comm_size;
    int top_group_rank = 0;
    const int MIN_GROUP_SIZE = 512;

    while (ICET_TRUE) {
        current_size /= 2;
        top_group_rank += current_size;
        if ((rank < top_group_rank) || (current_size < MIN_GROUP_SIZE)) {
            my_size = current_size;
            break;
        }
    }

    MPI_Comm sub_comm;
    MPI_Comm_split(MPI_COMM_WORLD, my_size, rank, &sub_comm);

    if (my_size >= MIN_GROUP_SIZE) {
        do_comm(arg, sub_comm);
    }

    MPI_Comm_free(&sub_comm);

    return result;
}

int do_strategy(const std::vector<char *> &arg)
{
#if 0
    int result = 0;

    std::vector<char *> strategies_to_try;
    strategies_to_try.push_back("-bswap");
    strategies_to_try.push_back("-fold-bswap");
    strategies_to_try.push_back("-radixk");

    for (std::vector<char *>::iterator strategy = strategies_to_try.begin();
         strategy != strategies_to_try.end();
         strategy++) {
        std::vector<char *>new_arg = arg;
        new_arg.push_back(*strategy);
        result += do_comm_size(new_arg);
    }

    return result;
#else
    std::vector<char *>new_arg = arg;
    new_arg.push_back("-magic-k-study");
    new_arg.push_back("64");

    return do_comm_size(new_arg);
#endif
}

int do_transparent(const std::vector<char *> &arg)
{
    std::vector<char *> transparent_arg(arg);
    transparent_arg.push_back("-transparent");

    int result = 0;
    result += do_strategy(transparent_arg);
    result += do_strategy(arg);

    return result;
}

int do_collect(const std::vector<char *> &arg)
{
    std::vector<char *> no_collect_arg(arg);
    no_collect_arg.push_back("-no-collect");

    int result = 0;
    // result += do_transparent(no_collect_arg);
    result += do_transparent(arg);

    return result;
}

int do_image_size(const std::vector<char *> &arg)
{
    const IceTSizeType begin_size = 2048;
    const IceTSizeType end_size = 2048;
    int result = 0;

    for (IceTSizeType dim = begin_size; dim <= end_size; dim *= 2) {
        SCREEN_WIDTH = SCREEN_HEIGHT = dim;
        result += do_collect(arg);
    }

    return result;
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
    arg.push_back("-sequential");
    // arg.push_back("-max-image-split-study");
    // arg.push_back("256");

    int result = do_image_size(arg);

    MPI_Finalize();

    return result;
}
