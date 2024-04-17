import java.util.ArrayList;

public class Randomized implements InterfaceProductionStrategy{
    public Randomized(){}

    public void produce(Worker agent, InterfaceMarket market){
        double gen = Math.random();
        int index;
        if(gen < 0.2){
            index = 0;
        } else if(gen < 0.4){
            index = 1;
        } else if (gen < 0.6) {
            index = 2;
        } else if (gen < 0.8) {
            index = 3;
        } else{
            index = 4;
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
            case 4:
                amount = agent.productivity(Typ.DIAX, market);
                agent.increaseDiamonds(amount);
        }
        agent.setProduced(amount);
    }
}
