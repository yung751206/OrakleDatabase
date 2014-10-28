#include "Database.h"
#include "MultiMap.h"
#include "http.h"
#include "Tokenizer.h"
#include <vector>
#include<iostream>
#include<fstream>
using namespace std;
Database::Database()             //containing nothing         
{;}

Database::~Database()                //Release all resources held by the Database
{
	m_schema.clear();                    
	m_rows.clear();
	m_sortCriteria.clear();
	for(int i =0;i<m_fieldIndex.size();i++)
		m_fieldIndex[i]->clear();
	vector<MultiMap*>::iterator it;        //Use iterator to delete pointer in the multimap vector
	it = m_fieldIndex.begin();
	while(it!=m_fieldIndex.end())
	{
		delete *it;                         
		it = m_fieldIndex.erase(it);
	}
}

bool Database::specifySchema(const std::vector<FieldDescriptor>&schema)   
{
	bool hasIndex = false;            //This flag is used to check if schema has index 
	for(int i=0;i<schema.size();i++)
	{
		if(schema[i].index == Database::it_indexed)
			hasIndex = true;          //The schema  has index
	}
	if(hasIndex == false)             //The schema doesn't specify schema
		return false;                

	if(m_schema.empty() == true)     //The m_schemsa is empty
	{
		m_schema = schema;                       
		for(int i=0;i<schema.size();i++)   //Create new multimap to store index
		{
			m_fieldIndex.push_back(new MultiMap);
		}
		return true;                  
	}
	else
	{
		m_schema.clear();                     //clear all the data in the database
		m_rows.clear();                  
		m_sortCriteria.clear();                  

		for(int i =0;i<m_fieldIndex.size();i++)
			m_fieldIndex[i]->clear();
		vector<MultiMap*>::iterator it;        //Use iterator to delete pointer in the multimap vector
		it = m_fieldIndex.begin();
		while(it!=m_fieldIndex.end())
		{
			delete *it;                         
			it = m_fieldIndex.erase(it);
		}

		m_schema = schema;
		for(int i=0;i<schema.size();i++)
		{
			m_fieldIndex.push_back(new MultiMap);
		}
		return true;
	}
}

bool Database:: addRow(const std::vector<std::string>&rowOfData)
{
	if(m_schema.size()==0)                //Doesn't have schema and return fasle
		return false;
	if(m_schema.size() != rowOfData.size())     //Data is not in the right format
		return false;
	m_rows.push_back(rowOfData);               //Push data to the m_rows
	for(int i=0;i<m_schema.size();i++)
	{
		if(m_schema[i].index == Database::it_indexed)  //If the schema has index, the position of the data has to be added to the multimap
		{
			m_fieldIndex[i]->insert(rowOfData[i],m_rows.size()-1);
		}
	}
	return true;
}

int Database:: getNumRows() const          //return the size of the rows
{
	return m_rows.size();
}
//remember to clear row before assign
bool Database:: getRow(int rowNum, std::vector<std::string>& row) const
{
	if( (rowNum <0) ||( rowNum > (int)m_rows.size()-1))  //If row number is not right, return false
		return false;
	else
	{
		row.clear();                     //Clear the data in the row
		row = m_rows[rowNum];           //Assign new data to the row
		return true;
	}
}
bool Database::loadFromFile(std::string filename)
{
	string tokenizeMe;
	string delimiters =",";             //using "," as delimiters
	ifstream infile(filename);          //load file
	vector<FieldDescriptor> schema;
	vector<string> row;
	if(!infile)                          //if cannot open file
	{
		cout<<"Error: Cannot open file: "<<filename<<endl;
		return false;
	}
	if(getline(infile,tokenizeMe))                  //get first line fo string in the file and put it into string tokenizeMe
	{
		Tokenizer t(tokenizeMe,delimiters);         //Using Tokenizer class to fetch the word seperated by the ","
		string tempWord;
		while(t.getNextToken(tempWord))             //Chech if there is no next token
		{
			FieldDescriptor tempField;
			if(tempWord.back() =='*')                   //Check if the last character of the word contains '*'
			{             
				string withOutStar="";                   
				for(int i =0;i<tempWord.size()-1;i++)  //Get rid of the '*' char
					withOutStar += tempWord[i];
				tempField.name = withOutStar;             
				tempField.index = Database::it_indexed; // set index as it_indexed
			}
			else
			{
				tempField.name = tempWord;             
				tempField.index = Database::it_none;    //there is no "*' char in the word, so set index as it_none
			}
			schema.push_back(tempField);
		}
		specifySchema(schema);               //specify the schema got from the file
	}
	while(getline(infile,tokenizeMe))              //The rest line of string are all data
	{
		string tempWord;
		Tokenizer t(tokenizeMe,delimiters);
		while(t.getNextToken(tempWord))
		{
			row.push_back(tempWord);
		}
		addRow(row);                    //add data in the file to m_rows
		row.clear();
	} 
	return true;                    //Successfully loadTheFile

}

