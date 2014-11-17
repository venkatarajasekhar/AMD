" Test MatrixMatrixFunc and ScalarMatrixFunc Python binding. "

from setup import *

def testSymMMF():
    row, col = 128, 128
    
    X = symMatrMat("X", row, col)
    fX = symMMF(X, False)

    A = symMatrMat("A", row, col)
    fA = symMMF(A, True)

    print fA.varNumRows
    print fX.varNumRows
    print fA.varNumCols
    print fX.varNumCols

    func = trace(fA*fX)
    assert func == "A'"


def main():
    """ Test MatrixMatrixFunc and ScalarMatrixFunc Python binding """
    testSymMMF()
    print 'Done'

if __name__ == '__main__':
    main()

