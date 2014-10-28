#ifndef MultiMap_H
#define MultiMap_H

#include<string>
class MultiMap
{
private:
//To prevent Multimaps from being copied or assigned, declare these members
//private and do not implement them.
	MultiMap(const MultiMap& other);
	MultiMap& operator=(const MultiMap&	rhs);
    struct ValueNode                                //multiple-value-per-node data structure
	{
		ValueNode(unsigned int value)              //constructor for valuenode
		{
			m_value = value;
			next = nullptr;
			pre = nullptr;
		}
		unsigned int m_value;                  //hold value 
		ValueNode *next;                       //the next valuenode pointer
		ValueNode *pre;                       //the previous valuenode pointer
	};
	 
	struct BTNode                                //struct for node in the tree
    {
		BTNode(std::string key,unsigned int value)    //constructor
		{
			m_key = key;	
			m_value = new ValueNode(value);
			left = nullptr; 
			right = nullptr;
		}
		void addValue(unsigned int value);   //multiple-value-per-node data structure uses this function to add value
        std::string   m_key;               //the key in the node
        ValueNode* m_value;                //the pointer of value in the node
		BTNode *left;                    //left pointer
		BTNode *right;                    //right pointer
		BTNode *parent;                   //parent pointer
    };
	BTNode* m_root;
	void clearHelper(BTNode* node);        //It's used to help the clean function

public:
//You must implement this public nested MultiMap::Iterator class
	class Iterator
	{
	public:
		Iterator();                           
		Iterator(bool valid,BTNode* node);//You must have a default constructor Iterator(/* you may have any parameters you like here */);
		bool valid() const;               //return ture if iterator is valid
		std::string getKey() const;       //return the key in the iterator
		unsigned int getValue() const;		//return the value in the iterator
		bool next();                      //find the next key or value
		bool prev();					//find the previous key or value

	private:
		bool m_valid;                  //hold valid state
		BTNode *m_cur;                 //current node
	};
	MultiMap();                                 //Constructor
	~MultiMap();								//Destructor
	void clear();                              //This function is uesd to remove all the associations from the multimap
	void insert(std::string key,unsigned int value);   //This function is uesd to add new association to the multimap
	Iterator findEqual(std::string key) const;     //Find if the key is in the map
	Iterator findEqualOrSuccessor(std::string key) const; //Find if there is a key in the map larger or equal to the key
	Iterator findEqualOrPredecessor(std::string key) const; //Find if there is a key in the map smaller or equal to the key

};

#endif