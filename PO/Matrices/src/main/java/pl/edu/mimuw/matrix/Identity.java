package pl.edu.mimuw.matrix;

public class Identity extends AbstrMatrix implements IDoubleMatrix{
    public Identity(int len){
        super(len, len);
    }
    public Full fullOf(){
        Full ans = new Full(dims.rows, dims.rows);
        for(int i = 0; i < dims.rows; i++){
            ans.setVal(i, i, 1);
        }
        return ans;
    }
    public IDoubleMatrix times(IDoubleMatrix other){
        assert(dims.columns == other.shape().rows);
        return other;
    }

    public IDoubleMatrix times(double scalar){
        double[] values = new double[dims.rows];
        for(int i = 0; i < dims.rows; i++)
            values[i] = scalar;
        Diagonal ans = new Diagonal(values);
        return ans;
    }

    public IDoubleMatrix plus(IDoubleMatrix other){
        assert(dims.rows == other.shape().rows);
        assert(dims.columns == other.shape().columns);
        Full temp = fullOf();
        return temp.plus(other);
    }

    public IDoubleMatrix plus(double scalar){
        Full temp = fullOf();
        return temp.plus(scalar);
    }

    public IDoubleMatrix minus(IDoubleMatrix other){
        assert(dims.rows == other.shape().rows);
        assert(dims.columns == other.shape().columns);
        Full temp = fullOf();
        return temp.minus(other);
    }

    public IDoubleMatrix minus(double scalar){
        Full temp = fullOf();
        return temp.minus(scalar);
    }

    public double get(int row, int column){
        shape().assertInShape(row, column);
        if(row == column)
            return 1;
        return 0;
    }

    public double[][] data(){
        Full temp = fullOf();
        return temp.data();
    }

    public double normOne(){
        return 1;
    }

    public double normInfinity(){
        return 1;
    }

    public double frobeniusNorm(){
        return Math.sqrt(dims.rows);
    }

    public String toString(){
        String ans = "".concat(String.valueOf(dims.rows)).concat(" ").concat(String.valueOf(dims.rows)).concat("\n");
        for(int i = 0; i < dims.rows; i++){
            if(i == 1)
                ans = ans.concat("0 ");
            if(i == 2)
                ans = ans.concat("0 0 ");
            if(i > 2)
                ans = ans.concat("0 .. 0 ");
            ans = ans.concat("1 ");
            if(dims.rows - i - 1 == 1)
                ans = ans.concat("0 ");
            if(dims.rows - i - 1 == 2)
                ans = ans.concat("0 0 ");
            if(dims.rows - i - 1 > 2)
                ans = ans.concat("0 ... 0 ");
            ans = ans.concat("\n");
        }
        return ans;
    }

    public Shape shape(){
        return dims;
    }
}
