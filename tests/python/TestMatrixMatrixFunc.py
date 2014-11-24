" Test MatrixMatrixFunc and ScalarMatrixFunc Python binding. "

from setup import *

def testSymMMF():
    row, col = 128, 128
    
    X = symMatrMat("X", row, col)
    fX = symMMF(X, False)

    A = symMatrMat("A", row, col)
    fA = symMMF(A, False) #TODO make 2nd argument True when exceptions are fixed
    
    try :
        fX.
        func = trace(fX)
        assert func == "I"

        #func = trace(transpose(fX)*transpose(fA))
        #assert func == "A'"

        #func = trace(transpose(fX*fA))
        #assert func == "A'"

    except AMDExceptionType, e:
        print e

def main():
    """ Test MatrixMatrixFunc and ScalarMatrixFunc Python binding """
    testSymMMF()
    print 'Done'

if __name__ == '__main__':
    main()

