// чтобы увидеть сразу все графики (все варианты)

package org.shuman;

import org.knowm.xchart.*;
import org.knowm.xchart.style.Styler;
import org.knowm.xchart.XYSeries;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class IonVisualization {

    public static final int SLEEP_TIME = 100;

    public static void main(String[] args) {
        // Массив для хранения всех графиков
        List<XYChart> charts = new ArrayList<>();

        // Комбинации параметров
        double[] energies = {1000, 10000, 100000};
        int[] elstopValues = {0, 1};

        for (double E0 : energies) {
            for (int elstop : elstopValues) {
                try {
                    // 1. Запускаем симуляцию с текущими параметрами
                    double Rp = runSimulationAndGetRp(E0, elstop);

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
                            .width(600)
                            .height(400)
                            .title(String.format("E0=%.0f эВ, elstop=%d, Rp=%.2f Å", E0, elstop, Rp))
                            .xAxisTitle("Глубина (Å)")
                            .yAxisTitle("Нормированное количество остановленных ионов")
                            .theme(Styler.ChartTheme.Matlab)
                            .build();

                    chart.getStyler().setDefaultSeriesRenderStyle(XYSeries.XYSeriesRenderStyle.Line);
                    chart.getStyler().setMarkerSize(5);
                    chart.getStyler().setLegendVisible(false);

                    chart.addSeries("Распределение ионов", xData, yData)
                            .setXYSeriesRenderStyle(XYSeries.XYSeriesRenderStyle.Line);

                    // Добавляем аннотацию с Rp
                    chart.addAnnotation(new AnnotationText(String.format("Rp = %.2f Å (Средняя глубина проникновения)", Rp), 0.7, 0.9, true));


                    charts.add(chart);

                } catch (IOException | InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        // 4. Отображаем все графики
        new SwingWrapper<>(charts).displayChartMatrix();
    }

    private static double runSimulationAndGetRp(double E0, int elstop) throws IOException, InterruptedException {
        // Модифицируем параметры в исходном коде перед компиляцией
        modifySourceFile(E0, elstop);

        // Компилируем и запускаем
        ProcessBuilder compileProcess = new ProcessBuilder("javac", "IonSimulation.java");
        Process compile = compileProcess.start();
        compile.waitFor();

        // Запускаем симуляцию и перехватываем вывод
        ProcessBuilder runProcess = new ProcessBuilder("java", "IonSimulation");
        runProcess.redirectErrorStream(true); // Перенаправляем stderr в stdout
        Process run = runProcess.start();


        run.waitFor();
        Thread.sleep(SLEEP_TIME);
        // Читаем вывод процесса для получения Rp
        try (BufferedReader reader = new BufferedReader(new java.io.InputStreamReader(run.getInputStream()))) {
            String line;
            while ((line = reader.readLine()) != null) {
                System.out.println("Simulation output: " + line);
                if (line.contains("Rp=")) {
                    // Извлекаем значение Rp из строки вида "Rp= 777.9844128117453 sum=20000.0"
                    String[] parts = line.split("Rp=")[1].split("sum=");
                    return Double.parseDouble(parts[0].trim());
                }
            }
        }
        run.waitFor();
        return 0; // Возвращаем 0, если не удалось извлечь Rp
    }

    private static void modifySourceFile(double E0, int elstop) throws IOException {
        // Читаем исходный файл
        List<String> lines = new ArrayList<>();
        try (BufferedReader reader = new BufferedReader(new FileReader("IonSimulation.java"))) {
            String line;
            while ((line = reader.readLine()) != null) {
                lines.add(line);
            }
        }

        // Модифицируем параметры
        try (FileWriter writer = new FileWriter("IonSimulation.java")) {
            for (String line : lines) {
                if (line.contains("private static final double E0 =")) {
                    writer.write("    private static final double E0 = " + E0 + ", H = 10000;   // Начальная энергия [эВ] и максимальная глубина [ангстрем]\n");
                } else if (line.contains("private static int elstop =")) {
                    writer.write("    private static int elstop = " + elstop + ";           // флаг электронного торможения (1 - включено)\n");
                } else if (line.contains("private static final double pmax =")) {
                    // Устанавливаем pmax в зависимости от E0
                    double pmax = (E0 == 1000) ? 2 : (E0 == 10000) ? 1.5 : 1;
                    writer.write("    private static final double pmax = " + pmax + ";    // максимальный прицельный параметр [ангстрем]\n");
                } else {
                    writer.write(line + "\n");
                }
            }
        }
    }
}