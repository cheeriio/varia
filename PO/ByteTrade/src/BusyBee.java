public class BusyBee implements InterfaceWorkerStrategy{
    public BusyBee(){}

    public boolean decisionToWork(Worker agent, InterfaceMarket market){
        return true;
    }
}