bool Database::loadFromURL(std::string url)
{
	string m_url = url;   
	string page;
	string delimiters ="\n";                  
	string tokenizeMe;
	string word;
	vector<string> row;
	if(HTTP().get(url,page))                   
	{
		tokenizeMe = page;
	}
	else 
	{
		cout<<"Error fetching content from URL "<<url<<endl;
		return false;
	}

	Tokenizer t(tokenizeMe,delimiters);        //Using this tokenizer class to get word line by line in the url 
	if(t.getNextToken(word))
	{
		string delimitersComma =",";             //Changing delimiter to "," to fetch the word seperated by the ","
		string tempWord;
		Tokenizer tempToken(word,delimitersComma);
		vector<FieldDescriptor> schema; 
		while(tempToken.getNextToken(tempWord))         
		{
			FieldDescriptor tempField;
			if(tempWord.back() =='*')               //Get rid of the '*' char
			{
				string withOutStar="";
				for(int i =0;i<tempWord.size()-1;i++)
					withOutStar += tempWord[i];
				tempField.name = withOutStar;
				tempField.index = Database::it_indexed;  // set index as it_indexed
			}
			else
			{
				tempField.name = tempWord;
				tempField.index = Database::it_none; //there is no "*' char in the word, so set index as it_none
			}
			schema.push_back(tempField);
		}
		specifySchema(schema);			//specify the schema got from the file
	}
	
	while(t.getNextToken(word))       //The rest lines of string are all data
	{ 
		string delimitersComma =",";
		string tempWord;
		Tokenizer tempToken(word,delimitersComma);
		while(tempToken.getNextToken(tempWord))
		{
			row.push_back(tempWord);
		}
		addRow(row);       //add data in the file to m_rows
		row.clear();
	} 
	return true;		  //Successfully loadTheURL
}

