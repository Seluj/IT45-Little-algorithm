/**
 * Projec : gtsp (voyageur de commerce)
 *
 * Date   : 07/04/2014
 * Author : Olivier Grunder
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <float.h>
#include <stdbool.h>

#define NBR_TOWNS 14

/* Distance matrix */
double dist[NBR_TOWNS][NBR_TOWNS];

/* Each edge has a starting and ending node */
int starting_town[NBR_TOWNS];
int ending_town[NBR_TOWNS];

/* no comment */
int best_solution[NBR_TOWNS];
double best_eval = -1.0;


/**
 * Berlin52 :
 *  6 towns : Best solution (2315.15): 0 1 2 3 5 4
 * 10 towns : Best solution (2826.50): 0 1 6 2 7 8 9 3 5 4
 */
float coord[NBR_TOWNS][2] = {
        {565.0, 575.0},
        {25.0, 185.0},
        {345.0, 750.0},
        {945.0, 685.0},
        {845.0, 655.0},
        {880.0, 660.0},
        {25.0, 230.0},
        {525.0, 1000.0},
        {580.0, 1175.0},
        {650.0, 1130.0},
        {1605.0, 620.0},
        {1220.0, 580.0},
        {1465.0, 200.0},
        {1530.0, 5.0},
    };



/**
 * print a matrix
 */
void print_matrix(double d[NBR_TOWNS][NBR_TOWNS]) {
  int i, j;
  for (i = 0; i < NBR_TOWNS; i++) {
    printf("%d:", i);
    for (j = 0; j < NBR_TOWNS; j++) {
      printf("%6.1f ", d[i][j]);
    }
    printf("\n");
  }
}



/**
 * print a solution
 */
void print_solution(int* sol, double eval) {
   int i;
   printf("(%.2f): ", eval);
   for (i = 0; i < NBR_TOWNS; i++) {
     printf("%d ", sol[i]);
   }

   printf("\n");
}




/**
 * evaluation of a solution
 */
double evaluation_solution(int* sol) {
  double eval = 0;
  int i;
  for (i = 0; i < NBR_TOWNS-1; i++) {
    eval += dist[sol[i]][sol[i+1]];
  }

  eval += dist[sol[NBR_TOWNS-1]][sol[0]];

  return eval;
}


/**
 * nearest neighbour solution
 */
double build_nearest_neighbour() {
  /* solution of the nearest neighbour */
  int i, j, sol[NBR_TOWNS] = {0};

  /* evaluation of the solution */
  double eval = 0;

  int visited[NBR_TOWNS] = {0};

  sol[0] = 0;
  visited[0] = 1;

  for (i = 1; i < NBR_TOWNS; i++) {
    double min = DBL_MAX;
    int indice = 0;
    for (j = 0; j < NBR_TOWNS; j++) {
      if (dist[i][j] < min && visited[j] == 0 && dist[i][j] != -1) {
        min = dist[i][j];
        indice = j;
      }
    }
    visited[indice] = 1;
    sol[i] = indice;
  }

  eval = evaluation_solution(sol);
  printf("Nearest neighbour ");
  print_solution(sol, eval);

  for (i = 0; i < NBR_TOWNS; i++) {
    best_solution[i] = sol[i];
  }
  best_eval = eval;

  return eval;
}




/**
 *  Build final solution
 */
void build_solution() {
  int i, solution[NBR_TOWNS];

  int indiceCour = 0;
  int villeCour = 0;

  while (indiceCour < NBR_TOWNS) {

    solution[indiceCour] = villeCour;

    // Test si le cycle est hamiltonien
    for (i = 0; i < indiceCour; i++) {
      if (solution[i] == villeCour) {
        //printf ("cycle non hamiltonien\n");
        return;
      }
    }

    // Recherche de la ville suivante
    int trouve = 0;
    i = 0;
    while ((!trouve) && (i < NBR_TOWNS)) {
      if (starting_town[i] == villeCour) {
        trouve = 1;
        villeCour = ending_town[i];
      }
      i++;
    }
    indiceCour++;
  }

  double eval = evaluation_solution(solution);

  if (best_eval<0 || eval < best_eval) {
    best_eval = eval;
    for (i = 0; i < NBR_TOWNS; i++) {

      best_solution[i] = solution[i];
    }
    printf("New best solution: ");
    print_solution(solution, best_eval);
  }
}



/**
 *  Little Algorithm
 */
