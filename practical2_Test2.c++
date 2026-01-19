#include<iostream>
#include<string>
using namespace std;

int main(){
    //initialization Phase 

    
    int numSymbols;
    cout << "Enter the number of symbols: ";
    cin >> numSymbols;

    char symbols[numSymbols];
    cout << "Enter the symbols: ";
    for(int i=0;i<numSymbols;i++){
        cin >> symbols[i];   
    }

    //enter a how many states are there
    int numStates;
    cout << "Enter the number of states: ";
    cin >> numStates;

    //enter a starting state
    int StartingState;
    cout << "Enter the starting state: ";
    cin >> StartingState;

    //enter a how many final states are there
    int FinalStates;
    cout << "Enter the number of final states: ";
    cin >> FinalStates;

     //final states array
    int finalStates[FinalStates];
    cout << "Enter the final states: ";
    for(int i=0;i<FinalStates;i++){
        cin >> finalStates[i];
    }


//creation of table
int transitionTable[numStates][numSymbols];
cout<<"enter a transition table:"<<endl;

for(int i=0;i<numStates;i++)
{
    for(int j=0;j<numSymbols;j++)
    {
        cout<<"State "<<(i+1)<<" symbol "<<symbols[j]<<"-> ";
        cin>>transitionTable[i][j];
    }
}
for(int i=0;i<numStates;i++)
{
    for(int j=0;j<numSymbols;j++)
    {
        cout<<"State "<<(i+1)<<" to "<<symbols[j]<<" -> "<<"state "<<transitionTable[i][j]<<endl;
    }
}

string inputString;
cout<<"enter input string:";
cin>>inputString;

int currentState = StartingState; 

for(char c: inputString)
{
    int symbolIndex=-1;
    for(int i=0;i<numSymbols;i++)
    {
        if(c==symbols[i])
        {
            symbolIndex=i;
            break;
        }
    }
    if(symbolIndex==-1)
    {
        cout<<"Invalid symbol input"<<endl;
        return 0;
    }
      currentState=transitionTable[currentState-1][symbolIndex];
      cout<<"After reading '"<<c<<"': currentState = "<<currentState<<endl;
}

bool Final=false;
for(int i=0;i<FinalStates;i++){
    if(currentState==finalStates[i]){
        Final=true;
        break;
    }
}

if(Final){
    cout<<"String accepted"<<endl;
}
else{
    cout<<"String not accepted"<<endl;
}
return 0;
}
