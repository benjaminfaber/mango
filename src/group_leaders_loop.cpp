#include<iostream>
#include "mango.hpp"

void mango::problem::group_leaders_loop() {
  double* state_vector = new double[N_parameters];
  double* gradient = new double[N_parameters];

  int data;
  double dummy;

  bool keep_going = true;
  while (keep_going) {
    /* Wait for proc 0 to send us a message. */
    MPI_Bcast(&data,1,MPI_INT,0,mpi_comm_group_leaders);
    if (data < 0) {
      std::cout << "proc " << mpi_rank_world << " (a group leader) is exiting.\n";
      keep_going = false;
    } else {
      std::cout << "proc " << mpi_rank_world << " (a group leader) is starting finite-difference gradient calculation.\n";
      finite_difference_gradient(state_vector, &dummy, gradient); 
    }
  }

  delete[] state_vector;
  delete[] gradient;
}