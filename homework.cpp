#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;
#define MAXQ 100
#define MAXQA 100
#define MAXS 1000

struct literal{
	int notFlag;
	string predicate;		
	string arguments[MAXQA];
	int numArguments;
};
struct sentence{
	literal ** literals;
	int numLiterals;
};

//assumes sentences are in CNF form
//checks only for a match for the positive literal of left, if not found doesnt resolve the sentence 
// not symetric!
sentence * resolve(sentence * left, sentence * right);
int compareSentences(sentence ** sentences, sentence * newSentence ,int numSentences);


int main(){

literal queries[100];
sentence * sentences[1000];
int numQueries,numSentences;

ifstream stream1("input.txt");
ofstream stream2("output.txt");


stream1 >> numQueries;
string tmp;

getline(stream1,tmp);
int i=0;
//parse the queries
for(i=0;i<numQueries;i++){
	string tmp;
	stream1 >> tmp;
	int j=0;
	queries[i].notFlag = 0;
	if(tmp[0] == '~'){
		queries[i].notFlag = 1;
		j=1;
	}
	int k=0;
	char predicate[100];
	while(tmp[j] != '('){
		predicate[k] =tmp[j];
		j++;
		k++;
	}
	j++;
	predicate[k] = 0;
	queries[i].predicate.append(predicate);
	
	int argument =0;
	queries[i].numArguments =0;
	for(argument=0; tmp[j] != ')'; argument++){
		char constant[100];
		int c=0;
////cout << tmp[j];
   	  	while(tmp[j] !=',' &&tmp[j]!=')'){
			constant[c] = tmp[j];
			j++;
			c++;
		}
		if(tmp[j] == ',')
			j++;
		constant[c] = 0;
		

		queries[i].arguments[argument].append(constant);
		queries[i].numArguments ++;	
	}
		
}

//parse the sentences
stream1 >> numSentences;
getline(stream1,tmp);
for(i=0;i<numSentences;i++){
	string tmp;
	getline(stream1,tmp);
	
	sentences[i]= new sentence;
	int numLiterals = 1;
	int j=0;
	for(j=0;j<tmp.length();j++){
		if(tmp[j]=='|'){
			numLiterals ++;
		}
	}
	
	sentences[i]->literals= new  literal * [numLiterals];
	sentences[i]->numLiterals = numLiterals;
	for(j=0;j<numLiterals;j++){
		sentences[i]->literals[j] = new literal;

	}
	
	//counter for literals
	int k=0;
	//counter for arguments
	int t=0;
	sentences[i]->literals[k]->notFlag = 0; 

	for(j=0;j<tmp.length();j++){
		
		char tmp2[100];
		//counter for tmp2 
		int h = 0;
		if(tmp[j] == '~')
			sentences[i]->literals[k]->notFlag = 1; 
		else if((tmp[j] >= 'A' && tmp[j] <='Z') || (tmp[j] >= 'a' && tmp[j]<='z')){

			
			while(tmp[j] != '(' && tmp[j] != ',' && tmp[j] != ')'  ){
				tmp2[h] = tmp[j];
				h++;
				j++; 
			}
			tmp2[h]= 0;
			if(tmp[j] == '(')
				sentences[i]->literals[k]->predicate.append(tmp2);
			else if(tmp[j] == ',' || tmp[j] == ')'){
				sentences[i]->literals[k]->arguments[t].append(tmp2);
				t ++;
				if(tmp[j]==')'){
					sentences[i]->literals[k]->numArguments = t;
					t=0;
				}
			}

			
		}	
		if(tmp[j] == '|'){
			k++;
			sentences[i]->literals[k]->notFlag = 0; 
		}
		
			
	}
}

///////////////////////////////////////////////////////resolve queries
sentences[numSentences] = new sentence;
sentences[numSentences]->literals  = new  literal*;
sentences[numSentences]->numLiterals =1;
 

for(i =0; i < numQueries; i++){
//	cout << "hi" << endl;
	literal * negatedQuery = new literal;
	negatedQuery->predicate = queries[i].predicate;
	negatedQuery->notFlag = 0;
	if(queries[i].notFlag == 0)
		negatedQuery->notFlag = 1;

	negatedQuery->numArguments = queries[i].numArguments;
	int a;
	for(a =0;a<negatedQuery->numArguments;a++)
		negatedQuery->arguments[a]=queries[i].arguments[a];
	sentences[numSentences]->literals[0] = negatedQuery;
	int j=0;
	int newSentences = 1;
	int prevNewSentences = 0;
	//j and t are the parents
	int queryResult =0;
	

	while(newSentences > prevNewSentences){
	//cout << "prev "<<  prevNewSentences << " next " << newSentences<<endl;
	prevNewSentences =newSentences;

	for(j=0;j< numSentences + newSentences;j++){
		
		
		//cout <<"here " << sentences[j]->literals[0]->notFlag<< " "<<sentences[j]->literals[0]->predicate << "("<< sentences[j]->literals[0]->arguments[0] << ")"<<endl;
		//only looking for single ones with a positive literal
		if(sentences[j]->numLiterals > 1 )
			continue; 
		int t;
		for(t=0 ;t< numSentences+newSentences;t++){		
		
			sentence * newSentence = resolve(sentences[j],sentences[t]);
		//	//cout <<"there " << sentences[t]->literals[0]->predicate << "("<< sentences[t]->literals[0]->arguments[0] << ")"<<endl;

			if(newSentence == NULL){
			
				continue;
			}
			//if resolvent is empty TRUE, return, next query
			else if(newSentence->numLiterals ==0){
				queryResult =1;
				//cout << "this is the solution" << j<< " " << t<< endl;

				stream2 << "TRUE"<< endl;
				break;
			}
			else{1;
			/*	//cout << newSentence->numLiterals<<endl;
				int k=0;
				for(k=0;k<newSentence->numLiterals;k++){
					//cout << newSentence->literals[k]->notFlag<< " " <<newSentence->literals[k]->predicate << "(";
					int u;
					for(u=0; u<newSentence->literals[k]->numArguments; u++ ){
						//cout << newSentence->literals[k]->arguments[u]<<"," ;
					} 
					//cout << ") " << "| ";
				}
				//cout << endl;*/
			}
				
			//if not add the sentence to KB
			if(!compareSentences(sentences,newSentence, numSentences+newSentences)){

			
				sentences[numSentences+newSentences]= newSentence;
				newSentences++;
				
			}			
		if(queryResult == 1)
			break;	
		}	
			if(queryResult == 1)
			break;	
		}

}
	//cout << "____________" <<endl;
	if(queryResult ==0){
		stream2 << "FALSE" <<endl;
	}
	
	/*
	for(int j =0;j<numSentences+newSentences;j++){
	//cout << j << " ";
	
					int k;
				for(k=0;k<sentences[j]->numLiterals;k++){
					//cout << sentences[j]->literals[k]->notFlag<< " " <<sentences[j]->literals[k]->predicate << "(";
					int u;
					for(u=0; u<sentences[j]->literals[k]->numArguments; u++ ){
						//cout << sentences[j]->literals[k]->arguments[u]<<"," ;
					} 
					//cout << ") " << "| ";
				}

				//cout << endl;
				
			}
				
				
			
	*/
	
	//clean up the extra sentences
	int y=0;
	for(y=1;y<newSentences;y++){
		int z=0;
		for(z=0;z<sentences[numSentences+y]->numLiterals;z++)
			delete sentences[numSentences+y]->literals[z];
		delete sentences[numSentences+y];
		
	} 
	
	
}


//clean the memory
for(i=0;i<numSentences;i++){
	int j=0;
	for(j=0;j<sentences[i]->numLiterals;j++)
		delete sentences[i]->literals[j];	
	
}
for(i=0;i<numSentences;i++)
	delete sentences[i];
	
return 0;	
}

