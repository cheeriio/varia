import java.util.ArrayList;

public abstract class AbstractOffer {
    protected InterfaceAgent agentInvolved;

    private Typ type;

    public AbstractOffer(InterfaceAgent agent, Typ t){
        agentInvolved = agent;
        type = t;
    }

    public Typ getType() {
        return type;
    }
    public InterfaceAgent getAgent(){
        return agentInvolved;
    }
}
