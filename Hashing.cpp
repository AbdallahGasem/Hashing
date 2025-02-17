//                                                      بسم الله الرحمن الرحيم                                                 //
// program: Hashing.cpp
// Description:
// Author:  Abdallah Gasem
// Date: 01-12-2024
// Version: 1.0
/* file run command: g++ -o Hashing Hashing.cpp; if ($?) { ./Hashing } */
// ----------------------------------------------------------------------------------------------------------------------------- //

#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <string>
#include <random>   // for the HashMM Function
using namespace std;

/*______________________________________________________Hashing Techniques_______________________________________________________*/
// Division Method:
int HashDM(int key, int tableSize)
{
    int Hashvalue = key % tableSize;
    return Hashvalue;
}



int HashMM(int key, int tableSize)
{
    // Uncomment the following line to seed the random number generator with the current time for true randomness
    // srand(time(0)); /* add if you are gonna use the upper helpers */
    //__________________________________________________________________________________________________________________________________________________________________________________

    

    random_device rd;                   // 1. Create a random device to seed the random number generator
    mt19937 gen(rd());                  // 2. Create a Mersenne Twister random number generator and seed it with rd()

    std::uniform_real_distribution<float> distrib(0.0, 1.0);  // 2. Create a uniform real distribution in the range [min, max]

    float A = distrib(gen);  // 3. Generate a random float number using the generator and the distribution
    
    int hashValue = floor(tableSize * fmod(key * A , 1));
    return hashValue;
}

// Folding Method:
// FS MODE
enum Ftype {
    SHIFT, BOUND
};

// helper
vector<int> seprateByr(string key ,int r, Ftype ftype = Ftype :: SHIFT){
    vector<int>slicedK;
    string subString;

    // BOUND hash logic
    if (ftype == Ftype::BOUND)
    {   
        for (int i = 0; i < key.length(); i += r)
        {
            subString = key.substr(i,r);
            if (i == 0 || i == key.size() - r)  // key.size() - r to handle  the last substring properly! as i will  = key.size() -1 only if the r = 1
            {
                reverse(subString.begin(), subString.end());
            }
            
            slicedK.push_back(stoi(subString));
        }

        return slicedK;

    }

    // SHIFT hash logic
    for (int i = 0; i < key.length(); i += r)
    {
        slicedK.push_back(stoi(key.substr(i,r)));
    }
    

    return slicedK;
    

}

int HashFS(string key , int tableSize , Ftype ftype = Ftype::SHIFT){
    int hashValue = INT_MIN;
    int sum = 0;
    
    // input grooming
    int r = to_string(tableSize - 1).length();
    vector<int>slicedK = seprateByr(key,r,ftype);
        
    for (int i = 0; i < slicedK.size(); i++)
    {
        sum += slicedK[i];
    }
    
    hashValue = sum % tableSize;
    

    return hashValue;
}


// Mid square Method
int HashMS(int key , int tableSize){
    int hashValue;
    
    // r:
    int midnumberDCount = to_string(tableSize - 1).length();
    cout << "r: " << midnumberDCount << endl;

    //K^2:
    int keysq = key * key;
    cout << "keysq: " << keysq << endl;

    // Convert key to string to extract middle digits
    string keyStr = to_string(keysq);
    int start = (keyStr.length() - midnumberDCount) / 2;
    cout << "start: " << start << endl;

    string midDigits = keyStr.substr(start, midnumberDCount);
    cout << "midDigits: " << midDigits << endl;

    // Convert the middle digits back to an integer
    hashValue = stoi(midDigits) % tableSize;

    return hashValue;
}

/*________________________________________________Collision Resolution Techniques_______________________________________________*/

//                                                         chanining                                                            //
//-------------------------------------------------------------------------------------------------------------------------------
// insert a key, element
enum hashType{
    DM, MM, FS, MS
};

vector<list<pair<int,int>>> chainValues;

