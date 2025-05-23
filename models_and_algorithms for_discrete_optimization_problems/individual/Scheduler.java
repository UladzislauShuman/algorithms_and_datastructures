import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.PriorityQueue;
import java.util.Set;

public class Scheduler {

    static class Task {
        int index;
        long exec, load;

        Task(int index, long exec, long load) {
            this.index = index;
            this.exec = exec;
            this.load = load;
        }
    }

    static class State {
        int i1, i2;
        long serverTime, proc1Time, proc2Time;
        int prevProc; // 0 = none, 1 = proc1, 2 = proc2
        long cost;

        State(int i1, int i2, long serverTime, long proc1Time, long proc2Time, int prevProc, long cost) {
            this.i1 = i1;
            this.i2 = i2;
            this.serverTime = serverTime;
            this.proc1Time = proc1Time;
            this.proc2Time = proc2Time;
            this.prevProc = prevProc;
            this.cost = cost;
        }

        public StateKey key() {
            return new StateKey(i1, i2, prevProc);
        }
    }

    static class StateKey {
        int i1, i2, prevProc;

        StateKey(int i1, int i2, int prevProc) {
            this.i1 = i1;
            this.i2 = i2;
            this.prevProc = prevProc;
        }

        @Override
        public boolean equals(Object o) {
            if (!(o instanceof StateKey sk)) return false;
            return i1 == sk.i1 && i2 == sk.i2 && prevProc == sk.prevProc;
        }

        @Override
        public int hashCode() {
            return Objects.hash(i1, i2, prevProc);
        }
    }

    public static void main(String[] args) throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader("input.txt"));
        BufferedWriter writer = new BufferedWriter(new FileWriter("output.txt"));

        String[] first = reader.readLine().split(" ");
        int n1 = Integer.parseInt(first[0]);
        int n2 = Integer.parseInt(first[1]);

        Task[] tasks1 = new Task[n1];
        Task[] tasks2 = new Task[n2];

        for (int i = 0; i < n1; i++) {
            String[] line = reader.readLine().split(" ");
            tasks1[i] = new Task(i + 1, Long.parseLong(line[0]), Long.parseLong(line[1]));
        }
        for (int i = 0; i < n2; i++) {
            String[] line = reader.readLine().split(" ");
            tasks2[i] = new Task(i + 1, Long.parseLong(line[0]), Long.parseLong(line[1]));
        }

        PriorityQueue<StateEntry> pq = new PriorityQueue<>(Comparator.comparingLong(se -> se.f));
        Set<StateKey> visited = new HashSet<>();

        pq.add(new StateEntry(0, new State(0, 0, 0, 0, 0, 0, 0), new ArrayList<>()));

        while (!pq.isEmpty()) {
            StateEntry current = pq.poll();
            State state = current.state;

            if (visited.contains(state.key())) continue;
            visited.add(state.key());

            if (state.i1 == n1 && state.i2 == n2) {
                long makespan = Math.max(state.proc1Time, state.proc2Time);
                writer.write(makespan + "\n");
                for (var pair : current.path) {
                    writer.write(pair.processor + " " + pair.taskIndex + "\n");
                }
                writer.close();
                return;
            }

            // попытка загрузить задачу на процессор 1
            if (state.i1 < n1 && state.prevProc != 1) {
                Task t = tasks1[state.i1];
                long start = Math.max(state.serverTime, state.proc1Time);
                long newServer = start + t.load;
                long newProc1 = newServer + t.exec;
                long newCost = Math.max(newProc1, state.proc2Time);

                State next = new State(state.i1 + 1, state.i2, newServer, newProc1, state.proc2Time, 1, newCost);
                List<ScheduleStep> newPath = new ArrayList<>(current.path);
                newPath.add(new ScheduleStep(1, t.index));
                long h = heuristic(next.i1, next.i2, tasks1, tasks2);
                pq.add(new StateEntry(next.cost + h, next, newPath));
            }

            // попытка загрузить задачу на процессор 2
            if (state.i2 < n2 && state.prevProc != 2) {
                Task t = tasks2[state.i2];
                long start = Math.max(state.serverTime, state.proc2Time);
                long newServer = start + t.load;
                long newProc2 = newServer + t.exec;
                long newCost = Math.max(newProc2, state.proc1Time);

                State next = new State(state.i1, state.i2 + 1, newServer, state.proc1Time, newProc2, 2, newCost);
                List<ScheduleStep> newPath = new ArrayList<>(current.path);
                newPath.add(new ScheduleStep(2, t.index));
                long h = heuristic(next.i1, next.i2, tasks1, tasks2);
                pq.add(new StateEntry(next.cost + h, next, newPath));
            }
        }

        writer.write("-1\n"); // unreachable (должно быть невозможно)
        writer.close();
    }

    static class ScheduleStep {
        int processor, taskIndex;

        ScheduleStep(int processor, int taskIndex) {
            this.processor = processor;
            this.taskIndex = taskIndex;
        }
    }

    static class StateEntry {
        long f;
        State state;
        List<ScheduleStep> path;

        StateEntry(long f, State state, List<ScheduleStep> path) {
            this.f = f;
            this.state = state;
            this.path = path;
        }
    }

    static long heuristic(int i1, int i2, Task[] t1, Task[] t2) {
    long sum = 0;
    for (int i = i1; i < t1.length; i++) {
        sum += t1[i].load + t1[i].exec;
    }
    for (int i = i2; i < t2.length; i++) {
        sum += t2[i].load + t2[i].exec;
    }
    return sum / 2;
}

}
