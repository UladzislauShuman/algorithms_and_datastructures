#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <cstdlib>

using namespace std;

const double pi = 3.1415926535897932384;


// Объявление функций
double generate_azimuthal_angle(); // Генерирует случайный азимутальный угол
void calculate_orthogonal_vectors(double vx, double vy, double vz, double& e2x, double& e2y, double& e2z); // Вычисляет ортогональные векторы
void calculate_third_orthogonal_vector(double vx, double vy, double vz, double e2x, double e2y, double e2z,
                                       double& e3x, double& e3y, double& e3z); // Вычисляет третий ортогональный вектор
void update_velocity(double vx, double vy, double vz, double cosTh, double sinTh, 
                     double e2x, double e2y, double e2z, double e3x, double e3y, double e3z,
                     double& v_new_x, double& v_new_y, double& v_new_z, double phi); // Обновляет направление движения
void normalize_vector(double& vx, double& vy, double& vz); // Нормализует вектор направления

// Функция обновления направления движения иона после столкновения
// vx, vy, vz - текущее направление движения (единичный вектор)
// cosTh, sinTh - косинус и синус угла рассеяния
void change_rotate(double& vx, double& vy, double& vz, double cosTh, double sinTh) {
    double phi = generate_azimuthal_angle();
    
    double e2x, e2y, e2z;
    calculate_orthogonal_vectors(vx, vy, vz, e2x, e2y, e2z);

    double e3x, e3y, e3z;
    calculate_third_orthogonal_vector(vx, vy, vz, e2x, e2y, e2z, e3x, e3y, e3z);

    double v_new_x, v_new_y, v_new_z;
    update_velocity(vx, vy, vz, cosTh, sinTh, e2x, e2y, e2z, e3x, e3y, e3z, v_new_x, v_new_y, v_new_z, phi);

    vx = v_new_x;
    vy = v_new_y;
    vz = v_new_z;

    normalize_vector(vx, vy, vz);
}

double generate_azimuthal_angle() {
    return 2 * pi * (double)rand() / (double)RAND_MAX;
}

void calculate_orthogonal_vectors(double vx, double vy, double vz, double& e2x, double& e2y, double& e2z) {
    if (fabs(vz) < 0.999) {
        double norm = sqrt(vx * vx + vy * vy);
        if (norm > 0) {
            e2x = -vy / norm;
            e2y = vx / norm;
            e2z = 0;
        } else {
            e2x = 1;
            e2y = 0;
            e2z = 0;
        }
    } else {
        e2x = 1;
        e2y = 0;
        e2z = 0;
    }
}

void calculate_third_orthogonal_vector(double vx, double vy, double vz, double e2x, double e2y, double e2z,
                                       double& e3x, double& e3y, double& e3z) {
    e3x = vy * e2z - vz * e2y;
    e3y = vz * e2x - vx * e2z;
    e3z = vx * e2y - vy * e2x;
}

void update_velocity(double vx, double vy, double vz, double cosTh, double sinTh, 
                     double e2x, double e2y, double e2z, double e3x, double e3y, double e3z,
                     double& v_new_x, double& v_new_y, double& v_new_z, double phi) {
    double cos_phi = cos(phi);
    double sin_phi = sin(phi);

    v_new_x = vx * cosTh + sinTh * (e2x * cos_phi + e3x * sin_phi);
    v_new_y = vy * cosTh + sinTh * (e2y * cos_phi + e3y * sin_phi);
    v_new_z = vz * cosTh + sinTh * (e2z * cos_phi + e3z * sin_phi);
}

void normalize_vector(double& vx, double& vy, double& vz) {
    double norm_new = sqrt(vx * vx + vy * vy + vz * vz);
    if (norm_new > 0) {
        vx /= norm_new;
        vy /= norm_new;
        vz /= norm_new;
    } else {
        vx = 1;
        vy = 0;
        vz = 0;
    }
}


// Параметры моделирования
const double pmax = 1;    // максимальный прицельный параметр [ангстрем]
int elstop = 1;           // флаг электронного торможения (1 - включено)

// Параметры мишени (кремний)
double M2 = 28.08, Z2 = 14;  // масса и заряд мишени [а.е.м. и атомный номер]

// Параметры иона (висмут по варианту)
double M1 = 208.98, Z1 = 83; // масса и заряд иона [а.е.м. и атомный номер]
const int se = 2;            // тип иона (для электронного торможения)

