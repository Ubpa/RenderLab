#include <Basic/Parallel.h>

using namespace Ubpa;

#include <omp.h>

Parallel::Parallel()
: coreNum(static_cast<size_t>(omp_get_num_procs())) { }

