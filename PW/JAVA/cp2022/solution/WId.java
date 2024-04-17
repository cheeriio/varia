package cp2022.solution;

import cp2022.base.WorkplaceId;

public class WId extends WorkplaceId {
    public int compareTo(WorkplaceId other){
        return Integer.compare(hashCode(), other.hashCode());
    }
}
