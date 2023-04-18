//
// Une sorte de "Hello World" pour les programmes MPI
// https://h-deb.clg.qc.ca/Sujets/Parallelisme/MPI.html
//
#include <mpi.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>

class MPI_Scope {
  int nprocs_;
  int rang_;

  void initialiser_mpi(int &argc, char **argv)
  {
    MPI_Init(&argc, &argv);  // une fois par processus, dans le thread principal
  }

public:
  MPI_Scope(const MPI_Scope&) = delete;
  MPI_Scope& operator=(const MPI_Scope&) = delete;

  MPI_Scope(int argc, char *argv[]) {
    initialiser_mpi(argc, argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs_);  // Taille du monde == combien de processus participent
    MPI_Comm_rank(MPI_COMM_WORLD, &rang_);  // Dans le monde, qui suis-je?
  }

  int nb_processus() const noexcept { return nprocs_; }
  int rang() const noexcept { return rang_; }

  // une fois par processus, dans le thread principal
  ~MPI_Scope() {
    MPI_Finalize();
  }
};

void send_string(const std::string &msg, int destinataire, int msg_tag) {
  MPI_Send(&msg[0], static_cast<int>(msg.size()), MPI_CHAR, destinataire, msg_tag, MPI_COMM_WORLD);
}

template <int BUFSIZE>
std::string recv_string(int emetteur, int msg_tag) {
  char buff[BUFSIZE];
  MPI_Status stat; 
  MPI_Recv(buff, BUFSIZE, MPI_CHAR, emetteur, msg_tag, MPI_COMM_WORLD, &stat);
  int nrecus;
  MPI_Get_count(&stat, MPI_CHAR, &nrecus);
  return std::string{buff, buff + nrecus};
}

int main(int argc, char *argv[]) {
  const int MSG_TAG = 0;
  MPI_Scope mpi_info(argc, argv);

  //
  // Initialement, tous les programmes sont équivalents. Le rang permet de leur attribuer
  // un rôle (typiquement, le processus de rang 0 a droit à un traitement particulier)
  //
  if(mpi_info.rang() == 0) {
    std::cout << "Rang: " << mpi_info.rang() << ", sur un total de " << mpi_info.nb_processus() << " processus" << std::endl;
    for(int i = 1; i < mpi_info.nb_processus(); ++i) {
      std::stringstream sstr;
      sstr << "Coucou " << i << " de 0";
      send_string(sstr.str(), i, MSG_TAG);
    }
    for(int i = 1; i < mpi_info.nb_processus(); ++i) {
      enum { BUFSIZE = 128 };
      std::cout << mpi_info.rang() << ": " << recv_string<BUFSIZE>(i, MSG_TAG)<< std::endl;
    }
  } else {
    enum { BUFSIZE = 128 };
    // Recevoir du processus de rang 0...
    std::cout << mpi_info.rang() << ": " << recv_string<BUFSIZE>(0, MSG_TAG)<< std::endl;
    std::stringstream sstr;
    sstr << "Processus " << mpi_info.rang() << " au boulot";
    // Envoyer au processus de rang 0...
    send_string(sstr.str(), 0, MSG_TAG);
  }
}
