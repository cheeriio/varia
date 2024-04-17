package pl.edu.mimuw.matrix;

public class Zero extends AbstrMatrix implements IDoubleMatrix{
    public Zero(int rows, int cols){
        super(rows, cols);
    }

    public Full fullOf(){
        Full temp = new Full(dims.rows, dims.columns);
        return temp;
    }
    public IDoubleMatrix times(IDoubleMatrix other){
        assert(dims.columns == other.shape().rows);
        Full ans = new Full(dims.rows , other.shape().columns);
        return ans;
    }

    public Zero times(double scalar){
        Zero temp = new Zero(dims.rows, dims.columns);
        return temp;
    }

    public IDoubleMatrix plus(IDoubleMatrix other){
        assert(dims.rows == other.shape().rows);
        assert(dims.columns == other.shape().columns);
        return fullOf().plus(other);
    }

    public IDoubleMatrix plus(double scalar){
        return fullOf().plus(scalar);
    }

    public IDoubleMatrix minus(IDoubleMatrix other){
        assert(dims.rows == other.shape().rows);
        assert(dims.columns == other.shape().columns);
        return fullOf().minus(other);
    }

    public IDoubleMatrix minus(double scalar){
        return fullOf().minus(scalar);
    }

    public double get(int row, int column){
        shape().assertInShape(row, column);
        return 0;
    }

    public double[][] data(){
        double[][] v = new double[dims.rows][dims.columns];
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++)
                v[i][j] = 0;
        }
        return v;
    }

    public double normOne(){
        return 0;
    }

    public double normInfinity(){
        return 0;
    }

    public double frobeniusNorm(){
        return 0;
    }

    public String toString(){
        String ans = "".concat(Integer.toString(dims.rows)).concat(" ").concat(Integer.toString(dims.columns)).concat("\n");
        for(int i = 0; i < dims.rows; i++){
            if(dims.columns > 2)
                ans = ans.concat("0 ... 0\n");
            if(dims.columns == 2)
                ans = ans.concat("0 0\n");
            if(dims.columns == 2)
                ans = ans.concat("0\n");
        }
        return ans;
    }

    public Shape shape(){
        return dims;
    }
}
