#pragma once

#include <limits>

#include <omp.h>

#include "network.h"

namespace parallel1a {
    void run_edmonds_karp(Network& network);
}
