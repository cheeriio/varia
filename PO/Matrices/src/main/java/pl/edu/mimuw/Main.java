package pl.edu.mimuw;

import pl.edu.mimuw.matrix.*;

import java.util.Random;

public class Main {

  public static void main(String[] args) {
    Random rand = new Random();
    //sparse
    MatrixCellValue[] temp = new MatrixCellValue[10];
    for(int i = 0; i < 10; i++){
      int row = rand.nextInt(10);
      int col = rand.nextInt(10);
      int val = rand.nextInt(50);
      temp[i] = new MatrixCellValue(row, col, val);
    }
    Sparse sp = new Sparse(10, 10, temp);
    System.out.println("Sparse:\n");
    System.out.print(sp.toString());

    //full
    double[][] data = new double[10][10];
    for(int i = 0; i < 10; i++){
      int row = rand.nextInt(10);
      int col = rand.nextInt(10);
      int val = rand.nextInt(50);
      data[row][col] = val;
    }
    Full f = new Full(10, 10);
    for(int i = 0; i < 10; i++){
      for(int j = 0; j < 10; j++)
        f.setVal(i, j, data[i][j]);
    }
    System.out.println("Full:\n");
    System.out.print(f.toString());

    //Diagonal
    double[] data2 = new double[10];
    for(int i = 0; i < 10; i++){
      int val = rand.nextInt(50);
      data2[i] = val;
    }
    Diagonal d = new Diagonal(data2);
    System.out.println("Diagonal:\n");
    System.out.print(d.toString());

    //AntiDiagonal
    double[] data3 = new double[10];
    for(int i = 0; i < 10; i++){
      int val = rand.nextInt(50);
      data3[i] = val;
    }
    AntiDiagonal adiag = new AntiDiagonal(data3);
    System.out.println("Antidiagonal:\n");
    System.out.print(adiag.toString());

    //Identity
    Identity id = new Identity(10);
    System.out.println("Identity:\n");
    System.out.print(id.toString());

    //Vector
    double[] data4 = new double[10];
    for(int i = 0; i < 10; i++){
      int val = rand.nextInt(50);
      data4[i] = val;
    }
    System.out.println("Vector:\n");
    Vector vec = new Vector(data4);
    System.out.print(vec.toString());

    //Zero
    Zero z = new Zero(10, 10);
    System.out.println("Zero:\n");
    System.out.print(z.toString());
  }
}
