import java.util.*;

public abstract class AbstractAgent implements InterfaceAgent{
    private int id;

    private int diamonds;

    protected ArrayList<Clothes> clothes;
    protected ArrayList<Food> foods;
    protected ArrayList<Tool> tools;
    protected ArrayList<Program> programs;

    public AbstractAgent(int idnum, int ds) {
        id = idnum;
        diamonds = ds;
        clothes = new ArrayList<Clothes>();
        foods = new ArrayList<Food>();
        tools = new ArrayList<Tool>();
        programs = new ArrayList<Program>();
    }

    public void addProduct(InterfaceProduct prod){
        switch(prod.getType()){
            case FOOD:
                foods.add((Food) prod);
            case CLOTHES:
                clothes.add((Clothes) prod);
            case TOOL:
                tools.add((Tool) prod);
            case PROGRAM:
                programs.add((Program) prod);
        }
    }
    public void act(InterfaceMarket market){}

    public ArrayList<Clothes> getClothes(){
        return clothes;
    }
    public ArrayList<Food> getFoods(){
        return foods;
    }
    public ArrayList<Program> getPrograms(){
        return programs;
    }
    public ArrayList<Tool> getTools(){
        return tools;
    }

    public int getID(){
        return id;
    }

    public int getDiamonds() {
        return diamonds;
    }

    public void increaseDiamonds(int amount){
        diamonds += amount;
    }
}
