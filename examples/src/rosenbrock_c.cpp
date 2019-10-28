#include<iostream>
#include<iomanip>
#include<mpi.h>
#include "mango.hpp"

void residual_function(int*, const double*, int*, double*, int*, mango::problem*);

void worker(mango::problem*);

int main(int argc, char *argv[]) {
  int ierr;

  std::cout << "Hello world from rosenbrock_c.\n";

  ierr = MPI_Init(&argc, &argv);
  if (ierr != 0) {
    std::cout << "\nError in MPI_Init.\n";
    exit(1);
  }

  double state_vector[2] = {0.0, 0.0};
  double targets[2] = {1.0, 0.0};
  double sigmas[2] = {1.0, 0.1};

  mango::problem myprob(2, state_vector, 2, targets, sigmas, &residual_function, argc, argv);


  /*  myprob.set_algorithm(mango::PETSC_POUNDERS); */
  // myprob.set_algorithm("nlopt_ln_neldermead");
  myprob.read_input_file("../input/mango_in.rosenbrock_c");
  myprob.output_filename = "../output/mango_out.rosenbrock_c";
  myprob.mpi_init(MPI_COMM_WORLD);
  /* myprob.centered_differences = true; */
  myprob.max_function_evaluations = 2000;

  if (myprob.is_proc0_worker_groups()) {
    myprob.optimize();
    /* Make workers stop */

    int data[1];
    data[0] = -1;
    MPI_Bcast(data, 1, MPI_INT, 0, myprob.get_mpi_comm_worker_groups());
  } else {
    worker(&myprob);
  }

  MPI_Finalize();

  return 0;
}


void residual_function(int* N_parameters, const double* x, int* N_terms, double* f, int* failed, mango::problem* this_problem) {
  int j;
  std::cout << "C residual function called with N="<< *N_parameters << "\n";
  /*   *f = (x[0] - 1) * (x[0] - 1) + 100 * (x[1] - x[0]*x[0]) * (x[1] - x[0]*x[0]); */
  f[0] = x[0];
  f[1] = x[1] - x[0] * x[0];
  *failed = false;
}


void worker(mango::problem* myprob) {
  bool keep_going = true;
  int data[1];

  while (keep_going) {
    MPI_Bcast(data, 1, MPI_INT, 0, myprob->get_mpi_comm_worker_groups());
    if (data[0] < 0) {
      std::cout << "Proc " << std::setw(5) << myprob->get_mpi_rank_world() << " is exiting.\n";
      keep_going = false;
    } else {
      std::cout<< "Proc " << std::setw(5) << myprob->get_mpi_rank_world() << " is doing calculation " << data[0] << "\n";
      /* For this problem, the workers don't actually do any work. */
    }
  }
}
