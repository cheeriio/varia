public class Technophobe implements InterfaceTradingStrategy{
    public Technophobe(){}

    public void trade(Worker agent, InterfaceMarket market){
        BuyOffer off = new BuyOffer(agent, Typ.FOOD, 100);
        market.addWorkerBuyOffer(off);
    }
}
