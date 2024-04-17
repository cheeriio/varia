public class Distrubutional implements InterfaceWorkerStrategy{
    public Distrubutional(){}

    public boolean decisionToWork(Worker agent, InterfaceMarket market){
        int day = market.getDays();
        double gen = Math.random();
        if(gen < 1 - 1/(day + 3))
            return true;
        return false;
    }
}
