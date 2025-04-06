
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;
const double pi=3.1415926535897932384;

const double pmax = 1;    // maximum of  IMPACT PARAMETER
int elstop = 1; // electronic stopping option
double M2 = 28.08, Z2 = 14;  // TARGET
double M1 = 74.92, Z1 = 33;  int se = 4;// Arsenic ION
//double 	M1 = 30.97, Z1 = 15;    const int se = 1; // Phosphorus ION
//double M1 = 208.98, Z1 = 83;    const int se = 2;// Ion  Bismuth
//double M1 = 11.009, Z1 = 5; const int se = 3;// Ion Boron
// 
 //double M1 = 74.92, M2 = 28.08, Z1 = 33, Z2 = 14;  int se = 2 ?;// Arsenic ION
// double M1 = 121.76, M2 = 28.08, Z1 = 51, Z2 = 14;  Antimony ION

const double E0=100000, H=10000;   //================ Estart, xmax (eV &  Angstrem)
const double Emin=E0/1000;

int NN=1000;  // ==================== number of particles

const double gamma=4*M1*M2/((M1+M2)*(M1+M2));

int Nx=500; // число шагов в разбиении всего интервала по глубине 

const double A0 = 0.5292; // Bohr radius, Angstrem
const  double ee2 = 27.21*A0; //eV*Angstrem  - electron charge squared 
const double a_u = (0.8854*A0)/(pow(Z1,0.23)+pow(Z2,0.23)); // universal screening length

double c2, s2; // output from magic
double eps; double b; // input for magic
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
const double delta1=M2*a_u*a_u/(M1*4*Z1*Z1*Z2*Z2*ee2*ee2);
const double au=1.138,  bu=0.01321,  c=0.21226,  d=0.19593;
	
double Se(double E)  // Electronic stopping
{
	double SS, X;
	X = E / 1000;
	// P, Bi, B -> Si 100 keV : VALUES eV / A ------------------------polynomial approximation	
	if (se==1)  SS = 3.87 + 0.43 * X - 0.00172 * X*X;
	if (se == 2)  SS = 6.36419 + 0.78722 * X - 0.00391 * X * X;
	if (se == 3)  SS = 3.30582+	0.54765 * X - 0.00303 * X * X;
	if (se == 4)  SS = 2.63632 + 0.32612 * X - 0.00162 * X * X;
	return SS;
}
//#####################################################################################
int main(int argc, char **argv ) 
{
	ofstream ofs("D:\\out.txt");
	double sum1 = 0;
	int i, icount=0;

int n, nn, j, nj, ntype;

	double E, L, x_cur, y_cur, z_cur;  // current Energy, L_free   
	double Tnucl, TC, cosTh, sinTh, val, T_el, alpha;
	//double ksix, ksiy;

	double hx=H/float(Nx);  //Angstrem
	double wstop[1001];  // array to save the stopped ions
	for (i = 0; i < 1001; ++i) wstop[i]=0;
	double CMU=M2/M1;
	int istop;
for (j=0; j<=Nx; j++) wstop[j]=0;

double ksi; 
//double Ec=E0*M2/(M1+M2);
double S_el_E0=Se(E0);
cout << " Se(E0)=" << S_el_E0 << endl;
eps=E0*a_u*M2/(Z1*Z2*ee2*(M1+M2));
cout <<"eps=" << eps << "  " << endl;
b=pmax/a_u;
cout<<"pmax = " << pmax << endl;

magic();
double theta=acos(sqrt(c2));   // scattering angle in the center of mass system
double sinthc=sqrt(s2);
Tnucl=gamma*E0*pow((sinthc/2),2);  // the energy transferred in an elastic collision
cout<<"pmax ="<<pmax<<"  E0="<<E0<<" eV  ,  Theta_c="<<theta<<endl;
double theta_lab = atan(M2 * sin(theta) / (M1 + M2 * cos(theta)));  // // scattering angle in the laboratory system

cout<<"theta_lab="<< theta_lab <<endl;
cout<<"Se="<<Se(E0)<<endl;

double vx, vy, vz;  // direction of movement for the  ion; norm(v)=1 !
double eps1 = 0.0000000000001;
srand(314159216);
int nb = 0;  // number of backscattered ions
int ntrans = 0;  // the number of ions that passed through the target
for (n=1; n<=NN; n++)   //======================================== main loop
{
	++icount;

	E=E0; 
	vx = 1; vy = 0;  vz=0;
	x_cur=0; y_cur=0;  z_cur=0;

	for (j=1; (j<=100000)&&(E>2*Emin);   j++)  //------------###################### collision loop  // while E>2*Emin; 
	{		
		ksi=float(rand())/float(RAND_MAX); 
		L=1.*ksi/(pi*0.05*pmax*pmax);  //   free movement length
		
		{
			x_cur = x_cur + L * vx;
			y_cur = y_cur + L * vy;
			z_cur = z_cur + L * vz;
		}

		ksi=float(rand())/RAND_MAX;
		double p=pmax*sqrt(ksi);  //  impact parameter
		eps=E*a_u*M2/(Z1*Z2*ee2*(M1+M2));  //energy in special units
		b=p/a_u;  //impact parameter  in special units
		magic();
		double sinthc_m=sqrt(s2);
		double sumsc=s2+c2;
		double thetac_m=asin(sinthc_m); // scattering angle in the center of mass system
		double Tnucl_m=gamma*E*pow((sinthc_m/2),2);
		Tnucl=Tnucl_m;  // nuclear energy loss in collision
		T_el = Se(E) * L;  // /electronic energy loss
		if (elstop == 0) 	 T_el = 0;  //---------------------------------------------- option yes or not el.stopping
		
		if ( (E-Tnucl-T_el) <=Emin ) break;  //the ion stopped, start new ion

		TC=Tnucl/E;  
		if (TC>1) cout<<"  ERROR TC>1"<<endl;    
		val= sqrt(1-TC); 
		if (val<=0) cout<<"  ERROR val<=0"<<endl;
		if (val<0.0000001 ) val=0.0000001;

		cosTh=(1-0.5*(1+CMU)*TC)/val;		// scattering angle in the laboratory system
		if ( cosTh>=1. ) cosTh=1;
		if ( cosTh<=-1. ) cosTh=-1;
		sinTh = sqrt(1. - cosTh * cosTh);

		vx = 1;
		vy = 1;
		vz = 1;
		
		if (x_cur <= 0) { 
			//cout << cosTh << endl;  
			break; }  //---------------------------------------back scattering
		
		E=E - Tnucl-T_el;		//------------------------------------------- variable  E  is going down here
	}

	if ((x_cur > 0) && (x_cur <= H))
	{
		istop = floor(x_cur / hx) + 1;
		sum1 = sum1 + x_cur;
	}
	else { istop=0;
	if (x_cur <= 0) nb = nb + 1; else { ++ntrans;  cout << " ion transmited  n=" << n << endl; }
	};
	wstop[istop]=wstop[istop]+1;
	if (icount%100==1) cout<<"x stop="<<x_cur<<" n particle="<<n<<endl;

}
cout<<" back scattered="<<nb<<" passed through the target=" << ntrans<<endl;
double sum=0;
for (j=1; j<Nx; j++) { sum=sum+hx*wstop[j]; }
sum1 = sum1 / float(NN-nb-ntrans);
cout << "Rp=  " << sum1 << "  sum=" << sum << endl;

for (j = 0; j <= Nx; j++)
{
	ofs << j * hx << "  " << wstop[j] / sum << endl;  //------------write resulting profile
}

}