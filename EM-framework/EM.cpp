#include "runEM.h"
using namespace std;
using namespace arma;
// to find energy-ES compare with RTH
// g++ -std=c++0x -w -fopenmp EM.cpp -O1 -o EM -larmadillo
// ./EM >EMoutput.txt

#ifndef typedef_emParam_t
#define typedef_emParam_t
typedef struct {
    vec mu;
    mat C;
    mat T;
    double sigma;
} emParam_t;
#endif //typedef_emParam_t

#ifndef typedef_emValue_t
#define typedef_emValue_t
typedef struct { 
    vec w;
    emParam_t em_t;
} emReturn_t;
#endif //typedef_emValue_t

#ifndef typedef_sample_t
#define typedef_sample_t
typedef struct {
    unsigned int id; // configuration id
    double performance;
    double power;
} sample_t;
#endif //typedef_sample_t

#ifndef typedef_application_t
#define typedef_application_t
typedef struct {    
    list<sample_t> s;
    int n; // number of samples known
    string name;
} application_t;
#endif //typedef_application_t

void init_EMParam(emParam_t *Old, int n);
void EM(emParam_t *Old, mat *W, mat *y_em, int i, int ep, int iteration_limit, emReturn_t *Appl);
double residualError(vec A, vec B);
long Time_Difference(struct timeval end, struct timeval start);
void loadData(int appId, mat *Dpower, mat *Dperformance, mat *truePower, mat *truePerformance,  mat *W);

void init_EMParam(emParam_t *Old, int n){
	Old->mu = zeros<vec>(n);
	Old->C = eye<mat>(n,n);
	Old->T = zeros<mat>(n,n);
	Old->sigma = 1;
}
void EM(emParam_t *Old, mat *W, mat *y_em, int i, int ep, int iteration_limit, emReturn_t *Appl){
	
	int n = y_em->n_rows;
	int m = y_em->n_cols;	
	int numSamples = sum(sum(*W));	
	double pi = 1;
	double tau = 1;
	double error = INFINITY;
	mat I = diagmat(*W);	
	double sigma = Old->sigma;
	vec mu = Old->mu;
	mat C = Old->C;
	mat wl = zeros<mat>(n,m);	
	
	for(int iterator = 1; iterator <= iteration_limit ; iterator++){		
		//struct timeval t1, t2,t3; long seconds; gettimeofday(&t1, NULL); ///////////////////////
		
		mat Cinv = inv(C);	
		mat Cl0 = inv(I/sigma + Cinv);
		mat Cll= inv(eye<mat>(n,n)/sigma + Cinv);			
		
		wl = (Cll/sigma)*(*y_em) + repmat(Cinv*mu,1,m);		
		wl.col(0) = Cl0*((y_em->col(0))/sigma +Cinv*mu);
		
		//gettimeofday(&t2, NULL); seconds = Time_Difference(t2,t1);
		//cout<<"Part1 milliseconds: "<<seconds<<endl;//////////////////
		
		double normSum = pow(norm(I*(y_em->col(0) -wl.col(0)),2),2) + trace(I* Cl0); 
		vec wlSum = sum(wl, 1);
		mat ClSum = eye<mat>(n,n) + Cl0 +(m-1)*Cll;		
		mat wlSumCov = eye<mat>(n,n) + (wl.col(0)-mu)*trans(wl.col(0)-mu);
		for(int l = 1; l < m; l++)		{
			wlSumCov = wlSumCov + (wl.col(l)-mu)*trans(wl.col(l)-mu);    
			normSum = normSum + pow(norm(y_em->col(l) - wl.col(l),2),2)+ trace( Cll);            
        }
		mu = ((1.0/(double)(pi + m))*wlSum);        
        C = ((1.0/(double)(tau + m))*(pi*(mu*trans(mu)) + tau*eye<mat>(n,n)+ ClSum + wlSumCov));        
        sigma = (1.0/(double)((m-1)*n + numSamples))*normSum;
        error = norm(Old->mu - mu,"fro")+ norm(Old->C - C,"fro") + abs(Old->sigma - sigma);        
        Old->mu = mu;    Old->C = C;    Old->sigma = sigma;  
    }
    
	Appl->w = wl.col(0);
	Appl->em_t = *Old;	
}		
long Time_Difference(struct timeval end, struct timeval start){
	long mtime, seconds, useconds; 
	seconds  = end.tv_sec  - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    return mtime;
}
double residualError(vec A, vec B){
	double n = A.n_elem;
	double rss = pow(norm(A-B, 2),2);     
    double tss = pow(norm(A-mean(A),2),2);
    double residualsquare = 1-rss/tss;
    return 1-(1-residualsquare)*(n-1)/(n-2);   
}


