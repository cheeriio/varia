package pl.edu.mimuw.matrix;

public class Vector extends AbstrMatrix implements IDoubleMatrix{
    double[] val;

    public Vector(double[] v){
        super(1, v.length);
        val = new double[v.length];
        for(int i = 0; i < v.length; i++)
            val[i] = v[i];
    }
    public Full fullOf(){
        Full temp = new Full(dims.columns, 1);
        for(int i = 0; i < dims.columns; i++)
            temp.setVal(i, 0, val[i]);
        return temp;
    }
    public IDoubleMatrix times(IDoubleMatrix other){
        assert(dims.columns == other.shape().rows);
        Full temp = fullOf();
        return temp.times(other);
    }

    public IDoubleMatrix times(double scalar){
        double[] v = new double[dims.columns];
        for(int i = 0; i < dims.columns; i++)
            v[i] = val[i] * scalar;
        Vector ans = new Vector(v);
        return ans;
    }

    public IDoubleMatrix plus(IDoubleMatrix other){
        assert(dims.rows == other.shape().rows);
        assert(dims.columns == other.shape().columns);
        double[] v = new double[dims.columns];
        for(int i = 0; i < dims.columns; i++)
            v[i] = val[i] + other.get(0, i);
        Vector temp = new Vector(v);
        return temp;
    }

    public IDoubleMatrix plus(double scalar){
        double[] v = new double[dims.columns];
        for(int i = 0; i < dims.columns; i++)
            v[i] = val[i] + scalar;
        Vector temp = new Vector(v);
        return temp;
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
        return val[column];
    }

    public double[][] data(){
        double[][] ans = new double[1][dims.columns];
        for(int i = 0; i < dims.columns; i++)
            ans[0][i] = val[i];
        return ans;
    }

    public double normInfinity(){
        double ans = Math.abs(val[0]);
        for(int i = 1; i < dims.columns; i++){
            if(Math.abs(val[i]) > ans)
                ans = Math.abs(val[i]);
        }
        return ans;
    }

    public double normOne(){
        double ans = 0;
        for(int i = 0; i < dims.columns; i++){
            ans += Math.abs(val[i]);
        }
        return ans;
    }

    public double frobeniusNorm(){
        double ans = 0;
        for(int i = 0; i < dims.columns; i++){
            ans += val[i] * val[i];
        }
        return Math.sqrt(ans);
    }

    public String toString(){
        String ans = "1 ";
        ans = ans.concat(Integer.toString(dims.columns)).concat("\n");
        for(int i = 0; i < dims.columns; i++){
            ans = ans.concat(String.valueOf(val[i])).concat("\n");
        }
        return ans;
    }

    public Shape shape(){
        return dims;
    }

}
