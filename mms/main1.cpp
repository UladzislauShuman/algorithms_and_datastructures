#include <iostream>
#include <fstream>
#include <cmath>
#include <random> // Для более качественной генерации случайных чисел

using namespace std;

const double pi = 3.1415926535897932384;

const double pmax = 1;    // maximum of  IMPACT PARAMETER
int elstop = 1; // electronic stopping option

double M2 = 28.08, Z2 = 14;  // TARGET
double M1 = 208.98, Z1 = 83;    const int se = 2;// Ion  Bismuth

const double E0 = 100000, H = 10000;   //================ Estart, xmax (eV &  Angstrem)
const double Ec = E0 / 1000; // Заменили Emin на Ec
const double Emin = E0/1000;

int NN = 1000;  // ==================== number of particles

const double gamma_ = 4 * M1 * M2 / ((M1 + M2) * (M1 + M2));

int Nx = 500; // число шагов в разбиении всего интервала по глубине
const double Np = 0.05;

const double A0 = 0.5292; // Bohr radius, Angstrem
const  double ee2 = 27.21*A0; //eV*Angstrem  - electron charge squared
const double a_u = (0.8854 * A0) / (pow(Z1, 0.23) + pow(Z2, 0.23)); // universal screening length

double c2, s2; // output from magic
double eps; double b; // input for magic

// Функция для вычисления угла рассеяния в системе центра масс (СЦМ)
// (Оставлена как есть, предполагается, что она работает правильно)
double magic()
{
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

    return 1;
}

const double delta1 = M2 * a_u * a_u / (M1 * 4 * Z1 * Z1 * Z2 * Z2 * ee2 * ee2);
const double au = 1.138, bu = 0.01321, c_const = 0.21226, d_const = 0.19593;

// Electronic stopping power (потеря энергии на электронное торможение)
double Se(double E)  // Electronic stopping
{
    double SS, X;
    X = E / 1000;
    // P, Bi, B -> Si 100 keV : VALUES eV / A ------------------------polynomial approximation
    if (se == 1)  SS = 3.87 + 0.43 * X - 0.00172 * X * X;
    if (se == 2)  SS = 6.36419 + 0.78722 * X - 0.00391 * X * X;
    if (se == 3)  SS = 3.30582 + 0.54765 * X - 0.00303 * X * X;
    if (se == 4)  SS = 2.63632 + 0.32612 * X - 0.00162 * X * X;
    return SS;
}

// Функция для вычисления сечения рассеяния (пример, нужно заменить на более физичную модель)
double sigma(double E) {
    double theta_min = pi / 36.0; // Минимальный угол рассеяния
    return pi * a_u * a_u * Z1 * Z1 * Z2 * Z2 * ee2 * ee2 / (E * E * tan(theta_min / 2.0) * tan(theta_min / 2.0));
}

#include <cmath>

void rotate_vector(double& vx, double& vy, double& vz, double theta, double alpha) {
    // Поворот вокруг оси z на угол alpha
    double cos_alpha = cos(alpha);
    double sin_alpha = sin(alpha);
    double vx_temp = vx * cos_alpha - vy * sin_alpha;
    double vy_temp = vx * sin_alpha + vy * cos_alpha;
    vx = vx_temp;
    vy = vy_temp;

    // Поворот вокруг оси y на угол theta
    double cos_theta = cos(theta);
    double sin_theta = sin(theta);
    double vx_rotated = vx * cos_theta + vz * sin_theta;
    double vz_rotated = -vx * sin_theta + vz * cos_theta;
    vx = vx_rotated;
    vz = vz_rotated;
}


