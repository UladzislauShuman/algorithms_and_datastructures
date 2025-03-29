package org.shuman;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.Arrays;
import java.util.Comparator;
import java.util.PriorityQueue;

public class Main436 {
    public static void main(String[] args) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(System.out));

        int n = Integer.parseInt(reader.readLine());
        Task[] tasks = new Task[n];

        String[] line;
        for (int i = 0; i < n; ++i) {
            line = reader.readLine().split(" ");
            tasks[i] = new Task(
                    Integer.parseInt(line[0]),
                    Integer.parseInt(line[1])
            );
        }

        Arrays.sort(tasks, Comparator.comparingInt(t -> t.d));

        PriorityQueue<Integer> maxBinaryHeap = new PriorityQueue<>();
        long totalStress = 0;

        for (Task task : tasks) {
            maxBinaryHeap.offer(task.w);
            if (maxBinaryHeap.size() > task.d) {
                totalStress += maxBinaryHeap.poll();
            }
        }

        writer.write(Long.toString(totalStress));

        reader.close();
        writer.close();
    }

    static class Task {
        public int d;
        public int w;

        Task(int d, int w) {
            this.d = d;
            this.w = w;
        }
    }
}