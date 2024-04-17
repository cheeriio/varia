package pl.edu.mimuw.matrix;

import java.util.ArrayList;

public class Sparse extends AbstrMatrix implements IDoubleMatrix{
    private class Pair{
        private int column;
        private double value;
        public Pair(int c, double v){
            column = c;
            value = v;
        }
        public void setColumn(int c){
            column = c;
        }
        public void setValue(double v){
            value = v;
        }
        public int getColumn(){
            return column;
        }
        public double getValue(){
            return value;
        }
    }
    private ArrayList<ArrayList<Pair>> val;


    public Sparse(int rows, int cols, MatrixCellValue... values){
        super(rows, cols);
        val = new ArrayList<ArrayList<Pair>>(rows);
        for(int i = 0; i < rows; i++){
            ArrayList<Pair> temp = new ArrayList<>();
            val.add(i, temp);
        }
        for(MatrixCellValue i : values){
            Pair x = new Pair(i.column, i.value);
            (val.get(i.row)).add(x);
        }
    }
    public Full fullOf(){
        Full ans = new Full(dims.rows, dims.columns);
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < val.get(i).size(); j++){
                Pair x = val.get(i).get(j);
                ans.setVal(i, x.getColumn(), x.getValue());
            }
        }
        return ans;
    }
    private Sparse copy(){
        int cnt = 0;
        for(int i = 0; i < dims.rows; i++){
            cnt += val.get(i).size();
        }
        MatrixCellValue[] vals = new MatrixCellValue[cnt];
        cnt = 0;
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < val.get(i).size(); j++){
                Pair x = val.get(i).get(j);
                MatrixCellValue y = new MatrixCellValue(i, x.getColumn(), x.getValue());
                vals[cnt] = y;
                cnt++;
            }
        }
        Sparse ans = new Sparse(dims.rows, dims.columns, vals);
        return ans;
    }

    public IDoubleMatrix times(IDoubleMatrix other){
        assert(dims.columns == other.shape().rows);
        if(other instanceof Identity){
            return this.copy();
        }
        Full temp = fullOf();
        return temp.times(other);
    }

    public IDoubleMatrix times(double scalar){
        int cnt = 0;
        for(int i = 0; i < dims.rows; i++){
            cnt += val.get(i).size();
        }
        MatrixCellValue[] vals = new MatrixCellValue[cnt];
        cnt = 0;
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < val.get(i).size(); j++){
                Pair x = val.get(i).get(j);
                MatrixCellValue y = new MatrixCellValue(i, x.getColumn(), x.getValue() * scalar);
                vals[cnt] = y;
                cnt++;
            }
        }
        Sparse ans = new Sparse(dims.rows, dims.columns, vals);
        return ans;
    }

    private ArrayList<ArrayList<Pair>> getSparseVal(){
        return val;
    }

    public IDoubleMatrix plus(IDoubleMatrix other){
        assert(dims.rows == other.shape().rows);
        assert(dims.columns == other.shape().columns);
        Full temp = fullOf();
        return temp.plus(other);
    }

    public IDoubleMatrix plus(double scalar){
        int cnt = 0;
        for(int i = 0; i < dims.rows; i++){
            cnt += val.get(i).size();
        }
        MatrixCellValue[] vals = new MatrixCellValue[cnt];
        cnt = 0;
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < val.get(i).size(); j++){
                Pair x = val.get(i).get(j);
                MatrixCellValue y = new MatrixCellValue(i, x.getColumn(), x.getValue() + scalar);
                vals[cnt] = y;
                cnt++;
            }
        }
        Sparse ans = new Sparse(dims.rows, dims.columns, vals);
        return ans;
    }

    public IDoubleMatrix minus(IDoubleMatrix other){
        assert(dims.rows == other.shape().rows);
        assert(dims.columns == other.shape().columns);
        Full temp = fullOf();
        return temp.minus(other);
    }

    public IDoubleMatrix minus(double scalar){
        return plus(-scalar);
    }

    public double get(int row, int column){
        shape().assertInShape(row, column);
        for(int i = 0; i < val.get(row).size(); i++){
            Pair x = val.get(row).get(i);
            if(x.getColumn() == column){
                return x.getValue();
            }
        }
        return 0;
    }

    public double[][] data(){
        double[][] ans = new double[dims.rows][dims.columns];
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < dims.columns; j++){
                ans[i][j] = 0;
            }
        }
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < val.get(i).size(); j++){
                Pair x = val.get(i).get(j);
                ans[i][x.getColumn()] = x.getValue();
            }
        }
        return ans;
    }

    public double normOne(){
        double ans = 0;
        for(int i = 0; i < val.get(0).size(); i++){
            ans += val.get(0).get(i).getValue();
        }
        double temp = 0;
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < val.get(i).size(); j++){
                temp += Math.abs(val.get(i).get(j).getValue());
            }
            if(temp > ans)
                ans = temp;
            temp = 0;
        }
        return ans;
    }

    public double normInfinity(){
        double[] arr = new double[dims.columns];
        for(int i = 0; i < dims.columns; i++)
            arr[i] = 0;
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < val.get(i).size(); j++){
                arr[val.get(i).get(j).getColumn()] += Math.abs(val.get(i).get(j).getValue());
            }
        }
        double ans = arr[0];
        for(int i = 1; i < dims.columns; i++){
            if(arr[i] > ans)
                ans = arr[i];
        }
        return ans;
    }

    public double frobeniusNorm(){
        double ans = 0;
        for(int i = 0; i < dims.rows; i++){
            for(int j = 0; j < val.get(i).size(); j++){
                double x = val.get(i).get(j).getValue();
                ans += x * x;
            }
        }
        return Math.sqrt(ans);
    }

    public String toString(){
        for(int i = 0; i < dims.rows; i++){
            val.get(i).sort((a, b) -> Integer.compare(a.getColumn(), b.getColumn()));
        }
        String ans = "";
        ans = ans.concat(Integer.toString(dims.rows)).concat(" ").concat(Integer.toString(dims.columns)).concat("\n");
        for(int i = 0; i < dims.rows; i++){
            if(val.get(i).toArray().length == 0){
                ans = ans.concat("0 ... 0\n");
                continue;
            }
            Pair x = val.get(i).get(0);
            if(x.getColumn() > 2)
                ans = ans.concat("0 ... 0 ").concat(String.valueOf(x.getValue())).concat(" ");
            else{
                if(x.getColumn() == 1)
                    ans = ans.concat("0 ");
                else if(x.getColumn() == 2)
                    ans = ans.concat("0 0 ");
                ans = ans.concat(String.valueOf(x.getValue())).concat(" ");
            }
            for(int j = 1; j < val.get(i).size(); j++){
                Pair cur = val.get(i).get(j);
                Pair prev = val.get(i).get(j-1);
                int zeros = cur.getColumn() - prev.getColumn();
                if(zeros > 2)
                    ans = ans.concat("0 ... 0 ");
                if(zeros == 2)
                    ans = ans.concat("0 0 ");
                if(zeros == 1)
                    ans = ans.concat("0 ");
                ans = ans.concat(String.valueOf(cur.getValue())).concat(" ");
            }
            int zeros = dims.columns - 1 - val.get(i).get(val.get(i).size() - 1).getColumn();
            if(zeros > 2)
                ans = ans.concat("0 ... 0");
            if(zeros == 2)
                ans = ans.concat("0 0");
            if(zeros == 1)
                ans = ans.concat("0");
            ans = ans.concat("\n");
        }
        return ans;
    }

    public Shape shape(){
        return dims;
    }
}