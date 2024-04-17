public class Clothes extends AbstractProduct{
    private int durability;

    private int lvl;

    public Clothes(Typ t){
        super(t);
        durability = 1;
    }

    public Clothes(Typ t, int lvl){
        super(t, lvl);
        durability = lvl * lvl;
    }
    public int use(){
        durability--;
        if(durability == 0)
            return 1;
        return 0;
    }
    public int getDurability(){
        return durability;
    }
}
