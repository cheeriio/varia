public class Conservatist implements InterfaceLearnStrategy{
    public Conservatist(){}

    public void learn(Worker agent, InterfaceMarket market){
        agent.advanceCareer();
    }
}