const double E0 = 100000, H = 10000;   // Начальная энергия [эВ] и максимальная глубина [ангстрем]
const double Ec = E0 / 1000;           // Энергия срезки [эВ] (ниже которой ион считается остановившимся)

int NN = 1000;  // Количество моделируемых ионов

const double gamma_ = 4 * M1 * M2 / ((M1 + M2) * (M1 + M2)); // Коэффициент для расчета переданной энергии

int Nx = 500; // Число шагов по глубине для гистограммы
const double Np = 0.05; // Параметр плотности мишени [атомов/ангстрем^3] (для кремния)

// Физические константы
const double A0 = 0.5292; // Боровский радиус [ангстрем]
const double ee2 = 27.21*A0; // Квадрат заряда электрона [эВ*ангстрем]
const double a_u = (0.8854 * A0) / (pow(Z1, 0.23) + pow(Z2, 0.23)); // Длина экранирования [ангстрем]

// Глобальные переменные для функции magic()
double c2, s2; // Выходные переменные (cos^2 и sin^2 угла рассеяния)
double eps;    // Безразмерный параметр энергии
double b;      // Безразмерный прицельный параметр

// Дополнительные константы для электронного торможения
const double delta1 = M2 * a_u * a_u / (M1 * 4 * Z1 * Z1 * Z2 * Z2 * ee2 * ee2);
const double au = 1.138, bu = 0.01321, c_const = 0.21226, d_const = 0.19593;

// Функция вычисления угла рассеяния в системе центра масс
double magic() {
    double r, rr, ex1, ex2, ex3, ex4, v, v1, fr, fr1, q, roc, sqe, cc, aa, ff, delta, co;
	r=b; 
	rr=-2.7*log(eps*b); 
	if (rr<b) goto lab330; 
	rr=-2.7*log(eps*rr); 
	if(rr<b) goto lab330; 
	r=rr; 
lab330: ex1=0.18175*exp(-3.1998*r); 
	ex2=0.50986*exp(-0.94229*r); 
	ex3=0.28022*exp(-0.4029*r); 
	ex4=0.028171*exp(-0.20162*r); 
	v=(ex1+ex2+ex3+ex4)/r; 
	v1=-(v+3.1998*ex1+0.94229*ex2+0.4029*ex3+0.20162*ex4)/r; 
	fr=b*b/r+v*r/eps-r; 
	fr1=-b*b/(r*r)+(v+v1*r)/eps-1.; 
	q=fr/fr1; 
	r=r-q; 
	if(fabs(q/r)>0.001) goto lab330; 
	roc=-2.*(eps-v)/v1; 
	sqe=sqrt(eps); 
	cc=(0.011615+sqe)/(0.0071222+sqe); 
	aa=2.*eps*(1.+(0.99229/sqe))*pow(b,cc); 
	ff=(sqrt(aa*aa+1.)-aa)*((9.3066+eps)/(14.813+eps)); 
	delta=(r-b)*aa*ff/(ff+1.); 
	co=(b+delta+roc)/(r+roc); 
	c2=co*co; 
	s2=1.-c2; 
    // Возвращает угол через глобальные переменные c2 (cos^2) и s2 (sin^2)
    return 1;
}

// Функция расчета электронного торможения (потери энергии на электроны)
double Se(double E) {
    double SS, X;
    X = E / 1000; // Энергия в кэВ
    // Полиномиальная аппроксимация для разных ионов
    if (se == 1) SS = 3.87 + 0.43 * X - 0.00172 * X * X;
    if (se == 2) SS = 6.36419 + 0.78722 * X - 0.00391 * X * X;
    if (se == 3) SS = 3.30582 + 0.54765 * X - 0.00303 * X * X;
    if (se == 4) SS = 2.63632 + 0.32612 * X - 0.00162 * X * X;
    return SS; // Возвращает потери энергии [эВ/ангстрем]
}

