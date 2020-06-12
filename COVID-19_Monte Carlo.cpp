
#include <iostream>
#include <cmath>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//global parameters
int initial_healthy;// %
int initial_infect;
int infect_pro;// %
int tot_time;
int tot_infect;
int tot_dead;
int tot_icu;
int tot_recovery;

const int max_x = 1000;
const int max_y = 1000;

using namespace std;
class SparseMatrix;
class MatrixTerm{
    friend class SparseMatrix;
    
    public:
        int row, col;
        int status; 
        //1 to 7 
        int timer;
        int speed;

};


class SparseMatrix{
    public:
    	int rows, cols, terms;
    	MatrixTerm *smArray;
        // constructor 
        SparseMatrix(int y, int x, int num_ppl){
            rows = y;
            cols = x;
            terms = num_ppl;
            smArray = new MatrixTerm[num_ppl];
        }

        //functions plug in the information into matrix
        // Every calling in matrix.Insert will reflash the parameters in the function
        void Insert(int i, bool first){// t is total time
        	double rand_status = (double) rand() / (RAND_MAX + 1.0);
			rand_status = rand_status*100;
			//cout << " " <<rand_status<< " " << initial_healthy << " " << infect_pro << endl;
			if( (rand_status >= initial_healthy) && first){ //initial_healthy = 99
				//cout << " Run the first-" << endl;
				initial_infect++;
				smArray[i].status = 3;//3 is Sick in incubation period
				smArray[i].speed = 20;
				tot_infect++;
			}else if(rand_status <= infect_pro){ // infect_pro = 20, 20% without mask
				smArray[i].status = 2;// 2 is healthy without mask
				smArray[i].speed = 20;
			}else{
				smArray[i].status = 1;// 1 is heaithy with mask.
				smArray[i].speed = 20;
			}
			smArray[i].timer = 0;
				
		    double x = (double) rand() / (RAND_MAX + 1.0);
		    double y = (double) rand() / (RAND_MAX + 1.0);
			// generate double between [0, 1)
		    int xx = (int)((x* (max_x-1) ) + 0.5);
		    int yy = (int)((y* (max_y-1) ) + 0.5);
		    //possible to repeat
            smArray[i].row = yy;
            smArray[i].col = xx;
        }
		
		
		//check his status and generate random number and then move
		void Move(int i,int t){
			//speed decide the moving range
			int i_speed = smArray[i].speed;
			double x1 = (double) rand() / (RAND_MAX + 1.0);
			x1 = x1*i_speed;
    		double y1 = (double) rand() / (RAND_MAX + 1.0);
    		y1 = y1*i_speed;
    		double x2 = (double) rand() / (RAND_MAX + 1.0);
			x2 = x2*i_speed;
    		double y2 = (double) rand() / (RAND_MAX + 1.0);
    		y2 = y2*i_speed;
    		//cout << x1<<" "<<x2<<" " <<y1<<" "<<y2<<endl;
    		
    		//confine in the city(matrix)
			if(smArray[i].row <= (int)(abs(y1-y2) + 0.5) ){
				smArray[i].row = smArray[i].row + (int)(abs(y1-y2) + 0.5);
			}else if(smArray[i].row >= (max_y - (int)(abs(y1-y2) + 0.5)) ){
				smArray[i].row = smArray[i].row - (int)(abs(y1-y2) + 0.5);
			}else{
				smArray[i].row = smArray[i].row + (int)((y1-y2) + 0.5);
			}
			
			if(smArray[i].col <= (int)(abs(x1-x2) + 0.5)){
				smArray[i].col = smArray[i].col + (int)(abs(x1-x2) + 0.5);
			}else if(smArray[i].col >= (max_x - (int)(abs(x1-x2) + 0.5)) ){                          
				smArray[i].col = smArray[i].col - (int)(abs(x1-x2) + 0.5);
			}else{
				smArray[i].col = smArray[i].col + (int)((x1-x2) + 0.5);
			}
		}

