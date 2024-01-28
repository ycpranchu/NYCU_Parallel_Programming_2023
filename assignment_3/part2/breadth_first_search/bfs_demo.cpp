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
#define ALPHA 14
#define BETA 24
#define MAX_THREAD_NUM 4

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
void top_down_step(
    Graph g,
    vertex_set *frontier,
    vertex_set *new_frontier,
    int *distances)
{
     #pragma omp parallel
    {
        // use local variables for sync.
        int temp_count = 0 ;
        Vertex* temp_frontier = (Vertex*)malloc(sizeof(Vertex)*g->num_nodes);

        #pragma omp for 
        for (int i = 0; i < frontier->count; i++)
        {

            int node = frontier->vertices[i];

            int start_edge = g->outgoing_starts[node];
            int end_edge = (node == g->num_nodes - 1)
                            ? g->num_edges
                            : g->outgoing_starts[node + 1];

            // attempt to add all neighbors to local frontier
            for (int neighbor = start_edge; neighbor < end_edge; neighbor++)
            {
                int outgoing = g->outgoing_edges[neighbor];
                if(distances[outgoing] == NOT_VISITED_MARKER)
                {
                    if(__sync_bool_compare_and_swap(&distances[outgoing], NOT_VISITED_MARKER, distances[node] + 1)){
                        temp_frontier[temp_count] = outgoing;
                        temp_count++;
                    }
                }
            }
        }
        // memcpy to new frontier
        #pragma omp critical
        {
            memcpy(new_frontier->vertices + new_frontier->count, temp_frontier, sizeof(int) * temp_count);
            new_frontier -> count += temp_count;
        }

        free(temp_frontier);
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

void bottom_up_step(
    Graph g,
    vertex_set *frontier,
    vertex_set *new_frontier,
    int *distances,
    bool *f)
{
    int add_d = distances[frontier->vertices[0]]+1;
    std::vector<int> buffer[MAX_THREAD_NUM];
#pragma omp parallel for
    for(int i=0;i<g->num_nodes;++i){
        if(distances[i]==NOT_VISITED_MARKER){
            int tid = omp_get_thread_num();
            int start_edge = g->incoming_starts[i];
            int end_edge = (i==g->num_nodes-1) ? g->num_edges : g->incoming_starts[i+1];
            for(int neighbor = start_edge;neighbor < end_edge; ++neighbor){
                int in_node = g->incoming_edges[neighbor];
                if(f[in_node]){
                    distances[i] = add_d;
                    buffer[tid].push_back(i);
                    break;
                }
            }
        }
    }

    int *v = new_frontier->vertices;
#pragma omp parallel for
    for(int i=0;i<MAX_THREAD_NUM;++i){
        int len = buffer[i].size();
        if(len==0)
            continue;
        int index = __sync_fetch_and_add(&new_frontier->count,len);
        for(int j=0;j<len;++j){
            v[index+j]=buffer[i][j];
        }
    }
}

void bfs_bottom_up(Graph graph, solution *sol)
{
    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    bool *f = (bool *)calloc(graph->num_nodes,sizeof(bool));
    // initialize all nodes to NOT_VISITED
    #pragma omp parallel for
    for (int i = 0; i < graph->num_nodes; i++){
        sol->distances[i] = NOT_VISITED_MARKER;
    }
        
    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;
    f[ROOT_NODE_ID] = true;

    while (frontier->count != 0)
    {

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier);

        bottom_up_step(graph, frontier, new_frontier, sol->distances,f);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        // swap pointers
        memset(f,0,sizeof(f));
        int *v = new_frontier->vertices;
        for(int i=0;i<new_frontier->count;++i){
            f[v[i]] = true;
        }
        std::swap(frontier,new_frontier);

    }
}


void bfs_hybrid(Graph graph, solution *sol)
{
    // Initialization
    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set *frontier = &list1;
    vertex_set *new_frontier = &list2;

    int state = 0;
    bool *f = (bool *)calloc(graph->num_nodes,sizeof(bool));

    // initialize all nodes to NOT_VISITED
    #pragma omp parallel for
    for (int i = 0; i < graph->num_nodes; i++)
        sol->distances[i] = NOT_VISITED_MARKER;

    // Setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;  // id: 0
    sol->distances[ROOT_NODE_ID] = 0;
    f[ROOT_NODE_ID] = true;

    while (frontier->count != 0)
    {

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier);
        if(state == 0)
            top_down_step(graph, frontier, new_frontier, sol->distances);
        else
            bottom_up_step(graph, frontier, new_frontier, sol->distances,f);
#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        double bf = (double) (new_frontier->count - frontier->count) / frontier->count;
        if(state == 0 && new_frontier->count > frontier->count){
            int mf = new_frontier->count * bf;
            int mu = graph->num_nodes * bf;
            if(mf > (mu/ALPHA))
                state = 1;
        }
        else if(state == 1 && new_frontier->count < frontier->count){
            int nf = frontier->count;
            int n = graph->num_nodes;
            if(nf < (n / BETA))
                state = 0;
        }

        // swap pointers
        memset(f,0,sizeof(f));
        int *v = new_frontier->vertices;
        for(int i=0;i<new_frontier->count;++i){
            f[v[i]] = true;
        }

        std::swap(frontier,new_frontier);
    }
}