int Database::search(const std::vector<SearchCriterion>& searchCriteria,
		const std::vector<SortCriterion>& sortCriteria,std::vector<int>& results)
{
	bool wrongName = true;								//wrongName flag is used to test if the searchCriteria is right
	int indexOfMap;										//indexOfMap is used to remember the fieldname
	vector<int> tempResults;                            //Store the temp searching results
	m_sortCriteria = sortCriteria;                      //a vector to hold sortcriterion

	if(searchCriteria.size() ==0)                       //There is no searchCriteria
		return ERROR_RESULT;							//Return error_result
	for(int i =0;i<searchCriteria.size();i++)      
	{		
		tempResults.clear();                        //clear the tempResults
		for(int j =0;j<m_schema.size();j++)				//This step figures out what fieldname is in the searchCriteria
		{	
			if(searchCriteria[i].fieldName == m_schema[j].name)   
			{
				wrongName = false;
				indexOfMap = j;
			}
		}
		if(searchCriteria[i].maxValue =="" && searchCriteria[i].minValue =="")   //Check if the user doesn't specify the maxValue and minValue
			wrongName = true;

		if(wrongName)                //Return error_result and clean the values in the results
		{
			results.clear();
			return ERROR_RESULT;
		}

		if(searchCriteria[i].minValue != "" && searchCriteria[i].maxValue !="" )        //If the user specifies the min value and max value
		{	
			MultiMap::Iterator it=m_fieldIndex[indexOfMap]->findEqualOrSuccessor(searchCriteria[i].minValue); //Declare a iterator to find the equal or successor index
			while(it.valid())                                  //If find it
			{
				if(it.getKey()>searchCriteria[i].maxValue)     //Check if this index's key bigger than the max value
					break;                                     //it says that this index doesn't meet the criteria
				else                                          
				{
					tempResults.push_back(it.getValue());    //The index meets the criteria, push the value back to tempResults
					it.next();                               //Continue searching the next index
				}
			}
		}
		else if(searchCriteria[i].minValue != ""&& searchCriteria[i].maxValue =="")    //If the user specifies the min value but max value
		{
			MultiMap::Iterator it=m_fieldIndex[indexOfMap]->findEqualOrSuccessor(searchCriteria[i].minValue); //Declare a iterator to find the equal or successor index
			while(it.valid())								 //If find it
			{ 
				if(it.getKey()>searchCriteria[i].maxValue)  //Check if this index's key bigger than the max value
					break;									//it says that this index doesn't meet the criteria
				else
				{
					tempResults.push_back(it.getValue());  //The index meets the criteria, push the value back to tempResults
					it.next();								//Continue searching the next index
				}
			}
		}
		else if(searchCriteria[i].minValue == ""&& searchCriteria[i].maxValue !="")	//If the user specifies the max value but min value
		{	
			MultiMap::Iterator it=m_fieldIndex[indexOfMap]->findEqualOrPredecessor(searchCriteria[i].maxValue); //Declare a iterator to find the equal or predecessor index
			while(it.valid())									//If find it
			{
				if(it.getKey()<searchCriteria[i].minValue)			//Check if this index's key smaller than the min value
					break;										//it says that this index doesn't meet the criteria
				else
				{
					tempResults.push_back(it.getValue());		//The index meets the criteria, push the value back to tempResults
					it.prev();									//Continue searching the previous index
				}
			}
		}

		if(results.size() ==0)				//if there is no value in the results, it means don't need to do intersection
			results = tempResults;			//assign tempResults to results
		else
		{
			vector<int> helpResults = results;			//This variable is used to store the results
			results.clear();                            //Clear the results in order to store the intersection results
			for(int i =0;i<helpResults.size();i++)
			{
				vector<int>::iterator it;
				it = tempResults.begin();
				while(it!=tempResults.end())
				{
					if(*it == helpResults[i])             //It means that there is a interseciton in the position i
						results.push_back(helpResults[i]);  //Store this intersection into results vector
					it++;                                //Search next index
				}
			}
		}
	}
			
	vector<int> indexVector;                        //Create an indexVector to remember the sort criteria's fieldname index
	for(int i=0;i<sortCriteria.size();i++)                  
	{
		vector<string> sortMe;                      
		int indexOfField;
		for(int j =0;j<m_schema.size();j++)
		{	
			if(sortCriteria[i].fieldName == m_schema[j].name)  //Match the fieldName and schema's name
			{	
				indexOfField = j;
				indexVector.push_back(j);                     //Store the sort criteria's fieldname index
			}
		}
		for(int k =0;k<results.size();k++)						//Push back the rows we are going to search in the sortMe variable
			sortMe.push_back(m_rows[results[k]][indexOfField]);


		if(i==0)	                                         //The first sort criteria. There are no sort results
		{
			quickSort(sortMe,results,0,results.size()-1);   //Using quickSort function to do the sort
			if(sortCriteria[i].ordering == Database::ot_descending)  //Becasue the quickSort function sorts the array in ascending order, 
			{													//If the ordering in the sort criteria is descending, it needs to reverse the results
				vector<int> tempResults = results;
				for(int l =0;l<results.size();l++)
				{
					tempResults[l] = results[results.size()-l-1];  //reverse the results
				}
				results = tempResults;                  //assign the tempResults to results
			}
		}
		else                                        //After the first sort, there are values in the resutls, so the value in the
		{											//results need to meet some check function to change the resutls
			int start=-1;							//The start stores the first position in the results.
			int num=0;                              //The num stores the number of results needed to do the sort
			for(int k = 0;k<results.size();k++)     
			{
				if(k==results.size()-1)				//check if index k out of range of result's size
				{
					if(start ==-1)                  // It means that there is need to do the sort
						break;
					else           
					{ 
						quickSort(sortMe,results,start,start+num);     //Do the quick sort from start position to start+num position
						if(sortCriteria[i].ordering == Database::ot_descending)  //If the ordering in the sort criteria is descending, it needs to reverse the results
						{
							vector<int> tempResults = results;
							for(int l =0;l<num+1;l++)
							{
								tempResults[l+start] = results[start+num-l];
							}
							results = tempResults;
							start = -1;                        //reset start position to -1
							num = 0;                           //reset the num to 0
						}
						break;
					}
				}
				else
				{
					if(m_rows[results[k]][indexVector[i-1]] == m_rows[results[k+1]][indexVector[i-1]])
					{
						if(start == -1)
							start =k;                      //store the first position of array needed to do the sort
						num = num+1;                       //increase the num by 1
					}
					else
					{
						if(start == -1)                           //if start is equal to -1, check the next index
							continue;
						quickSort(sortMe,results,start,start+num);         //if start is not equal to -1, do the quickSort from start to start+num position
						if(sortCriteria[i].ordering == Database::ot_descending)  //If the ordering in the sort criteria is descending, it needs to reverse the results
						{
							vector<int> tempResults = results;
							for(int l =0;l<num+1;l++)
							{
								tempResults[l+start] = results[start+num-l];
							}
							results = tempResults;
							start = -1;
							num = 0;
						}
						else
						{
							start = -1;
							num =0;
						}

					}
				}
			}
		}
	}

	return results.size();  //return the result's size
}

int Database::partition(vector<string>& a, vector<int>& index,int low, int high)
{
	int pi = low;
	string pivot = a[low];  
	do
	{
		while ( low <= high && a[low] <= pivot )   //If a[low] <= pivot, low increases one
			low++;
		while ( a[high] > pivot )                 //If a[high] > pivot, high decreases one
			high--;
		if ( low < high)                               
		{	
				swap(a[low], a[high]);                 //if high > low, then swap the value.
				swap(index[low],index[high]);          //swap the index to change the result
		}
	}
	while ( low < high );
	swap(a[pi], a[high]);                     //swap the pi to the right position     
	swap(index[pi],index[high]);              
	pi = high;
	return(pi);            //return pi index
}

void Database::quickSort(vector<string>& a,vector<int>& index,int first,int last)  //Quicksort algorithm
{
    if (last - first >=1)
	{
		int pivotIndex;
		pivotIndex = partition(a,index,first,last);   //Using partition function to get the pivotIndex
		quickSort(a,index,first, pivotIndex-1);       //Using quicksort to sort from first to pivotIndex-1
		quickSort(a,index,pivotIndex+1,last);      //Using quicksort to sort from pivotIndex to last
	}
}