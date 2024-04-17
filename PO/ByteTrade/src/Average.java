import java.util.ArrayList;

public class Average implements InterfaceProductionStrategy{
    private int days;

    public Average(int n){
        days = n;
    }

    public void produce(Worker agent, InterfaceMarket market){
        ArrayList<Integer>[] sold = market.getProductsSold();
        ArrayList<Double>[] money = market.getMoneySpent();
        double[] sum_money = new double[4];
        for(double f: sum_money)
            f = 0.0;
        int[] sum_sold = new int[4];
        for(int i : sum_sold)
            i = 0;
        for(int i = sold[1].size() - 1; i >= 0 && sold[1].size() - 1 - i < days; i--){
            for(int j = 0; j < 4; j++){
                sum_money[j] += money[j].get(i);
                sum_sold[j] += sold[j].get(i);
            }
        }
        if(sold[1].size() - days < 0){
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
        double max = avg[0];
        int index = 0;
        for (int i = 1; i < 4; i++) {
            if (max < avg[i]) {
                max = avg[i];
                index = i;
            }
        }
        int amount = 0;
        switch (index) {
            case 0:
                amount = agent.productivity(Typ.CLOTHES, market);
                SellOffer off = new SellOffer(agent, Typ.CLOTHES);
                ArrayList<Program> pr = agent.getPrograms();
                for (int i = 0; i < amount; i++) {
                    Program p = null;
                    if (pr.size() > 0)
                        p = pr.remove(0);
                    if (p != null) {
                        Clothes c = new Clothes(Typ.CLOTHES, p.getLevel());
                        off.addItem(c);
                    } else {
                        Clothes c = new Clothes(Typ.CLOTHES);
                        off.addItem(c);
                    }
                    p = null;
                }
                market.addWorkerSellOffer(off);
            case 1:
                amount = agent.productivity(Typ.FOOD, market);
                SellOffer off2 = new SellOffer(agent, Typ.FOOD);
                for (int i = 0; i < amount; i++) {
                    Food f = new Food(Typ.FOOD);
                    off2.addItem(f);
                }
                market.addWorkerSellOffer(off2);
            case 2:
                amount = agent.productivity(Typ.TOOL, market);
                SellOffer off3 = new SellOffer(agent, Typ.TOOL);
                ArrayList<Program> pr3 = agent.getPrograms();
                for (int i = 0; i < amount; i++) {
                    Program p = null;
                    if (pr3.size() > 0)
                        p = pr3.remove(0);
                    if (p != null) {
                        Tool c = new Tool(Typ.TOOL, p.getLevel());
                        off3.addItem(c);
                    } else {
                        Tool c = new Tool(Typ.TOOL);
                        off3.addItem(c);
                    }
                    p = null;
                }
                market.addWorkerSellOffer(off3);
            case 3:
                amount = agent.productivity(Typ.PROGRAM, market);
                SellOffer off4 = new SellOffer(agent, Typ.PROGRAM);
                for (int i = 0; i < amount; i++) {
                    Program prog = new Program(Typ.PROGRAM);
                    off4.addItem(prog);
                }
                market.addWorkerSellOffer(off4);
        }
        agent.setProduced(amount);
    }
}
