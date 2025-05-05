package mms;

import java.io.FileWriter;
import java.io.IOException;
import java.util.Random;

public class IonSimulation {
    // Константы
    private static final double pi = 3.1415926535897932384;
    private static final double pmax = 1;    // максимальный прицельный параметр [ангстрем]
    private static int elstop = 1;           // флаг электронного торможения (1 - включено)
    
    // Параметры мишени (кремний)
    private static final double M2 = 28.08, Z2 = 14;  // масса и заряд мишени [а.е.м. и атомный номер]
    
    // Параметры иона (висмут)
    private static final double M1 = 208.98, Z1 = 83; // масса и заряд иона [а.е.м. и атомный номер]
    private static final int se = 2;            // тип иона (для электронного торможения)
    
    private static final double E0 = 100000, H = 10000;   // Начальная энергия [эВ] и максимальная глубина [ангстрем]
    private static final double Ec = E0 / 1000;           // Энергия срезки [эВ]
    
    private static int NN = 1000;  // Количество моделируемых ионов
    
    private static final double gamma_ = 4 * M1 * M2 / ((M1 + M2) * (M1 + M2)); // Коэффициент для расчета переданной энергии
    
    private static int Nx = 500; // Число шагов по глубине для гистограммы
    private static final double Np = 0.05; // Параметр плотности мишени [атомов/ангстрем^3]
    
    // Физические константы
    private static final double A0 = 0.5292; // Боровский радиус [ангстрем]
    private static final double ee2 = 27.21 * A0; // Квадрат заряда электрона [эВ*ангстрем]
    private static final double a_u = (0.8854 * A0) / (Math.pow(Z1, 0.23) + Math.pow(Z2, 0.23)); // Длина экранирования [ангстрем]
    
    // Глобальные переменные для функции magic()
    private static double c2, s2; // Выходные переменные (cos^2 и sin^2 угла рассеяния)
    private static double eps;    // Безразмерный параметр энергии
    private static double b;      // Безразмерный прицельный параметр
    
    public static void main(String[] args) throws IOException {
        FileWriter ofs = new FileWriter("/home/vladlox/git/algorithms_and_datastructures/mms/out.txt"); // Файл для вывода результатов
        
        // Инициализация переменных
        double sum1 = 0; // Сумма глубин остановки для расчета среднего
        int icount = 0; // Счетчик
        
        double hx = H / Nx;  // Шаг по глубине для гистограммы [ангстрем]
        double[] wstop = new double[1001];  // Массив для хранения гистограммы остановившихся ионов
        
        double CMU = M2 / M1; // Отношение масс мишени и иона
        
        // Переменные для статистики
        int nb = 0;  // Количество обратно рассеянных ионов
        int ntrans = 0;  // Количество ионов, прошедших насквозь
        
        Random random = new Random(314159216);
        
        // Основной цикл по ионам
        for (int n = 1; n <= NN; n++) {
            ++icount;
            
            // Инициализация нового иона
            double E = E0; // Начальная энергия
            double[] v = {1, 0, 0}; // Начальное направление вдоль оси x
            double x_cur = 0, y_cur = 0, z_cur = 0; // Начальные координаты
            
            // Цикл столкновений для одного иона
            for (int j = 1; j <= 100000 && E > 2 * Ec; j++) {
                // 1. Вычисление длины свободного пробега
                double ksi = random.nextDouble(); 
                double L = ksi / (pi * Np * pmax * pmax); // Случайная длина по экспоненциальному закону
                
                // 2. Перемещение иона
                x_cur += L * v[0];
                y_cur += L * v[1];
                z_cur += L * v[2];
                
                // 3. Выбор прицельного параметра и расчет столкновения
                ksi = random.nextDouble();
                double p = pmax * Math.sqrt(ksi);  // Случайный прицельный параметр
                eps = E * a_u * M2 / (Z1 * Z2 * ee2 * (M1 + M2)); // Безразмерный параметр энергии
                b = p / a_u;  // Безразмерный прицельный параметр
                
                magic(); // Расчет угла рассеяния
                
                double sinthc_m = Math.sqrt(s2);
                double Tnucl_m = gamma_ * E * Math.pow((sinthc_m / 2), 2); // Энергия, переданная ядру
                double Tnucl = Tnucl_m; 
                double T_el = Se(E) * L;  // Потери на электронное торможение
                
                if (elstop == 0) T_el = 0; // Опция отключения электронного торможения
                
                // Проверка на остановку иона
                if ((E - Tnucl - T_el) <= Ec) break;
                
                // 4. Расчет нового направления движения
                double TC = Tnucl / E;  
                double val = Math.sqrt(1 - TC);
                double cosTh = (1 - 0.5 * (1 + CMU) * TC) / val; // Косинус угла рассеяния
                if (cosTh >= 1.) cosTh = 1;
                if (cosTh <= -1.) cosTh = -1;
                double sinTh = Math.sqrt(1. - cosTh * cosTh);
                
                changeRotate(v, cosTh, sinTh); // Обновление направления
                
                // Проверка на обратное рассеяние
                if (x_cur <= 0) break;
                
                // 5. Обновление энергии
                E -= (Tnucl + T_el);
            }
            
            // Запись результатов для текущего иона
            if ((x_cur > 0) && (x_cur <= H)) {
                int istop = (int) Math.floor(x_cur / hx) + 1;
                sum1 += x_cur;
                wstop[istop]++;
            } else {
                if (x_cur <= 0) nb++; // Учет обратно рассеянных ионов
                else ntrans++; // Учет прошедших насквозь ионов
            }
            
            if (icount % 100 == 1) System.out.println("x stop=" + x_cur + " n particle=" + n);
        }
        
        // Вывод результатов
        System.out.println("Обратно рассеянные ионы=" + nb + " Прошедшие насквозь=" + ntrans);
        
        // Нормировка гистограммы
        double sum = 0;
        for (int j = 1; j < Nx; j++) sum += hx * wstop[j];
        sum1 = sum1 / (NN - nb - ntrans);
        System.out.println("Rp= " + sum1 + " sum=" + sum);
        
        // Запись нормированной гистограммы в файл
        for (int j = 0; j <= Nx; j++) {
            ofs.write(j * hx + " " + wstop[j] / sum + "\n");
        }
        
        ofs.close();
    }
    
