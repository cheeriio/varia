import java.util.ArrayList;

public class Student implements InterfaceWorkerStrategy{
    private int zapas;
    private int okres;

    public Student(int zap, int okr){
        zapas = zap;
        okres = okr;
    }

    public boolean decisionToWork(Worker agent, InterfaceMarket market){
        ArrayList<Integer>[] sold = market.getProductsSold();
        ArrayList<Double>[] money = market.getMoneySpent();
        double sum_money = 0;
        int sum_sold = 0;
        for(int i = sold[1].size() - 1; i >= 0 && sold[1].size() - 1 - i < okres; i--){
            sum_money += money[1].get(i);
            sum_sold += sold[1].get(i);
        }
        if(sold[1].size() - okres < 0){
            ArrayList<Speculator> l = market.getSpeculators();
            double[] prices = market.getDayZeroPrices();
            sum_money += l.size() * prices[1];
            sum_sold += l.size();
        }
        double avg_price = sum_money / sum_sold;
        if(agent.getDiamonds() >= 100 * zapas * avg_price)
            return false;
        return true;
    }
}
