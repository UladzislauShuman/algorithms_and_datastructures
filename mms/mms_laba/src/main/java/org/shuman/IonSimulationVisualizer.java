package org.shuman;

import org.knowm.xchart.*;
import org.knowm.xchart.style.Styler;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class IonSimulationVisualizer {

    public static void main(String[] args) {
        try {
            // 1. Компилируем и запускаем IonSimulation
            ProcessBuilder compileProcess = new ProcessBuilder("javac", "IonSimulation.java");
            Process compile = compileProcess.start();
            compile.waitFor();

            ProcessBuilder runProcess = new ProcessBuilder("java", "IonSimulation");
            Process run = runProcess.start();
            run.waitFor();

            // 2. Читаем данные из файла
            List<Double> xData = new ArrayList<>();
            List<Double> yData = new ArrayList<>();

            try (BufferedReader reader = new BufferedReader(new FileReader("out.txt"))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    String[] parts = line.trim().split("\\s+");
                    if (parts.length == 2) {
                        xData.add(Double.parseDouble(parts[0]));
                        yData.add(Double.parseDouble(parts[1]));
                    }
                }
            }

            // 3. Создаем график
            XYChart chart = new XYChartBuilder()
                    .width(800)
                    .height(600)
                    .title("Ion Simulation Results")
                    .xAxisTitle("Depth (Å)")
                    .yAxisTitle("Normalized Stopped Ions")
                    .theme(Styler.ChartTheme.Matlab)
                    .build();

            chart.getStyler().setDefaultSeriesRenderStyle(XYSeries.XYSeriesRenderStyle.Scatter);
            chart.getStyler().setMarkerSize(5);

            chart.addSeries("Ion Distribution", xData, yData);

            // 4. Отображаем график
            new SwingWrapper<>(chart).displayChart();

        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }
}