// return the index of the element
int insert(int key, int element,int tableSize ,hashType ht = DM) {
    int hashValue , ftchoice = 1;

    switch (ht)
    {

    case DM:
        hashValue = HashDM(key,tableSize);
        break;

    case MM:
        hashValue = HashMM(key,tableSize);
        break;

    case FS:
        cout << "enter your Folding method Type 1) SHIFT or 2) BOUND:\n";
        cin >> ftchoice;
        if (ftchoice == 2)
        {
            hashValue = HashFS(to_string(key),tableSize,BOUND);
        }
        else
        {
            hashValue = HashFS(to_string(key),tableSize);
        }
        break;
        

    case MS:
        hashValue = HashMS(key,tableSize);
        break;

    default:
        cout << "INVALID Hash Fn type! ... try again\n";
        break;

    }

    chainValues[hashValue].push_back(pair(key,element));

    return hashValue;
}


// search in the ll at a vector index
int search(int key, int tableSize, hashType ht = DM) {
    int hashValue , ftchoice = 1;

    switch (ht)
    {

    case DM:
        hashValue = HashDM(key,tableSize);
        break;

    case MM:
        hashValue = HashMM(key,tableSize);
        break;

    case FS:
        cout << "enter your Folding method Type 1)BOUND 2)SHIFT:\n";
        cin >> ftchoice;
        if (ftchoice == 2)
        {
            hashValue = HashFS(to_string(key),tableSize,BOUND);
        }
        else
        {
            hashValue = HashFS(to_string(key),tableSize);
        }
        break;
        

    case MS:
        hashValue = HashMS(key,tableSize);
        break;

    default:
        cout << "INVALID Hash Fn type! ... try again\n";
        break;

    }


    auto it = find_if(chainValues[hashValue].begin(), chainValues[hashValue].end(), 
                      [key](const pair<int, int>& element) { return element.first == key; });

    if (it != chainValues[hashValue].end()) {
        return it->second; // return the element if found
    } else {
        return -1; // return -1 if not found
    }
}

void printChains(vector<list<pair<int,int>>> chainValues){
    for (int i = 0; i < chainValues.size(); i++)
    {
        cout << "list No." << i << endl;
        int j = 0;
        for (auto it = chainValues[i].begin(); it != chainValues[i].end(); ++it, ++j)
        {
            cout << "Pair No." << j << " (" << it->first << ", " << it->second << ")" << endl;
        }
    }

}

//                                                       Linear Probing                                                        //
//-------------------------------------------------------------------------------------------------------------------------------

vector<pair<int,int>>values;    // used in LP , QP & DH

bool isFull(vector<pair<int,int>>v){
    bool flag = false;

    for (int i = 0; i < v.size(); i++)
    {
        if (v[i].first == INT_MIN)
        {
            // not full
            break;
        }
        else
        {
            flag = true;
        }
        
        
    }
    
    return flag;
}

void intialize(int ts){
    values.resize(ts,pair(INT_MIN,0));
}

int insertLP(int key, int element, int tableSize, hashType ht = DM){
    int hashValue , ftchoice = 1;
    int i = 0;

    switch (ht)
    {

    case DM:
        hashValue = HashDM(key,tableSize);
        break;

    case MM:
        hashValue = HashMM(key,tableSize);
        break;

    case FS:
        cout << "enter your Folding method Type 1) SHIFT or 2) BOUND:\n";
        cin >> ftchoice;
        if (ftchoice == 2)
        {
            hashValue = HashFS(to_string(key),tableSize,BOUND);
        }
        else
        {
            hashValue = HashFS(to_string(key),tableSize);
        }
        break;
        
    case MS:
        hashValue = HashMS(key,tableSize);
        break;

    default:
        cout << "INVALID Hash Fn type! ... try again\n";
        break;

    }

    hashValue = HashDM(key,tableSize);

    while (values[(hashValue + i) % tableSize].first != INT_MIN && i < tableSize )
    {
        i++;
    }

    if (isFull(values))
    {
        cout << "the table is full!\n";
        return -1;
    }
        

    values[(hashValue + i) % tableSize] = pair(key,element);
    return hashValue;
}

