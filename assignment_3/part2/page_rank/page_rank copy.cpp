#include "page_rank.h"

#include <stdlib.h>
#include <cmath>
#include <omp.h>
#include <utility>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define PAD 8

// pageRank --
//
// g:           graph to process (see common/graph.h)
// solution:    array of per-vertex vertex scores (length of array is num_nodes(g))
// damping:     page-rank algorithm's damping parameter
// convergence: page-rank algorithm's convergence threshold
//
void pageRank(Graph g, double *solution, double damping, double convergence)
{

	// initialize vertex weights to uniform probability. Double
	// precision scores are used to avoid underflow for large graphs

	/*
	   For PP students: Implement the page rank algorithm here.  You
	   are expected to parallelize the algorithm using openMP.  Your
	   solution may need to allocate (and free) temporary arrays.

	   Basic page rank pseudocode is provided below to get you started:

	   // initialization: see example code above
	   score_old[vi] = 1/numNodes;

	   while (!converged) {

		 // compute score_new[vi] for all nodes vi:
		 score_new[vi] = sum over all nodes vj reachable from incoming edges
							{ score_old[vj] / number of edges leaving vj  }
		 score_new[vi] = (damping * score_new[vi]) + (1.0-damping) / numNodes;

		 score_new[vi] += sum over all nodes v in graph with no outgoing edges
							{ damping * score_old[v] / numNodes }

		 // compute how much per-node scores have changed
		 // quit once algorithm has converged

		 global_diff = sum over all nodes vi { abs(score_new[vi] - score_old[vi]) };
		 converged = (global_diff < convergence)
	   }

	 */

	int numNodes = num_nodes(g);
	int num_threads = 0;
	double equal_prob = 1.0 / numNodes;
	bool converged = false;

	double *score_old;
	double *score_new;
	double **buffer;

	score_old = (double *)malloc(numNodes * sizeof(double));
	score_new = (double *)malloc(numNodes * sizeof(double));

#pragma omp parallel
	{
		num_threads = omp_get_num_threads();

		for (int vi = 0; vi < numNodes; ++vi)
		{
			score_old[vi] = equal_prob;
			score_new[vi] = 0.0;
		}
	}

	buffer = (double **)malloc(num_threads * sizeof(double *));
	for (int i = 0; i < num_threads; i++)
	{
		buffer[i] = (double *)malloc(PAD * sizeof(double));
	}

	while (!converged)
	{
		double global_diff = 0.0;
		double not_outgoing = 0.0;

#pragma omp parallel
		{
			int id = omp_get_thread_num();
			buffer[id][0] = 0.0;

			for (int v = id; v < numNodes; v += num_threads)
			{
				if (outgoing_size(g, v) == 0)
					buffer[id][0] += damping * score_old[v] / (double)numNodes;
			}

#pragma omp critical
			{
				not_outgoing += buffer[id][0];
			}
		}

		// compute score_new[vi] for all nodes vi:

#pragma omp parallel for
		for (int vi = 0; vi < numNodes; ++vi)
		{
			if (incoming_size(g, vi) == 0)
			{
				score_new[vi] = (1.0 - damping) / (double)numNodes;
				continue;
			}

			const Vertex *start = incoming_begin(g, vi);
			const Vertex *end = incoming_end(g, vi);

			double sum = 0.0, size = 0.0;
			for (const Vertex *vj = start; vj != end; vj++)
			{
				size = (double)outgoing_size(g, *vj);
				sum += score_old[*vj] / size;
			}

			score_new[vi] = (damping * sum) + (1.0 - damping) / (double)numNodes;
		}

#pragma omp parallel for
		for (int vi = 0; vi < numNodes; ++vi)
		{
			score_new[vi] += not_outgoing;
		}

#pragma omp parallel
		{
			int id = omp_get_thread_num();
			buffer[id][0] = 0.0;

			for (int vi = id; vi < numNodes; vi += num_threads)
			{
				buffer[id][0] += abs(score_new[vi] - score_old[vi]);
			}

#pragma omp critical
			{
				global_diff += buffer[id][0];
			}
		}

#pragma omp parallel for
		for (int vi = 0; vi < numNodes; ++vi)
		{
			score_old[vi] = score_new[vi];
			score_new[vi] = 0.0;
		}

		// compute how much per-node scores have changed
		// quit once algorithm has converged
		converged = (global_diff < convergence);
	}

#pragma omp parallel for
	for (int vi = 0; vi < numNodes; ++vi)
	{
		solution[vi] = score_old[vi];
	}

	free(score_old);
	free(score_new);
}
