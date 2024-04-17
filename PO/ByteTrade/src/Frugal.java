public class Frugal implements InterfaceWorkerStrategy{
    private int diamondLimit;

    public Frugal(int limit){
        diamondLimit = limit;
    }

    public boolean decisionToWork(Worker agent, InterfaceMarket market){
        if(agent.getDiamonds() > diamondLimit)
            return true;
        return false;
    }
}
