package pl.edu.mimuw.matrix;

public abstract class AbstrMatrix{
    protected Shape dims;

    public AbstrMatrix(int rows, int cols){
        dims = Shape.matrix(rows, cols);
    }
}