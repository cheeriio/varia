public class Speculator extends AbstractAgent{
    private InterfaceSpeculatorType specsStrat;

    public Speculator(int idnum, int ds, InterfaceSpeculatorType str){
        super(idnum, ds);
        specsStrat = str;
    }
}
