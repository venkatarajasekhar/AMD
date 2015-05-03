" Test Symbolic Matrices Python binding. "

from setup import *

def test_SymScal():
    """ Test SymScal (SymbolicScalarMatlab) """
    print 'Testing SymScal...'
    a = SymScal('a')
    b = SymScal('b')
    c = SymScal('c')
    d = sqrt(a + b * a / c - c)
    assert d.string == "sqrt((a+((b*a)/c))-c)"

    e = SymScal(1.2)
    assert e.string == '1.2'

    print 'a = ', a
    print 'b = ', b
    print 'c = ', c
    print 'd = ', d

def test_SymMat():
    """ Test SymMat (SymbolicMatrixMatlab) """
    print 'Testing SymMat...'
    a = SymMat("A")
    b = SymMat("B")
    c = SymMat("C")
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

    e = fnorm(a + elementwise_product(b, c))
    assert e.string == "norm(A+(B.*C),'fro')"

    f = SymScal("f")
    b = f * a + a / f
    assert b.string == "((f.*A)+(A./f))"

    b = a * f
    assert b.string == "(f.*A)"

    #check sizes
    x = SymMat("X", 2, 3)
    y = SymMat("B", 3, 5)
    z = SymMat("C", 2, 5)
    u = transpose(x * y + z)
    assert u.num_rows == 5 and u.num_cols == 2

    

def main():
    """ Test Symbolic Matrices Python binding """
    test_SymScal()
    test_SymMat()
    print 'Done.'

if __name__ == '__main__':
    main()

