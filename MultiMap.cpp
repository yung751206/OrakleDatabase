#include "MultiMap.h"
using namespace std;

MultiMap::MultiMap()
{
	m_root = nullptr;  //initialize m_root pointer to nullptr;
}

MultiMap::~MultiMap()   //remove all the associations from the multimap by calling clear() function;
{
	clear();
}

void MultiMap::BTNode::addValue(unsigned int value)
{
	ValueNode *curPtr = m_value;
	while(curPtr->next!=nullptr)
	{ curPtr = curPtr->next;}              //traversal to the end of value node
	curPtr->next = new ValueNode(value);   //create a new valueNode
	curPtr->next->pre = curPtr;            //assign current pointer's next node's previous pointer to current node;
}

void MultiMap::insert(std::string key,unsigned int value)
{
	if(m_root == nullptr)           //Check if there is no node in the map.
	{
		m_root = new BTNode(key,value);
		m_root ->parent = nullptr;   //root pointer's parent is nullptr
		return;
	}

	BTNode *cur = m_root;           
	for(;;)							 //infinite for loop
	{ 
		if(key == cur->m_key)       //If there is a same key in the map
		{                           //,then just add value to the map
			cur->addValue(value);    //This is a multiple-value-per-node approach
			return;          
		}
		if(key < cur->m_key)           //If key is less than the current's key,
		{                              //then go to the left subtree
			if(cur->left!=nullptr)
			{cur = cur->left;}
			else
			{
				cur->left = new BTNode(key,value);  //If current node's left node is nullptr
				cur->left->parent = cur;			//create a new node
				return;
			}
		}
		else if (key > cur->m_key)           //If key is larger than the current's key,
		{                                   //then go to the right subtree
			if(cur->right!= nullptr)
			{ cur = cur->right;}
			else
			{
				cur->right = new BTNode(key,value);   //If current node's left node is nullptr
				cur->right->parent = cur;              //create a new node
				return;
			}
		}
	}
}

void MultiMap::clear()
{
	clearHelper(m_root);      //Call clearhelper function to help clean the associations
	m_root = nullptr;         //Reset m_root pointer to nullptr
}

void MultiMap::clearHelper(BTNode* node)  //Take node as input
{
	BTNode* temp = node; 
	if(temp == nullptr)                   //If the node is empty, return; it's a base case.
		return;
	BTNode* tempLeft = node->left;       //assign node's left and right node to temp node
	BTNode* tempRight = node->right;

	while(node->m_value != nullptr)       //Used to detect a node having multivalue
	{
		ValueNode* tempValueNode = node->m_value->next;  //Point to the next value node
		delete node->m_value;              //Delete current value node
		node->m_value = tempValueNode;    //assign value in the node to next value
	}
	delete node;                       //Because there is no value in the node, delete the node directly
	clearHelper(tempLeft);            //clean the left node
	clearHelper(tempRight);			//clean the right node
}


MultiMap::Iterator MultiMap::findEqual(string key) const //Find if the key is equal to the key in the map 
{
	BTNode* tempNode = m_root;					//start from the root node
	while(tempNode != nullptr)                  //check if it is null pointer
	{
		if(key == tempNode->m_key)				//The key is in the map; return true
			return Iterator(true,tempNode);
		else if(key < tempNode->m_key)			//Find left node
			tempNode = tempNode->left;
		else
			tempNode = tempNode->right;			//Find right node
	}
	return Iterator();							//Cannot find the key
}

MultiMap::Iterator MultiMap:: findEqualOrSuccessor(std::string key) const //Find if there is a key in the map larger or equal to the key
{
	Iterator it = findEqual(m_root->m_key);            //use findequal function to find equal key
	Iterator p = it;

	if(key == it.getKey())						//If the key is in the map, return it
		return it;
	else if(key > it.getKey())					//If the key is bigger than the current key
	{
		while(it.next())						//Using next function to find next key in the map
		{
			if(key <it.getKey())				//If the key is smaller than the key returned by the getKey()
				return it;						//then we find it
			else if(key==it.getKey())		
				return it;
		}
		return it;
	}
	else									////If the key is smaller than the current key
	{

		while(it.prev())					//If there is a previous key
		{
			p = it;
			if(key >it.getKey())			//If the key is bigger than the current key
			{								//then the next key is the one we want
				it.next();					
				return it;
			}
			else if(key == it.getKey())		//If the key is equal to the key returned by the getKey()
				return it;					//return it
		}
		return p;
	}
}