void little_algorithm(double d0[NBR_TOWNS][NBR_TOWNS], int iteration, double eval_node_parent) {

  /*
  printf("\n\n===========================================================================\n");
  printf("Initial matrix:\n");
  print_matrix(d0);
  printf("\n");
*/
  //sleep(2);
  if (iteration == NBR_TOWNS) {
    build_solution();
    return;
  }
  /* Do the modification on a copy of the distance matrix */
  double d[NBR_TOWNS][NBR_TOWNS];
  memcpy (d, d0, NBR_TOWNS*NBR_TOWNS*sizeof(double));
  int i, j;
  double eval_node_child = eval_node_parent;


  /**
    * substract the min of the rows and the min of the columns
    * and update the evaluation of the current node
   */
  double min;
  // En premier lieu, on soustrait le minimum de chaque ligne
  for (i = 0; i < NBR_TOWNS; i++) {
    min = DBL_MAX;
    for (j = 0; j < NBR_TOWNS; j++) {
      if (d[i][j] < min && d[i][j] != -1) {
        min = d[i][j];
      }
    }
    if (min != DBL_MAX) {
      for (j = 0; j < NBR_TOWNS; j++) {
        if (d[i][j] != -1) {
          d[i][j] -= min;
        }
      }
      eval_node_child += min;
    }
  }
  // Ensuite, on soustrait le minimum de chaque colonne
  for (i = 0; i < NBR_TOWNS; i++) {
    min = DBL_MAX;
    for (j = 0; j < NBR_TOWNS; j++) {
      if (d[j][i] < min && d[j][i] != -1) {
        min = d[j][i];
      }
    }
    if (min != DBL_MAX) {
      for (j = 0; j < NBR_TOWNS; j++) {
        if (d[j][i] != -1) {
          d[j][i] -= min;
        }
      }
      eval_node_child += min;
    }
  }

/*
  printf("Matrix after substracting min of rows and columns:\n");
  print_matrix(d);
  printf("\n");
  printf("Evaluation of the node: %f\n", eval_node_child);
*/


  /* Cut : stop the exploration of this node */
  if (best_eval>=0 && eval_node_child >= best_eval) {
    // printf("\nCut\n");
    return;
  }


  /**
    * Compute the penalities to identify the zero with max penalty
    * If no zero in the matrix, then return, solution infeasible
   */
  double zero[NBR_TOWNS][NBR_TOWNS] = {0};
  int zeroCount = 0;
  for (i = 0; i < NBR_TOWNS; i++) {
    for (j = 0; j < NBR_TOWNS; j++) {
      if (d[i][j] == 0) {
        min = DBL_MAX;
        for (int k = 0; k < NBR_TOWNS; k++) {
          if (d[i][k] < min && d[i][k] != -1 && k != j) {
            min = d[i][k];
          }
        }
        zero[i][j] += min;
        min = DBL_MAX;
        for (int k = 0; k < NBR_TOWNS; k++) {
          if (d[k][j] < min && d[k][j] != -1 && k != i) {
            min = d[k][j];
          }
        }
        zero[i][j] += min;
        zeroCount++;
      }
    }
  }
  if (zeroCount == 0) {
    return;
  }
  /* row and column of the zero with the max penalty */
  int izero=-1, jzero=-1;

/*
  printf("Matrix of penalties:\n");
  print_matrix(zero);
  printf("\n");
*/


  /**
    * Store the row and column of the zero with max penalty in
    * starting_town and ending_town
   */
  double max = 0;
  for (i = 0; i < NBR_TOWNS; i++) {
    for (j = 0; j < NBR_TOWNS; j++) {
      if (zero[i][j] > max) {
        max = zero[i][j];
        izero = i;
        jzero = j;
      }
    }
  }
  starting_town[iteration] = izero;
  ending_town[iteration] = jzero;
  //printf("Zero with max penalty: %d %d\n", izero, jzero);
  /* Do the modification on a copy of the distance matrix */
  double d2[NBR_TOWNS][NBR_TOWNS];
  memcpy (d2, d, NBR_TOWNS*NBR_TOWNS*sizeof(double));


  /**
    * Modify the matrix d2 according to the choice of the zero with the max penalty
   */
  for (i = 0; i < NBR_TOWNS; i++) {
    d2[izero][i] = -1;
    d2[i][jzero] = -1;
  }
  d2[jzero][izero] = -1;


  //sleep(2);



  /* Explore left child node according to given choice */
  little_algorithm(d2, iteration + 1, eval_node_child);

  /* Do the modification on a copy of the distance matrix */
  memcpy(d2, d, NBR_TOWNS*NBR_TOWNS*sizeof(double));

  /**
    * Modify the dist matrix to explore the other possibility : the non-choice
    * of the zero with the max penalty
   */
  d2[izero][jzero] = -1;

  /*
  printf("Matrix after choice of the zero with max penalty:\n");
  print_matrix(d2);
  printf("\n");
*/
  /* Explore right child node according to non-choice */
  little_algorithm(d2, iteration, eval_node_child);
}




/**
 *
 */
int main (int argc, char* argv[]) {

  best_eval = -1;

  /* Print problem information */
  printf("Points coordinates:\n");
  int i, j;
  for (i = 0; i < NBR_TOWNS; i++) {
    printf("Node %d: x=%f, y=%f\n", i, coord[i][0], coord[i][1]);
  }
  printf("\n");

  /* Calcul de la matrice des distances */
  for (j = 0; j < NBR_TOWNS; j++) {
    for (i = 0; i < NBR_TOWNS; i++) {
      if (j == i) {
        dist[j][i] = -1;
      } else {
        dist[j][i] = sqrt(pow(coord[j][0] - coord[i][0], 2) + pow(coord[j][1] - coord[i][1], 2));
      }
    }
  }


  printf("Distance Matrix:\n");
  print_matrix(dist);
  printf("\n");
  printf("Best solution: %f\n", best_eval);

  // Nearest Neighbour heuristic :

  double nearest_neighbour = build_nearest_neighbour();
  printf("Best solution: %f\n", best_eval);

  // Little :

  int iteration = 0;
  double lowerbound = 0.0;

  little_algorithm(dist, iteration, lowerbound);
  printf("Best solution:");
  print_solution(best_solution, best_eval);

  printf("Hit RETURN!\n");
  getchar();

  return 0;
}
