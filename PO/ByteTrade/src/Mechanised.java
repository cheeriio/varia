import java.util.ArrayList;

public class Mechanised implements InterfaceTradingStrategy{
    private int tools;

    public Mechanised(int t){ tools = t;}

    public void trade(Worker agent, InterfaceMarket market){
        BuyOffer off = new BuyOffer(agent, Typ.FOOD, 100);
        market.addWorkerBuyOffer(off);
        ArrayList<Clothes> clothes = agent.getClothes();
        int cnt = 100;
        for(Clothes c : clothes){
            if(c.getDurability() == 1)
                cnt--;
        }
        if(cnt > 0){
            BuyOffer off2 = new BuyOffer(agent, Typ.CLOTHES, cnt);
            market.addWorkerBuyOffer(off2);
        }
        BuyOffer off3 = new BuyOffer(agent, Typ.TOOL, tools);
        market.addWorkerBuyOffer(off3);
    }
}
