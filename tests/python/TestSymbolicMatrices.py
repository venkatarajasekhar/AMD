" Test Symbolic Matrices Python binding. "

from setup import *

def test_symScalMat():
    """ Test symScalMat (SymbolicScalarMatlab) """
    print 'Testing symScalMat...'
    a = symScalMat('a')
    b = symScalMat('b')
    c = symScalMat('c')
    d = sqrt(a + b * a / c - c)
    assert d.string == "sqrt((a+((b*a)/c))-c)"

    e = symScalMat(1.2)
    assert e.string == '1.2'

def test_symMatrMat():
    """ Test symMatrMat (SymbolicMatrixMatlab) """
    print 'Testing symMatrMat...'
    a = symMatrMat("A")
    b = symMatrMat("B")
    c = symMatrMat("C")
    d = inv(a + b * (transpose(a) - c))
    assert d.string == "inv(A+(B*(A'-C)))"

    e = trace(transpose(a + b))
    assert e.string == "trace((A+B)')"

    e = logdet(a - b * c)
    assert e.string == "log(det(A-(B*C)))"

    e = fnorm(a + elementwiseProduct(b, c))
    assert e.string == "norm(A+(B.*C),'fro')"

    f = symScalMat("f")
    b = f * a + a / f
    assert b.string == "((f.*A)+(A./f))"

    b = a * f
    assert b.string == "(f.*A)"

    #check sizes
    x = symMatrMat("X", 2, 3)
    y = symMatrMat("B", 3, 5)
    z = symMatrMat("C", 2, 5)
    u = transpose(x * y + z)
    assert u.numRows == 5 and u.numCols == 2

    #test exception handling
    x = symMatrMat("A", 2, 3)
    logdet(x)


def main():
    """ Test Symbolic Matrices Python binding """
    test_symScalMat()
    test_symMatrMat()
    print 'Done.'

if __name__ == '__main__':
    main()

