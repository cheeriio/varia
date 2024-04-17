public class BuyOffer extends AbstractOffer{

    private int amount;
    public BuyOffer(InterfaceAgent agent, Typ t, int am){
        super(agent, t);
        amount = am;
    }
    public int getAmount(){
        return amount;
    }
}
