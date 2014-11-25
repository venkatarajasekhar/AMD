" Test Symbolic Matrices Python binding. "

from setup import *

def test_symScal():
    """ Test symScal (SymbolicScalarMatlab) """
    print 'Testing symScal...'
    a = symScal('a')
    b = symScal('b')
    c = symScal('c')
    d = sqrt(a + b * a / c - c)
    assert d.string == "sqrt((a+((b*a)/c))-c)"

    e = symScal(1.2)
    assert e.string == '1.2'

    print 'a = ', a
    print 'b = ', b
    print 'c = ', c
    print 'd = ', d

def test_symMat():
    """ Test symMat (SymbolicMatrixMatlab) """
    print 'Testing symMat...'
    a = symMat("A")
    b = symMat("B")
    c = symMat("C")
    d = inv(a + b * (transpose(a) - c))
    assert d.string == "inv(A+(B*(A'-C)))"

    print 'A = ', a
    print 'B = ', b
    print 'C = ', c
    print 'D = ', d
    
    e = trace(transpose(a + b))
    assert e.string == "trace((A+B)')"

    e = logdet(a - b * c)
    assert e.string == "log(det(A-(B*C)))"

    e = fnorm(a + elementwiseProduct(b, c))
    assert e.string == "norm(A+(B.*C),'fro')"

    f = symScal("f")
    b = f * a + a / f
    assert b.string == "((f.*A)+(A./f))"

    b = a * f
    assert b.string == "(f.*A)"

    #check sizes
    x = symMat("X", 2, 3)
    y = symMat("B", 3, 5)
    z = symMat("C", 2, 5)
    u = transpose(x * y + z)
    assert u.numRows == 5 and u.numCols == 2

    

def main():
    """ Test Symbolic Matrices Python binding """
    test_symScal()
    test_symMat()
    print 'Done.'

if __name__ == '__main__':
    main()

