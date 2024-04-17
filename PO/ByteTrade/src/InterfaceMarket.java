import java.util.ArrayList;

public interface InterfaceMarket {
    public void simulateDay();
    public int getDays();
    public ArrayList<Integer>[] getProductsSold();
    public ArrayList<Double>[] getMoneySpent();
    public ArrayList<Integer>[] getProductsOffered();
    public ArrayList<Worker> getWorkers();
    public ArrayList<Speculator> getSpeculators();
    public double[] getDayZeroPrices();
    public int clothesPenalty();

    public void addWorkerSellOffer(SellOffer o);
    public void addSpecSellOffer(SellOffer o);
    public void addWorkerBuyOffer(BuyOffer o);
    public void addSpecBuyOffer(BuyOffer o);
}
