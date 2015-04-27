// For Eigen matrix operations, please see:
// http://eigen.tuxfamily.org/dox-devel/group__TutorialMatrixArithmetic.html

#include <AMD/exception.hpp>
#include <AMD/randommatrices.hpp>
#include <AMD/matrixadaptor.hpp>
#include <AMD/eigenmatrixadaptor.hpp>
#include <boost/shared_ptr.hpp>

#define BOOST_TEST_MODULE EigenMatrixAdaptorTest
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(SparseAdaptorTest)
{
    AMD_START_TRY_BLOCK()

    // Create some convenient typedefs 
    typedef Eigen::SparseMatrix<double> SparseMatrixType;
    typedef boost::shared_ptr<SparseMatrixType> SparseMatrixPtrType;
    typedef AMD::MatrixAdaptor_t<SparseMatrixType> MatrixAdaptorType;

    // Create two random symmetric positive semi-definite matrices.
    SparseMatrixPtrType A=AMD::rand_psd_t<SparseMatrixType>::apply(100, 1000);
    SparseMatrixPtrType B=AMD::rand_psd_t<SparseMatrixType>::apply(100, 1000);

    // Create a third matrix to hold the results
    SparseMatrixType C;

    // Check the dimensions.
    BOOST_CHECK(100==MatrixAdaptorType::getNumRows(*A));
    BOOST_CHECK(100==MatrixAdaptorType::getNumCols(*A));

    // Some examples of using MatrixAdaptorType
    MatrixAdaptorType::add(*A, *B, C);
    MatrixAdaptorType::minus(*A, *B, C);

    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()
}

BOOST_AUTO_TEST_CASE(DenseAdaptorTest)
{
    AMD_START_TRY_BLOCK()

    // Create some convenient typedefs 
    typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> 
                                                  DenseMatrixType;
    typedef boost::shared_ptr<DenseMatrixType> DenseMatrixPtrType;
    typedef AMD::MatrixAdaptor_t<DenseMatrixType> MatrixAdaptorType;

    // Create two random symmetric positive semi-definite matrices.
    DenseMatrixPtrType A=AMD::rand_psd_t<DenseMatrixType>::apply(100, 1000);
    DenseMatrixPtrType B=AMD::rand_psd_t<DenseMatrixType>::apply(100, 1000);

    // Create a third matrix to hold the results
    DenseMatrixType C;

    // Check the dimensions.
    BOOST_CHECK(100==MatrixAdaptorType::getNumRows(*A));
    BOOST_CHECK(100==MatrixAdaptorType::getNumCols(*A));

    // Some examples of using MatrixAdaptorType
    MatrixAdaptorType::add(*A, *B, C);
    MatrixAdaptorType::minus(*A, *B, C);

    AMD_END_TRY_BLOCK()
    AMD_CATCH_AND_PRINT()
}