/////////////////////////////////////////////////
int compareSentences(sentence ** sentences, sentence * newSentence ,int numSentences){

	//check if not already in the database
				int y=0;
				int exists =0;
				for(y=0;y<numSentences;y++){
					int match =1;
					if(newSentence->numLiterals == sentences[y]->numLiterals){
						int z =0;
						//if all the literals are the same the sentences are the same
						for(z=0;z< newSentence->numLiterals;z++){
							if(newSentence->literals[z]->predicate == sentences[y]->literals[z]->predicate)
							if(newSentence->literals[z]->notFlag == sentences[y]->literals[z]->notFlag){

								int g=0;
								for(g=0;g<newSentence->literals[z]->numArguments;g++){
									if(newSentence->literals[z]->arguments[g] != sentences[y]->literals[z]->arguments[g])
										match =0;
								}
								if(match ==1){
									exists=1;
								}	
							}
							
						}
						
								
					}	
					if(exists)
						break;
				}
return exists;

}
///////////////////////////////////////////////////////////////////////////
//				RESOLVE
//////////////////////////////////////////////////////////

sentence * resolve(sentence * left, sentence * right){


int rightSize = right->numLiterals, leftSize = left->numLiterals;//left size shoulbe always 1 in here
int k=0;


sentence * newSentence= new sentence;
int found =0, matchIndex =0;
//mapping of constants and variables for unification
//		string mapLeft[2][MAXQA];

string mapRight0[MAXQA];
string mapRight1[MAXQA];
int i=0;
for(i=0;i<MAXQA;i++){
	mapRight0[i] = "";
	mapRight1[i] = "";

}

	//		int mapLCount =0, 
int mapRCount =0;
		


for(k=0;k<rightSize && !found ;k++){

	if(left->literals[leftSize-1]->predicate == right->literals[k]->predicate)
		if(left->literals[leftSize-1]->notFlag != right->literals[k]->notFlag){
	
			found =1;
			matchIndex =k;
			
			//cout << left->literals[leftSize-1]->predicate <<endl;
			//combining term unification
			int h;
			int numArguments = left->literals[leftSize-1]->numArguments;
			for(h=0;h<numArguments;h++){
				//if both are constants there is no unification

				if(left->literals[leftSize-1]->arguments[h][0] >='A' && left->literals[leftSize-1]->arguments[h][0]<='Z')				
				if(right->literals[k]->arguments[h][0] >= 'A' && right->literals[k]->arguments[h][0] <= 'Z'){
					if(left->literals[leftSize-1]->arguments[h] != right->literals[k]->arguments[h]){
					
						delete newSentence;
						return NULL;
					}
				
				}
								
				if(left->literals[leftSize-1]->arguments[h][0] >='A' && left->literals[leftSize-1]->arguments[h][0]<='Z')
				if(right->literals[k]->arguments[h][0] >= 'a' && right->literals[k]->arguments[h][0] <= 'z'){
	
					mapRight0[mapRCount] = right->literals[k]->arguments[h];
					mapRight1[mapRCount] = left->literals[leftSize-1]->arguments[h];

					mapRCount ++;
				}
							
			}
			
			
		}

				
}
if(!found){
	delete newSentence;
	return NULL;
	
}



newSentence->literals = new literal*[rightSize-1];
			int v=0;
			for(v=0;v<rightSize-1;v++)
				newSentence->literals[v] = new literal;		

newSentence->numLiterals = right->numLiterals -1;

//fill the new sentence
int offset =0;
for(i=0;i<rightSize-1;i++){
	if(i == matchIndex){
		offset =1;

	}

	newSentence->literals[i]->notFlag = right->literals[i+offset]->notFlag;
	newSentence->literals[i]->predicate = right->literals[i+offset]->predicate;
	newSentence->literals[i]->numArguments = right->literals[i+offset]->numArguments;
	int numArguments = right->literals[i+offset]->numArguments;
	int j =0;
	//cout << numArguments <<endl;
	for(j=0; j < numArguments; j++){
		string argument = right->literals[i+offset]->arguments[j];
		int v=0;
		for(v=0;v<mapRCount;v++){
			if(argument == mapRight0[v]){
				argument = mapRight1[v];
				break;
			}
		}

		newSentence->literals[i]->arguments[j] = argument;
		
		
	//	cout << newSentence->literals[i]->predicate<< " "<<	newSentence->literals[i]->arguments[j] << endl;
	
	}
		

						
}
return newSentence;
}
