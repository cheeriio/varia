package pl.edu.mimuw.matrix;

public class DoubleMatrixFactory {

  private DoubleMatrixFactory() {
  }

  public static IDoubleMatrix sparse(Shape shape, MatrixCellValue... values){
    assert(values != null);
    assert(shape != null);
    for(MatrixCellValue i : values){
      shape.assertInShape(i.row, i.column);
    }
    Sparse temp = new Sparse(shape.rows, shape.columns, values);
    return temp;
  }

  public static IDoubleMatrix full(double[][] values) {
    assert(values != null);
    assert(values.length > 0);
    assert(values[0].length > 0);
    int len = values[0].length;
    for(int i = 0; i < values.length; i++){
      assert(values[i].length == len);
    }
    Full temp = new Full(values.length, values[0].length);
    for(int i = 0; i < values.length; i++){
      for(int j = 0; j < values[0].length; j++)
        temp.setVal(i, j, values[i][j]);
    }
    return temp;
  }

  public static IDoubleMatrix identity(int size) {
    assert(size > 0);
    Identity temp = new Identity(size);
    return temp;
  }

  public static IDoubleMatrix diagonal(double... diagonalValues) {
    assert(diagonalValues != null);
    assert(diagonalValues.length > 0);
    Diagonal temp = new Diagonal(diagonalValues);
    return temp;
  }

  public static IDoubleMatrix antiDiagonal(double... antiDiagonalValues) {
    assert(antiDiagonalValues != null);
    assert(antiDiagonalValues.length > 0);
    AntiDiagonal temp = new AntiDiagonal(antiDiagonalValues);
    return temp;
  }

  public static IDoubleMatrix vector(double... values){
    assert(values != null);
    assert(values.length > 0);
    Vector temp = new Vector(values);
    return temp;
  }

  public static IDoubleMatrix zero(Shape shape) {
    assert(shape != null);
    assert(shape.columns > 0 && shape.rows > 0);
    Zero temp = new Zero(shape.rows, shape.columns);
    return temp;
  }
}
