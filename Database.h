#ifndef Database_H
#define Database_H

#include<string>
#include<vector>
#include"MultiMap.h"

class Database
{
public:
	enum IndexType{it_none,it_indexed};
	enum OrderingType{ot_ascending,ot_descending};
	
	struct FieldDescriptor
	{
		std::string name;
		IndexType index;
	};
	
	struct SearchCriterion
	{
		std::string fieldName;
		std::string minValue;
		std::string maxValue;
	};

	struct SortCriterion
	{
		std::string fieldName;
		OrderingType ordering;
	};
	static const int ERROR_RESULT = -1;
	Database();
	~Database();
	bool specifySchema(const std::vector<FieldDescriptor>&schema);
	bool addRow(const std::vector<std::string>&rowOfData);
	bool loadFromURL(std::string url);
	bool loadFromFile(std::string filename);
	int getNumRows() const;
	bool getRow(int rowNum, std::vector<std::string>& row) const;
	int search(const std::vector<SearchCriterion>& searchCriteria,
		const std::vector<SortCriterion>& sortCriteria,std::vector<int>& results);
private:
	//To prevent Databases from being copied or assigned, declare these members
	//private and do not implement them.
	Database(const Database&other);
	Database&operator=(const Database&rhs);
	int partition(std::vector<std::string>& a,std::vector<int>& index,int low,int high);            //partition function used by quicksort
	void quickSort(std::vector<std::string>& a,std::vector<int>& index,int first,int last);         //quicksort algorithm
	std::vector<FieldDescriptor> m_schema;                //a vector to hold fieldDescriptor
	std::vector<MultiMap*> m_fieldIndex;                  //a vector to hold MultiMap
	std::vector<std::vector<std::string>> m_rows;         //a vector to hold vector of string
	std::vector<SortCriterion> m_sortCriteria;            //a vector to hold sortcriterion
};

#endif