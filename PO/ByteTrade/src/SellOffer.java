import java.util.ArrayList;

public class SellOffer extends AbstractOffer{
    private ArrayList<InterfaceProduct> merch;

    public SellOffer(InterfaceAgent agent, Typ t){
        super(agent, t);
        merch = new ArrayList<>();
    }

    public void addItem(InterfaceProduct item){
        merch.add(item);
    }
}