MultiMap::Iterator MultiMap:: findEqualOrPredecessor(std::string key) const //The algorithm is the same as next() function
{
	Iterator it = findEqual(m_root->m_key);
	Iterator p = it;

	if(key == it.getKey())				//If the key is in the map, return it
		return it;
	else if(key > it.getKey())			///If the key is bigger than the current key
	{
		while(it.next())
		{
			p = it;
			if(key <it.getKey())
			{	
				it.prev();
				return it;
			}
			else if(key ==it.getKey())			//If the key is equal to the key returned by the getKey()
			{									//return it
				return it;
			}

		}
		return p;
	}
	else if(key <it.getKey())
	{
		while(it.prev())			//If there is a previous key
		{
			if(key >it.getKey())
			{
				return it;
			}
			else if(key == it.getKey())  //If the key is equal to the key returned by the getKey()
			{							//return it
				return it;
			}

		}
		return it;
	}
}

MultiMap::Iterator::Iterator()   //Create a iterator in an invalid state
{
	m_valid = false;
}
MultiMap::Iterator::Iterator(bool valid,BTNode* node) //Create a iterator having two parameters
{
	m_valid = valid;
	m_cur = node;
}




bool MultiMap::Iterator::valid() const    //return valid state
{
	return m_valid;
}

string MultiMap::Iterator::getKey() const  //return the key in the iterator
{
	return m_cur->m_key;
}

unsigned int MultiMap::Iterator::getValue() const //return the value in the iterator
{
	return m_cur->m_value->m_value;
}

bool MultiMap::Iterator::next()
{
	if(m_valid == false)             //If the iterator is in an invalid state,do nothing and return false
		return false;
	else if(m_cur->m_value->next != nullptr)   //Because this is a multiple-value-per-node approach, check if there is a next value in the current node
	{                                          
		m_cur->m_value = m_cur->m_value->next;  //return the next value in the same node
		return true;
	}
	else
	{
		if(m_cur->right != nullptr)                 //if the current node has the right node
		{
			BTNode* tempNode = m_cur->right;    
			while(tempNode->left != nullptr)        //If this right node has left node
			{
				tempNode = tempNode->left;          
			}
			m_cur = tempNode;                 //assign the node which doesn't have left node to current node and return ture.       
			return true;
		}
		else                                         //If the current node doesn't have right node,
		{
			if(m_cur->parent != nullptr)     
			{
				if(m_cur == m_cur->parent->left)       //if the current node's parent node has left node, 
				{
					m_cur = m_cur->parent;			 //assign current node's parent node as current node and return true.
					return true;
				}
				else                                            
				{
					while(m_cur->parent != nullptr)                 
					{
						if(m_cur->parent->right == m_cur)       //It the current node belongs its parent node's right node, 
						{
							m_cur = m_cur->parent; 
						}
						else
						{
							m_cur = m_cur->parent;       //  //assign the node which doesn't have left node to current node and return ture.           
							return true;
						}
					}
					m_valid = false;                   // If the current node's parent node is null pointer, then return false and set state 
					return false;
				}
			}
			else
			{
				m_valid = false;    // If the current node's parent node is null pointer, then return false and set state 
				return false;
			}

		}
	}
}

bool MultiMap::Iterator::prev()                //Pretty much the same as next function
{
	if(m_valid == false)
		return false;
	else if(m_cur->m_value->pre != nullptr)
	{
		m_cur->m_value = m_cur->m_value->pre;
		return true;
	}
	else
	{
		if(m_cur->left != nullptr)
		{
			BTNode* tempNode = m_cur->left;
			while(tempNode->right != nullptr)
			{
				tempNode = tempNode->right;
			}
			m_cur = tempNode;
			return true;
		}
		else
		{
			if(m_cur->parent != nullptr)
			{
				if(m_cur == m_cur->parent->right)
				{
					m_cur = m_cur->parent;
					while(m_cur->m_value->next != nullptr)
						m_cur->m_value = m_cur->m_value->next;
					return true;
				}
				else
				{
					BTNode* tempLeftNode = m_cur;
					BTNode* tempNode = m_cur->parent;
					while(tempNode != nullptr)
					{
						if(tempNode->left == tempLeftNode)
						{
							if(tempNode->parent == nullptr)
							{
								m_valid = false;
								return false;
							}
							else
							{
								tempLeftNode = tempNode;
								tempNode = tempNode->parent;
							}
						}
						else
						{
							m_cur = tempNode;
							return true;
						}

					}

				}

			}
			else
			{
				m_valid = false;
				return false;
			}

		}
	}
}

