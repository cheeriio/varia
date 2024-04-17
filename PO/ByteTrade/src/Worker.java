public class Worker extends AbstractAgent{
    private static int careerIndex(Career t){
        switch(t){
            case FARMER:
                return 0;
            case CRAFTSMAN:
                return 1;
            case ENGINEER:
                return 2;
            case PROGRAMMER:
                return 3;
            case MINER:
                return 4;
        }
        return -1;
    }

    private static int typeIndex(Typ t){
        switch(t){
            case FOOD:
                return 0;
            case CLOTHES:
                return 1;
            case TOOL:
                return 2;
            case PROGRAM:
                return 3;
            case DIAX:
                return 4;
        }
        return -1;
    }

    boolean alive;

    private int produced;
    private int daysWithNoFood;
    private InterfaceProductionStrategy prodStrat;

    private InterfaceTradingStrategy tradeStrat;

    private InterfaceWorkerStrategy workStrat;

    private InterfaceLearnStrategy learnStrat;

    private int[] careerLevels;

    private Career zawod;

    public void act(InterfaceMarket market){
        if(alive == false)
            return;
        if(workStrat.decisionToWork(this, market)){
            prodStrat.produce(this, market);
            tradeStrat.trade(this, market);
        } else{
            learnStrat.learn(this, market);
        }
    }

    public Worker(int idnum, int ds, InterfaceProductionStrategy prod, InterfaceTradingStrategy trad,
                  InterfaceWorkerStrategy wrk, InterfaceLearnStrategy lrn, Career zaw){
        super(idnum, ds);
        prodStrat = prod;
        tradeStrat = trad;
        workStrat = wrk;
        learnStrat = lrn;
        careerLevels = new int[5];
        for(int i : careerLevels)
            i = 0;
        zawod = zaw;
        alive = true;
    }

    public void advanceCareer(){
        careerLevels[careerIndex(zawod)]++;
    }

    public void changeCareer(Career c){
        zawod = c;
    }

    public int productivity(Typ t, InterfaceMarket market){
        int ans = 100;
        if(careerIndex(zawod) == typeIndex(t)){
            int i = careerLevels[careerIndex(zawod)];
            if(i == 1)
                ans += 50;
            else if(i == 2)
                ans += 150;
            else if (i == 3)
                ans += 300;
            else if(i > 3)
                ans += 300 + 25*(i - 3);
        }

        if(daysWithNoFood == 1)
            ans -= 100;
        else if(daysWithNoFood == 2)
            ans -= 300;
        else if(daysWithNoFood > 2) {
            return -1000;
        }
        for(Tool tool : tools){
            ans += tool.getLevel();
        }
        if(clothes.size() < 100)
            ans -= market.clothesPenalty();

        return ans;
    }

    public boolean isAlive(){return alive;}

    public void setProduced(int p){
        produced = p;
    }
    public int getProduced(){
        return produced;
    }

}