int main(int argc, char** argv) {
    ofstream ofs("D:\\out.txt"); // Файл для вывода результатов
    
    // Инициализация переменных
    double sum1 = 0; // Сумма глубин остановки для расчета среднего
    int i, icount = 0; // Счетчики
    
    // Основные переменные для моделирования
    int n, nn, j, nj, ntype; // Счетчики циклов
    double E, L, x_cur, y_cur, z_cur;  // Текущие энергия, длина свободного пробега и координаты
    double Tnucl, TC, cosTh, sinTh, val, T_el, alpha; // Параметры столкновений
    
    double hx = H / float(Nx);  // Шаг по глубине для гистограммы [ангстрем]
    double wstop[1001];  // Массив для хранения гистограммы остановившихся ионов
    for (i = 0; i < 1001; ++i) wstop[i] = 0; // Инициализация массива
    
    double CMU = M2 / M1; // Отношение масс мишени и иона
    int istop; // Индекс для гистограммы
    
    double ksi; // Случайная величина для различных расчетов
    
    // Инициализация генератора случайных чисел
    srand(314159216);
    
    // Переменные для статистики
    int nb = 0;  // Количество обратно рассеянных ионов
    int ntrans = 0;  // Количество ионов, прошедших насквозь
    
    // Основной цикл по ионам
    for (n = 1; n <= NN; n++) {
        ++icount;
        
        // Инициализация нового иона
        E = E0; // Начальная энергия
        double vx = 1, vy = 0, vz = 0; // Начальное направление вдоль оси x
        x_cur = 0; y_cur = 0; z_cur = 0; // Начальные координаты
        
        // Цикл столкновений для одного иона
        for (j = 1; (j <= 100000) && (E > 2 * Ec); j++) {
            // 1. Вычисление длины свободного пробега
            ksi = float(rand())/float(RAND_MAX); 
            L = 1.*ksi/(pi*Np*pmax*pmax); // Случайная длина по экспоненциальному закону
            
            // 2. Перемещение иона
            x_cur += L * vx;
            y_cur += L * vy;
            z_cur += L * vz;
            
            // 3. Выбор прицельного параметра и расчет столкновения
            ksi = float(rand())/RAND_MAX;
            double p = pmax*sqrt(ksi);  // Случайный прицельный параметр
            eps = E*a_u*M2/(Z1*Z2*ee2*(M1+M2)); // Безразмерный параметр энергии
            b = p/a_u;  // Безразмерный прицельный параметр
            
            magic(); // Расчет угла рассеяния
            
            double sinthc_m = sqrt(s2);
            double thetac_m = asin(sinthc_m); // Угол рассеяния в СЦМ
            double Tnucl_m = gamma_*E*pow((sinthc_m/2),2); // Энергия, переданная ядру
            Tnucl = Tnucl_m; 
            T_el = Se(E) * L;  // Потери на электронное торможение
            
            if (elstop == 0) T_el = 0; // Опция отключения электронного торможения
            
            // Проверка на остановку иона
            if ((E-Tnucl-T_el) <= Ec) break;
            
            // 4. Расчет нового направления движения
            TC = Tnucl/E;  
            val = sqrt(1-TC);
            cosTh = (1-0.5*(1+CMU)*TC)/val; // Косинус угла рассеяния в лабораторной системе
            if (cosTh >= 1.) cosTh = 1;
            if (cosTh <= -1.) cosTh = -1;
            sinTh = sqrt(1. - cosTh * cosTh);
            
            change_rotate(vx, vy, vz, cosTh, sinTh); // Обновление направления
            
            // Проверка на обратное рассеяние
            if (x_cur <= 0) break;
            
            // 5. Обновление энергии
            E -= (Tnucl + T_el);
        }
        
        // Запись результатов для текущего иона
        if ((x_cur > 0) && (x_cur <= H)) {
            istop = floor(x_cur / hx) + 1;
            sum1 += x_cur;
            wstop[istop]++;
        } else {
            if (x_cur <= 0) nb++; // Учет обратно рассеянных ионов
            else ntrans++; // Учет прошедших насквозь ионов
        }
        
        if (icount%100 == 1) cout << "x stop=" << x_cur << " n particle=" << n << endl;
    }
    
    // Вывод результатов
    cout << "Обратно рассеянные ионы=" << nb << " Прошедшие насквозь=" << ntrans << endl;
    
    // Нормировка гистограммы
    double sum = 0;
    for (j = 1; j < Nx; j++) sum += hx * wstop[j];
    sum1 = sum1 / float(NN - nb - ntrans);
    cout << "Rp= " << sum1 << " sum=" << sum << endl;
    
    // Запись нормированной гистограммы в файл
    for (j = 0; j <= Nx; j++) {
        ofs << j * hx << " " << wstop[j] / sum << endl;
    }
    
    ofs.close();
    return 0;
}