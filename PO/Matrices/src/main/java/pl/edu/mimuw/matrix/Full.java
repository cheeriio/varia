package pl.edu.mimuw.matrix;

public class Full extends AbstrMatrix implements IDoubleMatrix{
    private double[][] val;

    public Full(int rows, int cols){
        super(rows, cols);
        val = new double [rows][cols];
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++)
                setVal(i, j, 0);
        }
    }
    public void setVal(int row, int col, double v){
        dims.assertInShape(row, col);
        val[row][col] = v;
    }
    public Full times(double scalar) {
        Full ans = new Full(dims.rows, dims.columns);
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++)
                ans.setVal(i, j, val[i][j] * scalar);
        }
        return ans;
    }
    public Full times(IDoubleMatrix other){
        assert(dims.columns == other.shape().rows);
        Full ans = new Full(dims.rows , other.shape().columns);
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < other.shape().columns; j++){
                double s = 0;
                for(int k = 0; k < dims.columns; k++)
                    s += get(i, k) * other.get(k, j);
                ans.setVal(i, j, s);
            }
        }
        return ans;
    }

    public Full plus(IDoubleMatrix other){
        assert(dims.rows == other.shape().rows);
        assert(dims.columns == other.shape().columns);
        Full ans = new Full(dims.rows, dims.columns);
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++)
                ans.setVal(i, j, val[i][j] + other.get(i, j));
        }
        return ans;
    }

    public Full plus(double scalar){
        Full ans = new Full(dims.rows, dims.columns);
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++)
                ans.setVal(i, j, val[i][j] + scalar);
        }
        return ans;
    }

    public Full minus(IDoubleMatrix other){
        assert(dims.rows == other.shape().rows);
        assert(dims.columns == other.shape().columns);
        Full ans = new Full(dims.rows, dims.columns);
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++)
                ans.setVal(i, j, val[i][j] - other.get(i, j));
        }
        return ans;
    }

    public Full minus(double scalar){
        Full ans = new Full(dims.rows, dims.columns);
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++)
                ans.setVal(i, j, val[i][j] - scalar);
        }
        return ans;
    }

    public double get(int row, int column){
        shape().assertInShape(row, column);
        return val[row][column];
    }

    public double[][] data(){
        return val;
    }

    public double normInfinity(){
        double ans = 0;
        for(int j = 0; j < dims.columns; j++){
            ans += Math.abs(val[0][j]);
        }
        double temp = 0;
        for(int i = 1; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++){
                temp += Math.abs(val[i][j]);
            }
            if(temp > ans)
                ans = temp;
            temp = 0;
        }
        return ans;
    }

    public double normOne(){
        double ans = 0;
        for(int i = 0; i < dims.rows; i++){
            ans += Math.abs(val[i][0]);
        }
        double temp = 0;
        for(int j = 1; j < dims.columns; j++){
            for(int i = 0; i < dims.rows; i++){
                temp += Math.abs(val[i][j]);
            }
            if(temp > ans)
                ans = temp;
            temp = 0;
        }
        return ans;
    }

    public double frobeniusNorm(){
        double ans = 0;
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++){
                ans += val[i][j] * val[i][j];
            }
        }
        return Math.sqrt(ans);
    }

    public String toString(){
        String ans = "";
        for(int i = 1; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++){
                ans = ans.concat(String.valueOf(val[i][j]));
                ans = ans.concat(" ");
            }
            ans = ans.concat("\n");
        }
        return ans;
    }

    public Shape shape(){
        return dims;
    }
}