" Test Symbolic Matrices Python binding. "

from setup import *

def symbolic_sample():
    row = 128
    col = 128
    
    # Create a variable X and an identity function
    X = SymMat("X", row, col)
    fX = SymMMF(X, False)

    # Create constants AB, and C and identity functions
    A = SymMat("A", row, col)
    fA = SymMMF(A, False)

    try:
        # Create scalar-matrix function
        func = trace(fX)
        # Output the function value and derivative value
        print 'Function value: ', func.function_val
        print 'Derivative value: {}', func.derivative_val
    
    except AMDExceptionType, e:
        print e

def main():
    """ Sample script that uses symbolicMMF """ 
    symbolic_sample()
    print 'Done.'

if __name__ == '__main__':
    main()