        //change the status for sick people
        //Sick is the last one checking
        void Sick(int i, int t){
            if( (smArray[i].timer == 8 ) ){
            	smArray[i].status = 4;  // 4 is checking symptom 
            	smArray[i].speed = 10;
			}else if( (smArray[i].timer == 11 ) ){
				if(tot_icu != 0){
					tot_icu--;
					smArray[i].status = 6; // 6 is in ICU
					smArray[i].speed = 2;
					
				}else{
					smArray[i].status = 5; //5 is sick without ICU
					smArray[i].speed = 5;
				}
			}else if( (smArray[i].timer == 20 ) ){ // decide dead or not
				if(smArray[i].status == 6){
					tot_icu++;
				}
				double rand1 = (double) rand() / (RAND_MAX + 1.0);
				if(rand1 >= 0.95){ // 5 % people will die in infection.
					smArray[i].status = 7;// 7 is dead
					tot_dead ++;
				}else{
					Insert(i, false);
				}
			}
			
			// speed & timer ++ 
	        if(smArray[i].status==3 && (smArray[i].timer <= 7) ){ //Sick in incubation period
	        	//cout << "status 3 Timer +++ "<< endl;
	        	smArray[i].speed = 20;
	            smArray[i].timer++;                 
	        }else if(smArray[i].status == 4 ){ 
	        	//cout << "status 4 Timer +++ "<< endl;
            	smArray[i].speed = 10;
            	smArray[i].timer++;  
			}else if(smArray[i].status == 5 ){ //5 is sick without ICU
				//cout << "status 5 Timer +++ "<< endl;
				smArray[i].speed = 5;  
				smArray[i].timer++;  
			}else if(smArray[i].status == 6 ){ // 6 is in ICU
				//cout << "status 6 Timer +++ "<< endl;
				smArray[i].speed = 1;
				smArray[i].timer++;  
			}else if(smArray[i].status == 7 ){ // 7 is dead
				smArray[i].speed = 0;
			}else if(smArray[i].status==1 || smArray[i].status==2 ){
				
			}else{
				cout << "wrong in Sick function"<< endl;
			}
        }

        // Deal with HealthyRT: priorty 3
        bool Healthy(int i, int t){
        	//cout << "In the healthy function" << endl;
            bool num_sick_around;
            num_sick_around = Check(i, t); // all sick ppl can spread virus
            if(num_sick_around){
                if(smArray[i].status == 2){ // 1 is health without mask
                    smArray[i].timer = 0;
                    smArray[i].status = 3; // 3 is sick in incubation period
                    tot_infect++;
                    return true;
                    //cout << "Oh no I was infected ////"<< endl;
                    //smArray[].timer = num_sickppl_arround*7+14; 
                }else{
                	cout<<"wrong in Healthy"<<endl;
				}
            }else{
            	return false;
			}
        }
        
        // check how many sick ppls around 
        bool Check(int i, int t){

            for(int j=0;j<terms;j++){
                if( i!=j ){
                    if(((smArray[i].row-1==smArray[j].row)&&(smArray[i].col ==smArray[j].col)&&(smArray[i].row!=0))  ||\
					((smArray[i].row+1==smArray[j].row)&&(smArray[i].col ==smArray[j].col)&&(smArray[i].row!= max_y-1)) ||\
					((smArray[i].col-1==smArray[j].col)&&(smArray[i].row ==smArray[j].row)&&(smArray[i].col!=0)) ||\
					((smArray[i].col+1==smArray[j].col)&&(smArray[i].row ==smArray[j].row)&&(smArray[i].col!= max_x-1)) ||\
					((smArray[i].col==smArray[j].col)&&(smArray[i].row ==smArray[j].row)) ){
                        if(smArray[j].status== 7 || smArray[j].status== 1 || smArray[j].timer == 20 ){
                        	//sick function is the last one running. As a result, check smArray[i].timer == 20 
                        }else if( smArray[j].status== 3 && smArray[j].timer == 0 ){ 
							// j is the neighbor
							// If he is infected today, he will not spread virus 
                        }else{
                        	if (smArray[j].status== 6){
                        		double rand_h = (double) rand() / (RAND_MAX + 1.0);
                        		if(rand_h>=0.9){
                        			return true;
								}else{
								}
							}else{
							return true;
							}
                        	
						}

                    }
                }
            }

            return false;
        }

