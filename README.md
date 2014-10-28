OrakleDatabase
==============
It's a simple Oracle-like database system written in C++.
The user can provide a schema to a database and add one or more data records to a database, either retrieving these 
rows of data from a website on the Internet, or letting the user add them one at a time locally.
The user are also able to issue a query to the database and obtain a collection of
records that match the query’s search criteria, ordered in a manner consistent with the query’s sorting criteria.
I build a class called MultiMap and MultiMap::Iterator, representing a collection of key/value associations; 
this class can store same key but different values, search for certain items and is implemented using a binary search tree.
