import java.util.ArrayList;

public abstract class AbstractMarket implements InterfaceMarket{
    protected int days;
    private int clothesPenalty;
    private double[] dayZeroPrices;
    private ArrayList<Integer>[] productsSold;
    private ArrayList<Integer>[] productsOffered;
    private ArrayList<Double>[] moneySpent;
    private ArrayList<Worker> workers;
    private ArrayList<Speculator> speculators;
    private ArrayList<BuyOffer> workBuyOffers;
    private ArrayList<BuyOffer> specBuyOffers;
    private ArrayList<SellOffer> workSellOffers;
    private ArrayList<SellOffer> specSellOffers;

    public AbstractMarket(double[] prices, int clothesPen){
        dayZeroPrices = prices;
        productsSold = new ArrayList[4];
        moneySpent = new ArrayList[4];
        productsOffered = new ArrayList[4];
        for(int i = 0; i < 4; i++){
            productsSold[i] = new ArrayList<Integer>();
            moneySpent[i] = new ArrayList<Double>();
            productsOffered[i] = new ArrayList<Integer>();
        }
        workers = new ArrayList<Worker>();
        speculators = new ArrayList<Speculator>();
        workBuyOffers = new ArrayList<>();
        workSellOffers = new ArrayList<>();
        specBuyOffers = new ArrayList<>();
        specSellOffers = new ArrayList<>();
        days = 0;
        clothesPenalty = clothesPen;
    }

    public void addWorker(Worker w){
        workers.add(w);
    }

    public void addSpeculator(Speculator s){
        speculators.add(s);
    }

    public int getDays(){
        return days;
    }

    public ArrayList<Integer>[] getProductsSold(){
        return productsSold;
    }

    public ArrayList<Double>[] getMoneySpent(){
        return moneySpent;
    }

    public ArrayList<Integer>[] getProductsOffered(){return productsOffered;}
    public void simulateDay(){}
    public ArrayList<Worker> getWorkers(){
        return workers;
    }
    public ArrayList<Speculator> getSpeculators(){
        return speculators;
    }
    public double[] getDayZeroPrices(){
        return dayZeroPrices;
    }

    public int clothesPenalty(){
        return clothesPenalty;
    }

    public void addWorkerSellOffer(SellOffer o){
        workSellOffers.add(o);
    }
    public void addSpecSellOffer(SellOffer o){
        specSellOffers.add(o);
    }
    public void addWorkerBuyOffer(BuyOffer o){
        workBuyOffers.add(o);
    }
    public void addSpecBuyOffer(BuyOffer o){
        specBuyOffers.add(o);
    }
}
