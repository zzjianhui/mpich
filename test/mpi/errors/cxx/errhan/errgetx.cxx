/*

  errgetx.cxx

  Tests whether a Get_errhandler following a Set_errhandler retrieves the same
  value as was set.
*/

#include <mpi.h>
#include <iostream>

/* #define VERBOSE */

template <class T>
void efn(T &obj, int *, ...)
{
}

/* returns number of errors found */
template <class T>
int testRetrieveErrhandler(T &obj)
{
    int errs = 0;
    MPI::Errhandler retrieved_handler;

    int errorClass = MPI::Add_error_class();
    int errorCode = MPI::Add_error_code(errorClass);
    std::string errorString = "Internal-use Error Code";
    MPI::Add_error_string(errorCode, errorString.c_str());
    MPI::Errhandler custom_handler = T::Create_errhandler(&efn);

    obj.Set_errhandler(MPI::ERRORS_THROW_EXCEPTIONS);
    retrieved_handler = obj.Get_errhandler();
    if (retrieved_handler != MPI::ERRORS_THROW_EXCEPTIONS) errs++;

    obj.Set_errhandler(MPI::ERRORS_RETURN);
    retrieved_handler = obj.Get_errhandler();
    if (retrieved_handler != MPI::ERRORS_RETURN) errs++;

    obj.Set_errhandler(MPI::ERRORS_ARE_FATAL);
    retrieved_handler = obj.Get_errhandler();
    if (retrieved_handler != MPI::ERRORS_ARE_FATAL) errs++;

    obj.Set_errhandler(custom_handler);
    retrieved_handler = obj.Get_errhandler();
    if (retrieved_handler != custom_handler) errs++;

    return errs;
}

int main( int argc, char *argv[] )
{
    int errs = 0;
    MPI::Win win = MPI::WIN_NULL;
    MPI::File file = MPI::FILE_NULL;

    MPI::Init();

    const unsigned int rank = MPI::COMM_WORLD.Get_rank();

    win = MPI::Win::Create(NULL, 0, 1, MPI::INFO_NULL, MPI_COMM_WORLD);
    file = MPI::File::Open(MPI::COMM_WORLD, "testfile", MPI::MODE_WRONLY | MPI::MODE_CREATE | MPI::MODE_DELETE_ON_CLOSE, MPI::INFO_NULL);

    if (0 == rank) {
        errs += testRetrieveErrhandler(MPI::COMM_WORLD);
        errs += testRetrieveErrhandler(win);
        errs += testRetrieveErrhandler(file);
    }

    if (errs == 0) {
        std::cout << " No Errors" << std::endl;
    }
    else {
        std::cout << " Found " << errs << " errors" << std::endl;
    }

    win.Free();
    file.Close();

    MPI::Finalize();

    return 0;
}
