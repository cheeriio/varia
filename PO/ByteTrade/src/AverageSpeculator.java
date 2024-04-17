import java.util.ArrayList;

public class AverageSpeculator implements InterfaceSpeculatorType{
    int history;

    public AverageSpeculator(int h){
        history = h;
    }

    public void trade(Speculator speculator, InterfaceMarket market){
        ArrayList<Integer>[] sold = market.getProductsSold();
        ArrayList<Double>[] money = market.getMoneySpent();
        double[] sum_money = new double[4];
        for(double f: sum_money)
            f = 0.0;
        int[] sum_sold = new int[4];
        for(int i : sum_sold)
            i = 0;
        for(int i = sold[1].size() - 1; i >= 0 && sold[1].size() - 1 - i < history; i--){
            for(int j = 0; j < 4; j++){
                sum_money[j] += money[j].get(i);
                sum_sold[j] += sold[j].get(i);
            }
        }
        if(sold[1].size() - history < 0){
            ArrayList<Speculator> l = market.getSpeculators();
            double[] prices = market.getDayZeroPrices();
            for(int i = 0 ; i < 4; i++){
                sum_money[i] += l.size() * prices[i];
                sum_sold[i] += l.size();
            }
        }
        double[] avg = new double[4];
        for(int i = 0 ; i < 4; i++){
            avg[i] = sum_money[i] / sum_sold[i];
        }


    }
}