int main(int argc, char** argv)
{
    ofstream ofs("D:\\out.txt");
    double sum1 = 0;
    int i, icount = 0;

    int n, nn, j, nj, ntype;

    double E, L, x_cur, y_cur, z_cur;  // current Energy, L_free
    double Tnucl, TC, cosTh, sinTh, val, T_el, alpha;

    double hx = H / float(Nx);  //Angstrem
    double wstop[1001];  // array to save the stopped ions
    for (i = 0; i < 1001; ++i) wstop[i] = 0;
    double CMU = M2 / M1;
    int istop;
    for (j = 0; j <= Nx; j++) wstop[j] = 0;

    double ksi;

    double S_el_E0 = Se(E0);
    cout << " Se(E0)=" << S_el_E0 << endl;
    eps = E0 * a_u * M2 / (Z1 * Z2 * ee2 * (M1 + M2));
    cout << "eps=" << eps << "  " << endl;
    b = pmax / a_u;
    cout << "pmax = " << pmax << endl;

    magic();
    double theta = acos(sqrt(c2));   // scattering angle in the center of mass system
    double sinthc = sqrt(s2);
    Tnucl = gamma_ * E0 * pow((sinthc / 2), 2);  // the energy transferred in an elastic collision
    cout << "pmax =" << pmax << "  E0=" << E0 << " eV  ,  Theta_c=" << theta << endl;
    double theta_lab = atan(M2 * sin(theta) / (M1 + M2 * cos(theta)));  // // scattering angle in the laboratory system

    cout << "theta_lab=" << theta_lab << endl;
    cout << "Se=" << Se(E0) << endl;

    double vx, vy, vz;  // direction of movement for the  ion; norm(v)=1 !
    double eps1 = 0.0000000000001;

    random_device rd; //  Engine obtain seed from hardware
    mt19937 gen(rd()); // Standard Mersenne Twister engine
    uniform_real_distribution<> distrib(0.0, 1.0); // Define range (0.0, 1.0)
    //srand(314159216);  Убрали старый srand

    int nb = 0;  // number of backscattered ions
    int ntrans = 0;  // the number of ions that passed through the target

    for (n = 1; n <= NN; n++)   //======================================== main loop
    {
        ++icount;

        E = E0;
        vx = 1; vy = 0;  vz = 0;
        x_cur = 0; y_cur = 0;  z_cur = 0;

        for (j = 1; (j <= 100000) && (E > Ec); j++)  //------------###################### collision loop  // while E>Ec;
        {
            // 1. Рассчитать длину свободного пробега L(E) на основе энергии
            double sigma_E = sigma(E); // Сечение рассеяния (нужна реализация sigma(E))
            L = -log(distrib(gen)) / (Np * sigma_E);  // Длина свободного пробега (используем distrib(gen))

            // 2. Обновить координаты
            x_cur += L * vx;
            y_cur += L * vy;
            z_cur += L * vz;

            // 3. Рассчитать параметры столкновения
            double ksi = distrib(gen); // случайное число от 0 до 1
            double p = pmax * sqrt(ksi);
            eps = E * a_u * M2 / (Z1 * Z2 * ee2 * (M1 + M2));
            b = p / a_u;
            magic();  // ВАЖНО: убедиться, что magic() корректно работает с новыми значениями eps и b

            // 4. Рассчитать потерю энергии при упругом столкновении
            double sinthc_m = sqrt(s2);
            double Tnucl = gamma_ * E * pow((sinthc_m / 2), 2);

            // 5. Рассчитать потерю энергии на электронное торможение
            double T_el = Se(E) * L;
            if (elstop == 0) T_el = 0;

            // 6. Проверить, не остановился ли ион
            if ((E - Tnucl - T_el) <= Ec) break;

            // 7. Вычислить угол рассеяния в ЛСК (theta_lab)
            double TC = Tnucl / E;
            double val = sqrt(1 - TC);
            cosTh = (1 - 0.5 * (1 + CMU) * TC) / val;
            if (cosTh > 1.0) cosTh = 1.0;
            if (cosTh < -1.0) cosTh = -1.0;
            sinTh = sqrt(1.0 - cosTh * cosTh);
            double theta_lab = acos(cosTh);

            // 8. Сгенерировать случайный азимутальный угол alpha
            alpha = 2 * pi * distrib(gen);

            // 9. Повернуть вектор направления движения (vx, vy, vz)
            rotate_vector(vx, vy, vz, theta_lab, alpha); // Теперь используем функцию поворота

            // 10. Обновить энергию
            E -= (Tnucl + T_el);

            // 11. Проверить на backscattering (правильно)
            if (x_cur <= 0 && vx < 0) { nb++; break; }  // Условие Backscattering

        } // End Collision Loop

        // После цикла столкновений:
        if ((x_cur > 0) && (x_cur <= H))
        {
            istop = floor(x_cur / hx) + 1;
            sum1 = sum1 + x_cur;
        }
        else {
            istop = 0;
            if (x_cur <= 0) nb = nb + 1; else { ++ntrans;  cout << " ion transmited  n=" << n << endl; }
        };
        wstop[istop] = wstop[istop] + 1;
        if (icount % 100 == 1) cout << "x stop=" << x_cur << " n particle=" << n << endl;

    } // End Main Loop

    cout << " back scattered=" << nb << " passed through the target=" << ntrans << endl;
    double sum = 0;
    for (j = 1; j < Nx; j++) { sum = sum + hx * wstop[j]; }
    sum1 = sum1 / float(NN - nb - ntrans);
    cout << "Rp=  " << sum1 << "  sum=" << sum << endl;

    for (j = 0; j <= Nx; j++)
    {
        ofs << j * hx << "  " << wstop[j] / sum << endl;  //------------write resulting profile
    }

    ofs.close();
    return 0;
}