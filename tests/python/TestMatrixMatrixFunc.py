" Test MatrixMatrixFunc and ScalarMatrixFunc Python binding. "

from setup import *

def testSymMMF():
    row, col = 128, 128
    
    X = symMat("X", row, col)
    fX = symMMF(X, False)

    A = symMat("A", row, col)
    fA = symMMF(A, False) #TODO make 2nd argument True when exceptions are fixed
    
    try :
        func = trace(fX)

    except AMDExceptionType, e:
        print e

    print 'fA = ', fA 
    print 'fX = ', fX

def main():
    """ Test MatrixMatrixFunc and ScalarMatrixFunc Python binding """
    testSymMMF()
    print 'Done'

if __name__ == '__main__':
    main()

