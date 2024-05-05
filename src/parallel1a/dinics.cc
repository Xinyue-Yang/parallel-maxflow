#include "parallel1a/dinics.h"

namespace {
    constexpr auto NONE{-1};
    constexpr auto DUMMY{-2};
    constexpr auto INF{std::numeric_limits<int>::max()};
}

namespace parallel1a {
    void run_dinics(Network& network) {
        const auto num_verts{network.num_verts};
        const auto source{network.source};
        const auto sink{network.sink};
        auto& edges{network.edges};
        const auto& adj{network.adj};

        std::vector<int> edge_in(num_verts);
        std::vector<int> dist(num_verts);

        int frontier_size{};
        std::vector<int> frontier(num_verts);
        int new_frontier_size{};
        std::vector<int> new_frontier(num_verts);

        std::vector<int> curr(num_verts);

        const std::function<int(int, int)>
        push_flow{[&push_flow, &sink, &edges, &adj, &dist, &curr](
            const int u,
            const int flow_in
        ) -> int {
            if (u == sink)
                return flow_in;

            const auto degree{static_cast<int>(std::size(adj[u]))};
            const auto dist_v{dist[u] + 1};

            int flow_out{};
            for (auto& i{curr[u]}; i < degree; ++i) {
                const auto j{adj[u][i]};
                if (auto& [from, to, cap, flow]{edges[j]};
                    flow < cap and dist[to] == dist_v) {
                    const auto flow_pushed{
                        push_flow(to, std::min(flow_in - flow_out, cap - flow))
                    };

                    flow += flow_pushed;
                    edges[j ^ 1].flow -= flow_pushed;

                    if ((flow_out += flow_pushed) == flow_in) {
                        i += static_cast<int>(flow == cap);
                        return flow_out;
                    }
                }
            }

            return flow_out;
        }};

        while (true) {
#pragma omp parallel for default(none) shared(num_verts, edge_in)
            for (int u = 0; u < num_verts; ++u)
                edge_in[u] = NONE;
#pragma omp parallel for default(none) shared(num_verts, dist)
            for (int u = 0; u < num_verts; ++u)
                dist[u] = NONE;

            edge_in[source] = DUMMY;
            dist[source] = 0;
            frontier[frontier_size++] = source;

            while (frontier_size > 0) {
#pragma omp parallel for default(none) shared(adj, edges, edge_in, dist, \
frontier_size, frontier, new_frontier_size, new_frontier)
                for (int i = 0; i < frontier_size; ++i) {
                    const auto u{frontier[i]};
                    const auto dist_v{dist[u] + 1};
                    for (const auto j: adj[u])
                        if (const auto& [from, to, cap, flow]{edges[j]};
                            flow < cap and edge_in[to] == NONE) {
#pragma omp critical
                            edge_in[to] = edge_in[to] == NONE ? j : edge_in[to];
                            if (edge_in[to] == j) {
                                dist[to] = dist_v;

                                int index{};
#pragma omp atomic capture
                                index = new_frontier_size++;
                                new_frontier[index] = to;
                            }
                        }
                }

                frontier_size = new_frontier_size;
                new_frontier_size = 0;
                std::swap(frontier, new_frontier);
            }

            if (dist[sink] == NONE)
                return;

            std::fill(std::begin(curr), std::end(curr), 0);
            while (push_flow(source, INF) > 0);
        }
    }
}
