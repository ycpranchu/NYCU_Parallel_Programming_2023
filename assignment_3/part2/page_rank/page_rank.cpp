#include "page_rank.h"

#include <stdlib.h>
#include <cmath>
#include <omp.h>
#include <utility>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define PAD 1

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
	double equal_prob = 1.0 / numNodes;
	bool converged = false;

	double *score_old;
	double *score_new;
	double temp = (1.0 - damping) / (double)numNodes;

	score_old = (double *)malloc(numNodes * sizeof(double));
	score_new = (double *)malloc(numNodes * sizeof(double));

	#pragma omp parallel for
	for (int vi = 0; vi < numNodes; ++vi)
	{
		score_old[vi] = equal_prob;
		score_new[vi] = temp;
	}

	while (!converged)
	{
		double global_diff = 0.0;
		double not_outgoing = 0.0;

		#pragma omp parallel for reduction(+:not_outgoing)
		for (int v = 0; v < numNodes; ++v)
		{
			if (outgoing_size(g, v) == 0)
				not_outgoing += score_old[v];
		}

		not_outgoing = not_outgoing * damping / (double)numNodes;

		#pragma omp parallel for
		for (int vi = 0; vi < numNodes; ++vi)
		{
			if (incoming_size(g, vi) == 0) continue;

			const Vertex *start = incoming_begin(g, vi);
			const Vertex *end = incoming_end(g, vi);
			double sum = 0.0;

			for (const Vertex *vj = start; vj != end; ++vj)
			{
				int size = outgoing_size(g, *vj);
				sum += score_old[*vj] / size;
			}

			score_new[vi] += damping * sum;
		}

		#pragma omp parallel for reduction(+:global_diff)
		for (int vi = 0; vi < numNodes; ++vi)
		{
			score_new[vi] += not_outgoing;
			global_diff += abs(score_new[vi] - score_old[vi]);
			score_old[vi] = score_new[vi];
			score_new[vi] = temp;
		}

		// compute how much per-node scores have changed
		// quit once algorithm has converged
		converged = (global_diff < convergence);
	}

	#pragma omp parallel for
	for (int vi = 0; vi < numNodes; ++vi) solution[vi] = score_old[vi];
	
	free(score_old);
	free(score_new);
}