void loadDataFramework(mat *Dpower, mat *Dperf, mat *W, mat *WPerf, mat* WPower){
	int n;	

	Dpower->load("../Data/train/Power.txt");
	Dperf->load("../Data/train/Performance.txt");
	WPerf->load("../Data/test/TestPerformance.txt");	
	WPower->load("../Data/test/TestPower.txt");
	W->load("../Data/test/TestX.txt");

	//get the data of the targeted app to truePower and truePerformance
	//*truePower = Dpower->col(appId);
	//*truePerformance = Dperf->col(appId);
	
	//add the sample data of the targeted app to Dpower and Dperf
	Dpower->insert_cols(0,*WPower);
	Dperf->insert_cols(0,*WPerf);  
	
	//int cols=Dpower->cols();
	//move targeted app to the first collumn
	//Dpower->swap_cols(0,cols-1);    
	//Dperf->swap_cols(0,cols-1);    
}
//find the most energy-efficient configurations
void findEnergyConfigs( mat *Vconfig, vec Vpower, vec Vperf,  int n_config){
	int n, ind;
	double min;
	n = Vpower.n_elem;	// # CONFIGURATIONS	
	
	*Vconfig = zeros<mat>(n_config,2);
	vec Venergy(n);
	
	for(int i = 0; i < n; i++)
		Venergy(i) = Vpower(i)* Vperf(i);
	
	//Venergy.print();
	
	min=0, ind=0;
	//cout<< n <<"\t" << min<<"\t" << ind<<endl;
	
	//cout<< "computed energy"<<endl;
	for(int i = 0; i < n_config; i++){
		min = Venergy(0);
		//cout<< n <<"\t" << min<<"\t" << ind<<endl;
		if (i==0){
			//cout<< "I=0"<<endl;
			for(int j = 0; j < n; j++){		
				if (Venergy(j)<min){
					min = Venergy(j);
					ind = j;
				}
			}
			//cout<< ind <<"\t" << min<<endl;
			(*Vconfig)(i,0)= ind;
			(*Vconfig)(i,1)= min;
			//cout<< (*Vconfig)(i,0) <<"\t" << (*Vconfig)(i,1)<<endl;
		}
		else {
			//cout<< "I>0"<<endl;
			for(int j = 0; j < n; j++){		
				if ((Venergy(j)<min)&&(Venergy(j) > (*Vconfig)(i-1, 1))){
					min = Venergy(j);
					ind = j;
				}
			}
			//cout<< ind <<"\t" << min<<endl;
			(*Vconfig)(i,0)= ind;
			(*Vconfig)(i,1)= min;
			//cout<< (*Vconfig)(i,0) <<"\t" << (*Vconfig)(i,1)<<endl;
		}
	}
	//print mat Vconfig
	//(*Vconfig).print();
	//for(int i = 0; i < n_config; i++)
	//	cout<< (*Vconfig)(i,0) <<"\t" << (*Vconfig)(i,1)<<endl;
}

