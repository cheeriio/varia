public class Periodic implements InterfaceWorkerStrategy{
    int okres;
    int cnt;

    public Periodic(int okr){
        okres = okr;
        cnt = 0;
    }

    public boolean decisionToWork(Worker agent, InterfaceMarket market){
        cnt++;
        if(cnt == okres){
            cnt = 0;
            return false;
        }
        return true;
    }
}