int searchLP(int key , int tableSize, hashType ht = DM){

    int hashValue , ftchoice = 1;
    int i = 0;

    switch (ht)
    {

    case DM:
        hashValue = HashDM(key,tableSize);
        break;

    case MM:
        hashValue = HashMM(key,tableSize);
        break;

    case FS:
        cout << "enter your Folding method Type 1) SHIFT or 2) BOUND:\n";
        cin >> ftchoice;
        if (ftchoice == 2)
        {
            hashValue = HashFS(to_string(key),tableSize,BOUND);
        }
        else
        {
            hashValue = HashFS(to_string(key),tableSize);
        }
        break;
        
    case MS:
        hashValue = HashMS(key,tableSize);
        break;

    default:
        cout << "INVALID Hash Fn type! ... try again\n";
        break;

    }

    while (values[(hashValue + i) % tableSize].first != INT_MIN && i < tableSize )
    {
        if (values[(hashValue + i) % tableSize].first == key)
        {
            cout << "key found\n";
            return (hashValue + i) % tableSize;
        }
        i++;
        
    }

    cout << "key not found\n";
    return -1;

}

//                                                      Quadratic Probing                                                      //
//-------------------------------------------------------------------------------------------------------------------------------

int insertQP(int key, int element, int tableSize, hashType ht = DM){
    int hashValue , ftchoice = 1;
    int i = 0;

    switch (ht)
    {

    case DM:
        hashValue = HashDM(key,tableSize);
        break;

    case MM:
        hashValue = HashMM(key,tableSize);
        break;

    case FS:
        cout << "enter your Folding method Type 1) SHIFT or 2) BOUND:\n";
        cin >> ftchoice;
        if (ftchoice == 2)
        {
            hashValue = HashFS(to_string(key),tableSize,BOUND);
        }
        else
        {
            hashValue = HashFS(to_string(key),tableSize);
        }
        break;
        
    case MS:
        hashValue = HashMS(key,tableSize);
        break;

    default:
        cout << "INVALID Hash Fn type! ... try again\n";
        break;

    }

    hashValue = HashDM(key,tableSize);

    while (values[(hashValue + (i*i)) % tableSize].first != INT_MIN && i < tableSize )
    {
        i++;
    }

    if (isFull(values))
    {
        cout << "the table is full!\n";
        return -1;
    }
        

    values[(hashValue + (i*i)) % tableSize] = pair(key,element);
    return hashValue;
}

int searchQP(int key , int tableSize, hashType ht = DM){

    int hashValue , ftchoice = 1;
    int i = 0;

    switch (ht)
    {

    case DM:
        hashValue = HashDM(key,tableSize);
        break;

    case MM:
        hashValue = HashMM(key,tableSize);
        break;

    case FS:
        cout << "enter your Folding method Type 1) SHIFT or 2) BOUND:\n";
        cin >> ftchoice;
        if (ftchoice == 2)
        {
            hashValue = HashFS(to_string(key),tableSize,BOUND);
        }
        else
        {
            hashValue = HashFS(to_string(key),tableSize);
        }
        break;
        
    case MS:
        hashValue = HashMS(key,tableSize);
        break;

    default:
        cout << "INVALID Hash Fn type! ... try again\n";
        break;
    }

    while (values[(hashValue + (i*i)) % tableSize].first != INT_MIN && i < tableSize )
    {
        if (values[(hashValue + (i*i)) % tableSize].first == key)
        {
            cout << "key found\n";
            return (hashValue + (i*i)) % tableSize;
        }
        i++;
    }

    cout << "key not found\n";
    return -1;

}

//                                                       Double Hashing                                                        //
//-------------------------------------------------------------------------------------------------------------------------------
int insertDH(int key, int element, int tableSize)
{
    int hashValue;
    int j = 0;
    
    while (true)
    {
        hashValue = HashDM(key,tableSize) + j * HashMM(key,tableSize);
    
        // add if empty
        if (values[hashValue].first == INT_MIN )
        {
            values[hashValue] = pair(key,element);
            break;        
        }

        else
        {
            j++;
        }
        
    }

    return hashValue;
}

int searchDH(int key, int tableSize){
    int hashValue;
    int j = 0;
    
    while (true)
    {
        hashValue = HashDM(key,tableSize) + j * HashMM(key,tableSize);
    
        // add if empty
        if (values[hashValue].first == key )
        {
            return hashValue;
        }

        else
        {
            j++;
        }
        
    }

    return -1;
}