//find the most time-efficient configurations
void findPerfConfigs( mat *Pconfig, vec Vperf, int n_config){
	int n, ind;
	double max;
	n = Vperf.n_elem;	// # CONFIGURATIONS	
	
	*Pconfig = zeros<mat>(n_config,2);
	max=0, ind=0;
	
	//cout<< "computed energy"<<endl;
	for(int i = 0; i < n_config; i++){
		max = Vperf(0);
		//cout<< n <<"\t" << max<<"\t" << ind<<endl;
		if (i==0){
			//cout<< "I=0"<<endl;
			for(int j = 0; j < n; j++){		
				if (Vperf(j)>max){
					max = Vperf(j);
					ind = j;
				}
			}
			//cout<< ind <<"\t" << max<<endl;
			(*Pconfig)(i,0)= ind;
			(*Pconfig)(i,1)= max;
			//cout<< (*Vconfig)(i,0) <<"\t" << (*Vconfig)(i,1)<<endl;
		}
		else {
			//cout<< "I>0"<<endl;
			for(int j = 0; j < n; j++){		
				if ((Vperf(j)>max)&&(Vperf(j) < (*Pconfig)(i-1, 1))){
					max = Vperf(j);
					ind = j;
				}
			}
			(*Pconfig)(i,0)= ind;
			(*Pconfig)(i,1)= max;
		}
	}
	//for(int i = 0; i < n_config; i++)
	//	cout<< (*Pconfig)(i,0) <<"\t" << (*Pconfig)(i,1)<<endl;
}


//int main(int argc, char *argv[]){	
int main(){
	//string str(argv[1]);	
	int appId, n, m;
	double accuracy_power, accuracy_perf;	
	string strPow, strPerf;
	mat Dpower, Dperformance, y_em, Wpower, Wperf, W;
	vec pow, perf;
	application_t targetApp;	
	emParam_t Old_power, Old_perf;
    emReturn_t Appl_power, Appl_perf;   
	
	// LOAD DATA AND MISSING VALUES, appId is the index of application to do prediction
//	appId = stoi(str)-1;
//	cout<< "First run with Auto-Framework approach"<<endl;
    loadDataFramework(&Dpower, &Dperformance, &W, &Wpower, &Wperf);	
	
    n = Dpower.n_rows;	// # CONFIGURATIONS
    m = Dpower.n_cols;	// # APPLICATIONS	
	
	// START EM ALGORITHM FOR POWER	AND PERFORMANCE, prediceted data of targeted app is Appl_perf and Appl_power
	#pragma omp parallel sections		
	{
		#pragma omp section			
		{
			 init_EMParam(&Old_perf, n);      
			 EM (&Old_perf, &W , &Dperformance, 0, 50000, 100 , &Appl_perf);     
		} 
		#pragma omp section		
		{    
			 init_EMParam(&Old_power, n);      
			 EM (&Old_power, &W , &Dpower, 0, 50000, 100, &Appl_power);  
		}
	}
    
    // CALCULATE ERROR AND GET OUTPUT TO THE FILES     
    //accuracy_perf = residualError( Appl_perf.w, truePerformance.col(0));        
    //accuracy_power = residualError( Appl_power.w, truePower.col(0));  
	//cout<< "-------------------------------"<<endl;
	//cout<< accuracy_perf <<"\t" << accuracy_power<<endl;
	//Appl_perf.w.print();
	//Appl_perf.w.print();
	
	
	//find energy saving of best configurations
	mat Vconfig;
	mat Pconfig;
	
	// prediction of energy configurations and energy saving

    //cout << "Appl_perf: " << Appl_perf << endl;
    
   //cout << "Appl_perf: " << Appl_power << endl;
    
	findEnergyConfigs(&Vconfig, Appl_power.w, Appl_perf.w, 1);	
	//double true_energy = ((vec)truePower.col(0))(Vconfig(0,0)) * ((vec)truePerformance.col(0))(Vconfig(0,0));
	cout<<(Vconfig)(0,0)<<endl;
    //cout<< "Vconfig \t" << (Vconfig)(0,0)<< "Venergy \t" << (Vconfig)(0,1)<<endl;

}