        // print current condition
        void Print(){
            for(int i=0;i<terms;i++){
                cout << "NO: "<< i << endl ;
                cout << "row: " << smArray[i].row << " ";
                cout << "col: " << smArray[i].col << " ";              
	        	cout << "speed: " << smArray[i].speed << " ";
                cout << "status: " << smArray[i].status << " ";
                cout << "timer: " << smArray[i].timer << endl;
                
            }
        };


};
//int initial_healthy;
//double infect_pro;
//int tot_time;
//int tot_infect;
//int tot_dead;
//int tot_icu;
//
//const int max_x = 1000;
//const int max_y = 1000;
//global parameters(in the front)

int main(){
	//global tot_time, infect_pro
    int y, x, cap, num_ppl;
    //int num_output_ppl=0;
    int status;
	
	cout << "It's in the 1000*1000 matrix" <<endl;
	//const max_x && max_y ==1000;
	
	//cout << "Assuming 95% people in the city were healthy"<<endl;
	initial_healthy = 99;
	cout << "Enter the Number of people in the city and bed capacity"<<endl;
    //cin >> num_ppl >> tot_icu ;
    num_ppl = 38614;
    //tot_icu = 200;
    cout << "Enter the total time in days (int) and infect probability(0, 1) "<<endl;
    //cin >> tot_time >> infect_pro; 
	tot_time = 100;
    infect_pro = 70;// % 
    SparseMatrix matrix(max_y, max_x, num_ppl);//declare matrix as SparseMatrix(friend class: Matrixterm)
    
	cout <<"infect_pro : "<< infect_pro << " %,  "<<"initial_healthy : " << initial_healthy << " %"<<endl;
	srand( time(NULL) );
	
	//for(int bbig = 1; bbig <=5; bbig++){
	//initial status;
		//cout<<"*********************        loops times:\t"<<bbig<<"*************************"<< endl<<endl;
		for(int kk = 1; kk <=1; kk++){
			tot_infect = 0;
		    tot_icu = 368;
		    tot_dead = 0;
		    initial_infect = 0;
			int i_status = 0;
			cout<<endl<< "Repeat_for:\t"<< kk << "\ttimes"<< endl;
			cout << "infect_pro:\t"<<infect_pro<<"\t%"<<endl<<endl;
			
			for(int i=0; i<num_ppl; i++){
				//cout << "Insert Done"<<endl;
			    matrix.Insert(i, true);
			}
			cout<<"Initial_infectppl:\t"<<initial_infect<< endl<<endl;
			
		    // main progress flow
		    for(int day=1; day<=tot_time;day++){
		    	int day_new_infect = 0;
				for(int i=0;i<num_ppl;i++){
					//double rand1 = (double) rand() / (RAND_MAX + 1.0);
					//cout << "NO : "<< i <<" : "<<matrix.smArray[i].status<<endl;
					i_status = matrix.smArray[i].status;
					if(i_status == 1){
						//cout << "Healthy with mask"<< endl;
					}else if(i_status == 2){
						if( matrix.Healthy(i, day)){
							day_new_infect++;
						}
					}else if(i_status == 7){
						//cout << "dead"<<endl;
					}else{
						matrix.Sick(i, day);
					}
			        
			        //check and move;
			        matrix.Move(i, day);
			    }
		    	//matrix.Print();
		    	cout <<" "<<day_new_infect<<"\t"<<tot_infect<<"\t"<<tot_icu<<"\t"<<tot_dead<< endl;
			    //cout << "day_new_infected : " << day_new_infect<<  endl;
			    //cout << "tot_infected : " << tot_infect << endl;
			    //cout << "tot_icu : " << tot_icu << endl;
			    //cout<<"tot_dead : "<<tot_dead <<endl;
			    //cout << "************** Day : "<<day<<" END *****************" << endl;
		
		        // matrix.Print();    
		        
		    }
		    //cout << "tot_infected : " << tot_infect << endl;
		    //infect_pro = infect_pro + 5;
		}
	//}
    return 0;
}

