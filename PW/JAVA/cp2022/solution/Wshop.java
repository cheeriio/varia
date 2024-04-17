package cp2022.solution;

import cp2022.base.Workplace;
import cp2022.base.WorkplaceId;
import cp2022.base.Workshop;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Semaphore;

public class Wshop implements Workshop {
    private final HashMap<WorkplaceId, Workplace> places = new HashMap<>();
    private final HashMap<WorkplaceId, Semaphore> stationSemaphores = new HashMap<>();
    private final HashMap<Long, WorkplaceId> workerLocations = new HashMap<>();
    private final ConcurrentHashMap<Long, Boolean> workersWaiting = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<Long, Semaphore> workerSemaphores = new ConcurrentHashMap<>();
    private final int N;
    public Wshop(Collection<Workplace> plcs){
        N = plcs.size();
        for(Workplace w : plcs) {
            places.put(w.getId(), w);
            stationSemaphores.put(w.getId(), new Semaphore(1, true));
        }
    }
    public Workplace enter(WorkplaceId wid){
        Semaphore station = stationSemaphores.get(wid);
        Thread current = Thread.currentThread();

        workerSemaphores.put(current.getId(), new Semaphore(2*N, true));

        //  communicate that we are waiting
        workersWaiting.put(current.getId(), true);
        Semaphore worker = workerSemaphores.get(current.getId());
        if(worker.availablePermits() < 2 * N)
            worker.release(2 * N - worker.availablePermits());

        try{
            for(Long id : workersWaiting.keySet()){
                if(workersWaiting.get(id))
                    workerSemaphores.get(id).acquire();
            }
            station.acquire();

            // communicate that we ended waiting
            workersWaiting.put(current.getId(), false);
            if(worker.availablePermits() < 2 * N)
                worker.release(2 * N - worker.availablePermits());

            workerLocations.put(current.getId(), wid);
            return places.get(wid);
        } catch (InterruptedException e){
            throw new RuntimeException("panic: unexpected thread interruption");
        }
    }

    public Workplace switchTo(WorkplaceId wid){
        Thread current = Thread.currentThread();

        // free previous workplace
        WorkplaceId prevId = workerLocations.get(current.getId());
        Semaphore station = stationSemaphores.get(prevId);
        station.release();

        //  communicate that we are waiting
        workersWaiting.put(current.getId(), true);
        Semaphore worker = workerSemaphores.get(current.getId());
        if(worker.availablePermits() < 2 * N)
            worker.release(2 * N - worker.availablePermits());

        // get to the new workplace
        station = stationSemaphores.get(wid);
        try{
            station.acquire();

            // communicate that we ended waiting
            workersWaiting.put(current.getId(), false);
            if(worker.availablePermits() < 2 * N)
                worker.release(2 * N - worker.availablePermits());

            workerLocations.put(current.getId(), wid);
            return places.get(wid);
        } catch (InterruptedException e){
            throw new RuntimeException("panic: unexpected thread interruption");
        }
    }

    public void leave(){
        Thread current = Thread.currentThread();
        WorkplaceId prevId = workerLocations.get(current.getId());
        Semaphore station = stationSemaphores.get(prevId);
        station.release();
    }
}
