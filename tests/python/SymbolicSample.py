" Test Symbolic Matrices Python binding. "

from setup import *

def symbolicSample():
    row = 128
    col = 128
    
    # Create a variable X and an identity function
    X = AMD.symMatrMat("X", row, col)
    fX = AMD.symMMF(X, False)

    # Create constants AB, and C and identity functions
    A = AMD.symMatrMat("A", row, col)
    fA = AMD.symMMF(A, True)

    # Create scalar-matrix function
    func = AMD.trace(fA * AMD.inv(fX))

    # Output the function value and derivative value
    print 'Function value: {}'.format(func.functionVal.string)
    print 'Derivative value: {}'.format(func.derivativeVal.string)

def main():
    """ Sample script that uses symbolicMMF """ 
    symbolicSample()
    print 'Done.'

if __name__ == '__main__':
    main()

