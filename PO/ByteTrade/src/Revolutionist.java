import java.util.ArrayList;

public class Revolutionist implements InterfaceLearnStrategy{
    private int cnt;

    public Revolutionist(){
        cnt = 0;
    }

    public void learn(Worker agent, InterfaceMarket market){
        cnt++;
        if(cnt % 7 == 0){
            cnt = 0;
            int n = Math.max(1, agent.getID() % 17);

            ArrayList<Integer>[] offered = market.getProductsOffered();
            int[] sums = {0, 0, 0, 0};
            for(int i = offered[0].size() - 1; i >= 0 && offered[0].size() - 1 - i < n; i--){
                for(int j = 0; j < 4; j++)
                    sums[j] += offered[j].get(i);
            }
            int max = sums[0];
            int index = 0;
            for(int i = 1; i < 4; i++){
                if(sums[i] > max){
                    max = sums[i];
                    index = i;
                }
            }
            switch(index){
                case 0:
                    agent.changeCareer(Career.CRAFTSMAN);
                case 1:
                    agent.changeCareer(Career.FARMER);
                case 2:
                    agent.changeCareer(Career.ENGINEER);
                case 3:
                    agent.changeCareer(Career.PROGRAMMER);
            }
        }
    }
}
