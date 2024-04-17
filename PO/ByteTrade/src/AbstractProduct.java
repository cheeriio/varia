public abstract class AbstractProduct implements InterfaceProduct{
    private int level;

    private Typ typ;

    public AbstractProduct(Typ t){
        typ = t;
        level = 1;
    }
    public AbstractProduct(Typ t, int lvl){
        typ = t;
        level = lvl;
    }
    public void increaseLevel(){
        level += 1;
    }
    public int getLevel(){
        return level;
    }

    public Typ getType(){
        return typ;
    }
}