    // Функция вычисления угла рассеяния в системе центра масс
    private static void magic() {
        double r = b;
        double rr = -2.7 * Math.log(eps * b);
        if (rr >= b) {
            rr = -2.7 * Math.log(eps * rr);
            if (rr >= b) {
                r = rr;
            }
        }
        double v, v1;
        while (true) {
            double ex1 = 0.18175 * Math.exp(-3.1998 * r);
            double ex2 = 0.50986 * Math.exp(-0.94229 * r);
            double ex3 = 0.28022 * Math.exp(-0.4029 * r);
            double ex4 = 0.028171 * Math.exp(-0.20162 * r);
            v = (ex1 + ex2 + ex3 + ex4) / r;
            v1 = -(v + 3.1998 * ex1 + 0.94229 * ex2 + 0.4029 * ex3 + 0.20162 * ex4) / r;
            double fr = b * b / r + v * r / eps - r;
            double fr1 = -b * b / (r * r) + (v + v1 * r) / eps - 1.;
            double q = fr / fr1;
            r = r - q;
            if (Math.abs(q / r) <= 0.001) break;
        }
        
        double roc = -2. * (eps - v) / v1;
        double sqe = Math.sqrt(eps);
        double cc = (0.011615 + sqe) / (0.0071222 + sqe);
        double aa = 2. * eps * (1. + (0.99229 / sqe)) * Math.pow(b, cc);
        double ff = (Math.sqrt(aa * aa + 1.) - aa) * ((9.3066 + eps) / (14.813 + eps));
        double delta = (r - b) * aa * ff / (ff + 1.);
        double co = (b + delta + roc) / (r + roc);
        c2 = co * co;
        s2 = 1. - c2;
    }
    
    // Функция расчета электронного торможения
    private static double Se(double E) {
        double SS, X;
        X = E / 1000; // Энергия в кэВ
        // Полиномиальная аппроксимация для разных ионов
        if (se == 1) SS = 3.87 + 0.43 * X - 0.00172 * X * X;
        if (se == 2) SS = 6.36419 + 0.78722 * X - 0.00391 * X * X;
        if (se == 3) SS = 3.30582 + 0.54765 * X - 0.00303 * X * X;
        if (se == 4) SS = 2.63632 + 0.32612 * X - 0.00162 * X * X;
        return SS; // Возвращает потери энергии [эВ/ангстрем]
    }
    
    // Функция обновления направления движения иона после столкновения
    private static void changeRotate(double[] v, double cosTh, double sinTh) {
        double phi = generateAzimuthalAngle();
        
        double[] e2 = new double[3];
        calculateOrthogonalVectors(v, e2);
        
        double[] e3 = new double[3];
        calculateThirdOrthogonalVector(v, e2, e3);
        
        double[] v_new = new double[3];
        updateVelocity(v, cosTh, sinTh, e2, e3, v_new, phi);
        
        System.arraycopy(v_new, 0, v, 0, 3);
        normalizeVector(v);
    }
    
    private static double generateAzimuthalAngle() {
        return 2 * pi * Math.random();
    }
    
    private static void calculateOrthogonalVectors(double[] v, double[] e2) {
        if (Math.abs(v[2]) < 0.999) {
            double norm = Math.sqrt(v[0] * v[0] + v[1] * v[1]);
            if (norm > 0) {
                e2[0] = -v[1] / norm;
                e2[1] = v[0] / norm;
                e2[2] = 0;
            } else {
                e2[0] = 1;
                e2[1] = 0;
                e2[2] = 0;
            }
        } else {
            e2[0] = 1;
            e2[1] = 0;
            e2[2] = 0;
        }
    }
    
    private static void calculateThirdOrthogonalVector(double[] v, double[] e2, double[] e3) {
        e3[0] = v[1] * e2[2] - v[2] * e2[1];
        e3[1] = v[2] * e2[0] - v[0] * e2[2];
        e3[2] = v[0] * e2[1] - v[1] * e2[0];
    }
    
    private static void updateVelocity(double[] v, double cosTh, double sinTh, 
                                     double[] e2, double[] e3, double[] v_new, double phi) {
        double cos_phi = Math.cos(phi);
        double sin_phi = Math.sin(phi);
        
        v_new[0] = v[0] * cosTh + sinTh * (e2[0] * cos_phi + e3[0] * sin_phi);
        v_new[1] = v[1] * cosTh + sinTh * (e2[1] * cos_phi + e3[1] * sin_phi);
        v_new[2] = v[2] * cosTh + sinTh * (e2[2] * cos_phi + e3[2] * sin_phi);
    }
    
    private static void normalizeVector(double[] v) {
        double norm_new = Math.sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        if (norm_new > 0) {
            v[0] /= norm_new;
            v[1] /= norm_new;
            v[2] /= norm_new;
        } else {
            v[0] = 1;
            v[1] = 0;
            v[2] = 0;
        }
    }
}