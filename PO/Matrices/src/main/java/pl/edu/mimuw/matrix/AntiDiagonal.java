package pl.edu.mimuw.matrix;

public class AntiDiagonal extends AbstrMatrix implements IDoubleMatrix{
    double[] val;

    public AntiDiagonal(double[] v){
        super(v.length, v.length);
        val = new double[v.length];
        for(int i = 0; i < v.length; i++)
            val[i] = v[i];
    }

    public Full fullOf(){
        Full ans = new Full(dims.rows, dims.rows);
        for(int i = 0; i < dims.rows; i++){
            ans.setVal(i, dims.rows - i - 1, val[i]);
        }
        return ans;
    }
    public IDoubleMatrix times(IDoubleMatrix other){
        assert(dims.columns == other.shape().rows);
        if(other instanceof Identity){
            AntiDiagonal temp = new AntiDiagonal(val);
            return temp;
        }
        Full temp = fullOf();
        return temp.times(other);
    }

    public IDoubleMatrix times(double scalar){
        double[] v = new double[dims.rows];
        for(int i = 0; i < dims.rows; i++)
            v[i] = val[i] * scalar;
        AntiDiagonal temp = new AntiDiagonal(v);
        return temp;
    }

    public IDoubleMatrix plus(IDoubleMatrix other){
        assert(dims.rows == other.shape().rows);
        assert(dims.columns == other.shape().columns);
        if(other instanceof AntiDiagonal){
            double[] v = new double[dims.rows];
            for(int i = 0; i < dims.rows; i++)
                v[i] = val[i] + other.get(i, i);
            AntiDiagonal temp = new AntiDiagonal(v);
            return temp;
        }
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
        return plus(other.times(-1));
    }

    public IDoubleMatrix minus(double scalar){
        return plus(-scalar);
    }

    public double get(int row, int column){
        shape().assertInShape(row, column);
        if(row == column)
            return val[row];
        return 0;
    }

    public double[][] data(){
        double[][] arr = new double[dims.rows][dims.rows];
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < dims.rows; j++)
                arr[i][j] = 0;
        }
        for(int i = 0; i < dims.rows; i++)
            arr[i][dims.rows - 1 - i] = val[i];
        return arr;
    }

    public double normOne(){
        double ans = Math.abs(val[0]);
        for(int i = 1; i < dims.rows; i++){
            if(Math.abs(val[i]) > ans)
                ans = Math.abs(val[i]);
        }
        return ans;
    }

    public double normInfinity(){
        return normOne();
    }

    public double frobeniusNorm(){
        double ans = 0;
        for(int i = 0; i < dims.rows; i++)
            ans += val[i] * val[i];
        return Math.sqrt(ans);
    }

    public String toString(){
        String ans = "".concat(String.valueOf(dims.rows)).concat(" ").concat(String.valueOf(dims.rows)).concat("\n");
        for(int i = 0; i < dims.rows; i++){
            if(i == dims.rows - 2)
                ans = ans.concat("0 ");
            if(i == dims.rows - 3)
                ans = ans.concat("0 0 ");
            if(i < dims.rows - 3)
                ans = ans.concat("0 .. 0 ");
            ans = ans.concat(String.valueOf(val[i])).concat(" ");
            if(i == 1)
                ans = ans.concat("0 ");
            if(i == 2)
                ans = ans.concat("0 0 ");
            if(i > 2)
                ans = ans.concat("0 ... 0 ");
            ans = ans.concat("\n");
        }
        return ans;
    }

    public Shape shape(){
        return dims;
    }
}
