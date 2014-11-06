" Test Symbolic Matrices Python binding. "

from setup import *

def test_symScalMat():
    """ Test symScalMat (SymbolicScalarMatlab) """
    print 'Testing symScalMat...'
    a = AMD.symScalMat('a')
    b = AMD.symScalMat('b')
    c = AMD.symScalMat('c')
    d = AMD.sqrt(a + b * a / c - c)
    assert d.string == "sqrt((a+((b*a)/c))-c)"

    e = AMD.symScalMat(1.2)
    assert e.string == '1.2'

def test_symMatrMat():
    """ Test symMatrMat (SymbolicMatrixMatlab) """
    print 'Testing symMatrMat...'
    a = AMD.symMatrMat("A")
    b = AMD.symMatrMat("B")
    c = AMD.symMatrMat("C")
    d = AMD.inv(a + b * (AMD.transpose(a) - c))
    assert d.string == "inv(A+(B*(A'-C)))"

    e = AMD.trace(AMD.transpose(a + b))
    assert e.string == "trace((A+B)')"

    e = AMD.logdet(a - b * c)
    assert e.string == "log(det(A-(B*C)))"

    e = AMD.fnorm(a + AMD.elementwiseProduct(b, c))
    assert e.string == "norm(A+(B.*C),'fro')"

    f = AMD.symScalMat("f")
    b = f * a + a / f
    assert b.string == "((f.*A)+(A./f))"

    b = a * f
    assert b.string == "(f.*A)"

    #check sizes
    x = AMD.symMatrMat("X", 2, 3)
    y = AMD.symMatrMat("B", 3, 5)
    z = AMD.symMatrMat("C", 2, 5)
    u = AMD.transpose(x * y + z)
    assert u.numRows == 5 and u.numCols == 2

def main():
    """ Test Symbolic Matrices Python binding """
    test_symScalMat()
    test_symMatrMat()
    print 'Done.'

if __name__ == '__main__':
    main()

