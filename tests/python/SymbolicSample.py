" Test Symbolic Matrices Python binding. "

from setup import *

def symbolicSample():
    row = 128
    col = 128
    
    # Create a variable X and an identity function
    X = symMatrMat("X", row, col)
    fX = symMMF(X, False)

    # Create constants AB, and C and identity functions
    A = symMatrMat("A", row, col)
    fA = symMMF(A, False)

    try:
        # Create scalar-matrix function
        func = trace(fX)
        #func = trace(fA * inv(fX))
        # Output the function value and derivative value
        print 'Function value: {}'.format(func.functionVal.string)
        print 'Derivative value: {}'.format(func.derivativeVal.string)
    except AMDException, e:
        print 'Trace: ', e.trace
        print 'What: ', e.what
        print 'Error code: ', e.errorCode

def main():
    """ Sample script that uses symbolicMMF """ 
    symbolicSample()
    print 'Done.'

if __name__ == '__main__':
    main()