int main() {

    // // testing DM
    // cout << "DM: "<< HashDM(1,30) << endl;
    // cout << "DM: "<< HashDM(2,30) << endl;


    // // // testing MM
    // cout << "MM: "<< HashMM(1,30) << endl;
    // cout << "MM: "<< HashMM(2,30) << endl;
    
    // testing Folding Method
    // cout << "the hash value of the key '12345678' using Shift Fold is:" << HashFS("12345678",100) << endl;    //80
    // cout << "the hash value of the key '12345678' using Bound Fold is:" << HashFS("12345678",100,BOUND) << endl;    //98


    // testing midsquare
    // cout << HashMS(1234,1000);

    // testing chaining <chaining program>
    //-------------------------------------------------------------------------------------------------------------------------------
    // int ts = 0, key, element;
    // bool cont = true;
    // char c = 'y';

    // cout << "Enter the Table size: ";
    // cin >> ts;
    // chainValues.resize(ts);
    
    // while (cont)
    // {
    //     cout << "Enter Key followed by element values to insert: \n";
    //     cin >> key;
    //     cin >> element;

    //     insert(key,element,ts);

    //     cout << "want to add another key element? y/n\n";
    //     cin >> c;
    //     if (c == 'n')
    //     {
    //         cont = false;
    //     }
    // }

    // printChains(chainValues);
    //-------------------------------------------------------------------------------------------------------------------------------

    
    // testing linear probing
    //-----------------------------------------------------------------------------------------------------------------------------
    // int ts = 0, key, element;
    // bool cont = true;
    // char c = 'y';

    // cout << "Enter the Table size: ";
    // cin >> ts;
    // intialize(ts);

    // while (cont)
    // {
    //     cout << "Enter Key followed by element values to insert: \n";
    //     cin >> key;
    //     cin >> element;

    //     insertLP(key,element,ts);

    //     cout << "want to add another key element? y/n\n";
    //     cin >> c;
    //     if (c == 'n')
    //     {
    //         cont = false;
    //     }
    // }

    // for (int i = 0; i < values.size(); i++)
    // {
    //     cout << "key: " << values[i].first << ", value: " << values[i].second << endl;  
    // }
    
    // int index = searchLP(22,ts);
    // cout << "search result: \n";
    // cout << "key: " << values[index].first << ", value: " << values[index].second << endl;  // 22, 3333
    //-------------------------------------------------------------------------------------------------------------------------------
   
    // testing Quadrstic probing
    //-----------------------------------------------------------------------------------------------------------------------------
    // int ts = 0, key, element;
    // bool cont = true;
    // char c = 'y';

    // cout << "Enter the Table size: ";
    // cin >> ts;
    // intialize(ts);

    // while (cont)
    // {
    //     cout << "Enter Key followed by element values to insert: \n";
    //     cin >> key;
    //     cin >> element;

    //     insertQP(key,element,ts);

    //     cout << "want to add another key element? y/n\n";
    //     cin >> c;
    //     if (c == 'n')
    //     {
    //         cont = false;
    //     }
    // }

    // for (int i = 0; i < values.size(); i++)
    // {
    //     cout << "key: " << values[i].first << ", value: " << values[i].second << endl;  
    // }
    
    // int index = searchQP(22,ts);
    // cout << "search result: \n";
    // cout << "key: " << values[index].first << ", value: " << values[index].second << endl;  // 22, 3333
    //-------------------------------------------------------------------------------------------------------------------------------
    
    // testing Double Hashing
    //-----------------------------------------------------------------------------------------------------------------------------
    int ts = 0, key, element;
    bool cont = true;
    char c = 'y';

    cout << "Enter the Table size: ";
    cin >> ts;
    intialize(ts);

    while (cont)
    {
        cout << "Enter Key followed by element values to insert: \n";
        cin >> key;
        cin >> element;

        insertDH(key,element,ts);

        cout << "want to add another key element? y/n\n";
        cin >> c;
        if (c == 'n')
        {
            cont = false;
        }
    }

    for (int i = 0; i < values.size(); i++)
    {
        cout << i <<")" <<"key: " << values[i].first << ", value: " << values[i].second << endl;  
    }
    
    int index = searchDH(22,ts);
    cout << "search result: \n";
    cout << "key: " << values[index].first << ", value: " << values[index].second << endl;  // 22 , 
    //-------------------------------------------------------------------------------------------------------------------------------

    return 0;

}