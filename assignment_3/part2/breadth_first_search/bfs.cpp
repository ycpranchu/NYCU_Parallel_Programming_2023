#include "bfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <omp.h>
#include <vector>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1
#define PAD 8

void vertex_set_clear(vertex_set *list)
{
    list->count = 0;
}

void vertex_set_init(vertex_set *list, int count)
{
    list->max_vertices = count;
    list->vertices = (int *)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}

// Take one step of "top-down" BFS.  For each vertex on the frontier,
// follow all outgoing edges, and add all neighboring vertices to the
// new_frontier.
void top_down_step(Graph g, vertex_set *frontier, vertex_set *new_frontier, int *distances)
{
    #pragma omp parallel
    {
        Vertex *local_new_frontier = new Vertex[g->num_nodes];
        int local_count = 0;

        #pragma omp for schedule(dynamic, 1024)
        for (int i = 0; i < frontier->count; ++i)
        {
            int node = frontier->vertices[i];

            int start_edge = g->outgoing_starts[node];
            int end_edge = (node == g->num_nodes - 1) ? g->num_edges : g->outgoing_starts[node + 1];

            // attempt to add all neighbors to the new frontier
            for (int neighbor = start_edge; neighbor < end_edge; ++neighbor)
            {
                int outgoing = g->outgoing_edges[neighbor];

                if (distances[outgoing] == NOT_VISITED_MARKER)
                {
                    if (__sync_bool_compare_and_swap(&distances[outgoing], NOT_VISITED_MARKER, distances[node] + 1))
                    {
                        local_new_frontier[local_count++] = outgoing;
                    }
                }
            }
        }

        #pragma omp critical
        {
            memcpy(new_frontier->vertices + new_frontier->count, local_new_frontier, sizeof(int) * local_count);
            new_frontier->count += local_count;
        }

        delete [] local_new_frontier;
    }
}

// Implements top-down BFS.
//
// Result of execution is that, for each node in the graph, the
// distance to the root is stored in sol.distances.
void bfs_top_down(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    // initialize all nodes to NOT_VISITED
    #pragma omp parallel for
    for (int i = 0; i < graph->num_nodes; i++)
        sol->distances[i] = NOT_VISITED_MARKER;

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0)
    {
        #ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
        #endif

        vertex_set_clear(new_frontier);
        top_down_step(graph, frontier, new_frontier, sol->distances);

        #ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
        #endif

        // swap pointers
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;
    }
}

void bottom_up_step(Graph g, vertex_set *frontier, vertex_set *new_frontier, int *distances, int *found)
{
    int level = distances[frontier->vertices[0]] + 1;

    #pragma omp parallel
    {
        Vertex *local_new_frontier = new Vertex[g->num_nodes];
        int local_count = 0;

        #pragma omp for schedule(dynamic, 1024)
        for (int vertex = 0; vertex < g->num_nodes; ++vertex)
        {
            if (distances[vertex] == NOT_VISITED_MARKER)
            {
                int start_edge = g->incoming_starts[vertex];
                int end_edge = (vertex == g->num_nodes - 1) ? g->num_edges : g->incoming_starts[vertex + 1];

                for (int neighbor = start_edge; neighbor < end_edge; ++neighbor)
                {
                    int incoming = g->incoming_edges[neighbor];

                    if (found[incoming] == 1)
                    {
                        distances[vertex] = level;
                        local_new_frontier[local_count++] = vertex;
                        break;
                    }
                }
            }
        }

        #pragma omp critical
        {
            memcpy(new_frontier->vertices + new_frontier->count, local_new_frontier, sizeof(int) * local_count);
            new_frontier->count += local_count;
        }

        delete [] local_new_frontier;
    }
}

void bfs_bottom_up(Graph graph, solution *sol)
{
    // For PP students:
    //
    // You will need to implement the "bottom up" BFS here as
    // described in the handout.
    //
    // As a result of your code's execution, sol.distances should be
    // correctly populated for all nodes in the graph.
    //
    // As was done in the top-down case, you may wish to organize your
    // code by creating subroutine bottom_up_step() that is called in
    // each step of the BFS process.

    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;
    int *found = (int *)calloc(graph->num_nodes, sizeof(int));

    // initialize all nodes to NOT_VISITED
    #pragma omp parallel for
    for (int i = 0; i < graph->num_nodes; i++)
    {
        sol->distances[i] = NOT_VISITED_MARKER;
        found[i] = 0;
    }

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;
    found[ROOT_NODE_ID] = 1;

    while (frontier->count != 0)
    {
        vertex_set_clear(new_frontier);
        bottom_up_step(graph, frontier, new_frontier, sol->distances, found);
 
        // swap pointers
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;

        memset(found, 0, sizeof(found));
        for (int i = 0; i < frontier->count; i++)
            found[frontier->vertices[i]] = 1;
    }
}

void bfs_hybrid(Graph graph, solution *sol)
{
    // For PP students:
    //
    // You will need to implement the "hybrid" BFS here as
    // described in the handout.

    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;
    int *found = (int *)calloc(graph->num_nodes, sizeof(int));

    // initialize all nodes to NOT_VISITED
    #pragma omp parallel for
    for (int i = 0; i < graph->num_nodes; i++)
    {
        sol->distances[i] = NOT_VISITED_MARKER;
        found[i] = 0;
    }

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;
    found[ROOT_NODE_ID] = 1;

    int mode = 0;
    int alpha = 14, beta = 24;

    while (frontier->count != 0)
    {
        vertex_set_clear(new_frontier);

        if (mode == 0)
            top_down_step(graph, frontier, new_frontier, sol->distances);
        else
            bottom_up_step(graph, frontier, new_frontier, sol->distances, found);

        int g_s = new_frontier->count - frontier->count;

        if (mode == 0 && g_s >= 0) // growing up
        {
            int mf = new_frontier->count;
            int mu = graph->num_nodes;
            if (alpha * mf > mu) mode = 1;
        }
        else if (mode == 1 && g_s < 0) // shrinking up
        {
            int nf = frontier->count;
            int nu = graph->num_nodes;
            if (beta * nf < nu) mode = 0;
        }

        // swap pointers
        vertex_set *tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;

        memset(found, 0, sizeof(found));
        for (int i = 0; i < frontier->count; i++)
            found[frontier->vertices[i]] = 1;
    }
}
