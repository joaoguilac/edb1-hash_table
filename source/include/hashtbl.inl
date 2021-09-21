#include "hashtbl.h"

namespace ac {
    /*!
     * @brief Regular constructor of a Hash table.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     * @param sz will determine the size of the table, being the smallest 
     * prime number >= than the value specified in this parameter.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( size_type sz )
	{
        // Set attributes.
        m_size = find_next_prime(sz);
        m_count = 0;
        m_table = std::unique_ptr<list_type[]> (new list_type[m_size]);
	}

    /*!
     * @brief Copy constructor from another hash table.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     * @param source the hash table that will be copied.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( const HashTbl& source )
	{
        // Set attributes.
        m_size = source.m_size;
        m_count = source.m_count;
        m_max_load_factor = source.m_max_load_factor;
        m_table = std::unique_ptr<list_type[]> (new list_type[m_size]);
        // Run through all elements.
        for (size_t i{0}; i < m_size; i++) {
            auto element = source.m_table[i].begin(); // First element of collision list i.
            auto sz = source.m_table[i].size(); // Size of collision list i.
            for (size_t j{0}; j < sz; j++) {
                entry_type new_entry{ element->m_key , element->m_data }; // Element of collision list.
                m_table[i].push_back( new_entry ); // Add the element to the collision list.
                element++; // Next element of collision list i.
            }
        }
	}

    /*!
     * @brief Constructor from an initializer list.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     * @param ilist the initializer list that the data of the elements will be copied.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( const std::initializer_list<entry_type>& ilist )
    {
        // Set attributes.
        m_size = ilist.size();
        m_count = 0;
        m_table = std::unique_ptr<list_type[]> (new list_type[m_size]);
        // Run through all elements.
        auto element = ilist.begin(); // First element of initializer list.
        for (size_t i{0}; i < m_size; i++) {
            insert( element->m_key, element->m_data );
            element++;
        }
    }

    /*!
     * @brief Assignment operator with another hash table.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     * @param clone the hash table that will be copied.
     * @return the hash table with the same attributes as the copied hash table.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>&
    HashTbl<KeyType,DataType,KeyHash,KeyEqual>::operator=( const HashTbl& clone )
    {
        // Set attributes.
        m_size = clone.m_size;
        m_count = clone.m_count;
        m_max_load_factor = clone.m_max_load_factor;
        m_table = std::unique_ptr<list_type[]> (new list_type[m_size]);
        // Run through all elements.
        for (size_t i{0}; i < m_size; i++) {
            auto element = clone.m_table[i].begin(); // First element of collision list i.
            auto sz = clone.m_table[i].size(); // Size of collision list i.
            for (size_t j{0}; j < sz; j++) {
                entry_type new_entry{ element->m_key , element->m_data }; // Element of collision list.
                m_table[i].push_back( new_entry ); // Add the element to the collision list.
                element++; // Next element of collision list i.
            }
        }
        return *this;
    }

    /*!
     * @brief Assignment operator with a initializer list.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     * @param ilist the initializer list that the data of the elements will be copied.
     * @return the hash table with the data of the elements of the initializer list.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>&
    HashTbl<KeyType,DataType,KeyHash,KeyEqual>::operator=( const std::initializer_list< entry_type >& ilist )
    {
        // Set attributes.
        m_size = ilist.size();
        m_count = 0;
        m_table = std::unique_ptr<list_type[]> (new list_type[m_size]);
        // Run through all elements.
        auto element = ilist.begin(); // First element of initializer list.
        for (size_t i{0}; i < m_size; i++) {
            bool successful = insert( element->m_key, element->m_data );
            element++;
        }
        return *this;
    }

    /*!
     * @brief Destroy the HashTbl object.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::~HashTbl( )
	{
        clear(); // Could be empty, due to the use of smart pointer.
	}

    /*!
     * @brief Inserts into the table the information contained in new_data_ and associated with a key key_.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     * @param key_ element key to be inserted.
     * @param new_data_ element data to be inserted.
     * @return true if a new element was inserted in the table.
     * @return false if the key already exists and the data has just been overwritten.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	bool HashTbl<KeyType,DataType,KeyHash,KeyEqual>::insert( const KeyType & key_, const DataType & new_data_ )
    {
        KeyHash hashFunc; // Instantiate the "functor" for primary hash.
        KeyEqual equalFunc; // Instantiate the "functor" for the equal to test.
        entry_type new_entry{ key_ , new_data_ }; // Create a new entry based on arguments.
        // Apply double hashing method, one functor and the other with modulo function.
        auto end{ hashFunc( key_ ) % m_size };
        // First element of collision list that will be insert the new_entry.
        auto it = m_table[end].begin();
        // Comparing keys inside the collision list.
        for (size_t i{0}; i < m_table[end].size(); i++) {
            // In this case, the key already exists in the table.
            if ( true == equalFunc( it->m_key, new_entry.m_key ) ) {
                it->m_data = new_data_; // Update the data of the element.
                return false;
            }
            it++;
        }
        // In this case, a new element will be inserted into the table.
        m_table[end].push_back( new_entry );
        m_count++;
        // Check if it is necessary to rehash().
        if (m_count / m_size > m_max_load_factor) {
            rehash();
        }
        return true;
    }
	
    /*!
     * @brief Clears the data table.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::clear()
    {
        // Clears all linked lists (std::list) in the table.
        for (size_t i{0}; i < m_size; i++) {
            m_table[i].clear();
        }
        m_count = 0; // No elements in hash table.
    }

    /*!
     * @brief Tests whether the table is empty.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     * @return true is table is empty; false, otherwise.
     */
    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::empty() const
    {
        if (m_count == 0)
            return true;
        else
            return false;
    }

    //----------------------------------------------------------------------------------------
    //! Retrieves data from the table.
    /*! @brief Retrieves a data item from the table, based on the key associated with the data.
     *  If the data cannot be found, false is returned; otherwise, true is returned instead.
     *  @param key_ Data key to search for in the table.
     *  @param data_item_ Data record to be filled in when data item is found.
     *  @return true if the data item is found; false, otherwise.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::retrieve( const KeyType & key_, DataType & data_item_ ) const
    {
        KeyHash hashFunc; // Instantiate the "functor" for primary hash.
        KeyEqual equalFunc; // Instantiate the "functor" for the equal to test.
        // Apply double hashing method, one functor and the other with modulo function.
        auto end{ hashFunc( key_ ) % m_size };
        // First element of collision list that will be searched the element.
        auto it = m_table[end].begin();
        for (size_t i{0}; i < m_table[end].size(); i++) {
            // The element key was found and its data returned.
            if ( true == equalFunc( it->m_key, key_ ) ) {
                data_item_ = it->m_data;
                return true;
            }
            it++; // Next element of collision list.
        }
        return false;
    }

    /*!
     * @brief Method that will be called when load factor is greater than m_max_load_factor.
     * This method will create a new table whose size will be equal to smaller
     * prime number >= than twice the size of the table before the rehash() call. 
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::rehash( void )
    {
        KeyHash hashFunc; // Instantiate the "functor" for primary hash.
        KeyEqual equalFunc; // Instantiate the "functor" for the equal to test.
        // Auxiliary hash table.
        size_type size_aux = find_next_prime(m_size * 2);
        std::unique_ptr<list_type[]> table_aux = std::unique_ptr<list_type[]> (new list_type[size_aux]);
        // The table elements will be passed to the auxiliary table.
        for (size_t i{0}; i < m_size; i++) {
            auto element = m_table[i].begin(); // First element of collision list i.
            auto sz = m_table[i].size(); // Size of collision list i.
            for (size_t j{0}; j < sz; j++) {
                entry_type new_entry{ element->m_key , element->m_data }; // Element of collision list.
                // Apply double hashing method, one functor and the other with modulo function.
                auto end{ hashFunc( element->m_key ) % size_aux };
                table_aux[end].push_back( new_entry ); // Add the element to the collision list.
                element++; // Next element of collision list i.
            }
        }
        // Update attributes.
        m_size = size_aux;
        m_table = std::unique_ptr<list_type[]> (new list_type[m_size]);
        // The elements of the auxiliary hash table will be passed to the original hash table.
        for (size_t i{0}; i < m_size; i++) {
            auto element = table_aux[i].begin(); // First element of collision list i.
            auto sz = table_aux[i].size(); // Size of collision list i.
            for (size_t j{0}; j < sz; j++) {
                entry_type new_entry{ element->m_key , element->m_data }; // Element of collision list.
                // Apply double hashing method, one functor and the other with modulo function.
                auto end{ hashFunc( element->m_key ) % size_aux };
                m_table[end].push_back( new_entry ); // Add the element to the collision list.
                element++; // Next element of collision list i.
            }
        }
    }

    /*!
     * @brief Removes a table item identified by its key_ key.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     * @param key_ the key of the element to be removed.
     * @return true if key is found; false, otherwise.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl< KeyType, DataType, KeyHash, KeyEqual >::erase( const KeyType & key_ )
    {
        KeyHash hashFunc; // Instantiate the "functor" for primary hash.
        KeyEqual equalFunc; // Instantiate the "functor" for the equal to test.
        // Apply double hashing method, one functor and the other with modulo function.
        auto end{ hashFunc( key_ ) % m_size };
        // First element in the collision list of the element to be removed.
        auto it = m_table[end].begin();
        // Comparing keys inside the collision list.
        for (size_t i{0}; i < m_table[end].size(); i++) {
            // If it finds the key, removes the element and decreases the number of elements (m_count).
            if ( true == equalFunc( it->m_key, key_ ) ) {
                m_table[end].erase(it);
                m_count--;
                return true;
            }
            it++;
        }
        return false;
    }

    // Function that returns true if n
    // is prime else returns false
    
    /**
     * @brief Checks if n_ is prime or not.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client. 
     * @param n_ the number that will be checked if it is prime or not.
     * @return true if n_ is prime; false, otherwise.
     * 
     * @see The implementation was copied by the website:
     * https://www.geeksforgeeks.org/program-to-find-the-next-prime-number/
     */
    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::is_prime( size_type n_ )
    {
        // Corner cases
        if (n_ <= 1) return false;
        if (n_ <= 3) return true;
    
        // This is checked so that we can skip 
        // middle five numbers in below loop
        if (n_%2 == 0 || n_%3 == 0) return false;
    
        for (size_type i=5; i*i<=n_; i=i+6)
            if (n_%i == 0 || n_%(i+2) == 0)
                return false;
    
        return true;
    }

    /**
     * @brief Find the next prime >= N.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client. 
     * @param N lower limit of the list of primes that will be searched.
     * @return the smallest prime number greater than or equal to N.
     * 
     * @see The implementation was copied by the website:
     * https://www.geeksforgeeks.org/program-to-find-the-next-prime-number/
     */
    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    typename HashTbl< KeyType, DataType, KeyHash, KeyEqual >::size_type
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::find_next_prime(size_type N)
    {
        // Base case
        if (N <= 1)
            return 2;
    
        size_type prime = N;
        bool found = false;
    
        // Loop continuously until is_prime returns
        // true for a number greater than n
        while (!found) {
            prime++;
    
            if (is_prime(prime))
                found = true;
        }
    
        return prime;
    }

    /*!
     * @brief Returns the number of table elements that are in the collision list associated with key key_.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client. 
     * @param key_ key whose collision list will be searched.
     * @return HashTbl< KeyType, DataType, KeyHash, KeyEqual >::size_type 
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    typename HashTbl< KeyType, DataType, KeyHash, KeyEqual >::size_type
    HashTbl< KeyType, DataType, KeyHash, KeyEqual >::count( const KeyType & key_ ) const
    {
        KeyHash hashFunc; // Instantiate the "functor" for primary hash.
        KeyEqual equalFunc; // Instantiate the "functor" for the equal to test.
        // Apply double hashing method, one functor and the other with modulo function.
        auto end{ hashFunc( key_ ) % m_size };
        return m_table[end].size();
    }

    /*!
     * @brief Returns a reference to the data associated with the given key key_.
     * If the key is not in the table, the method throws an exception of type std::out_of_range.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     * @param key_ key that we look for the data.
     * @return DataType& reference to the data associated with the given key.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::at( const KeyType & key_ )
    {
        KeyHash hashFunc; // Instantiate the "functor" for primary hash.
        KeyEqual equalFunc; // Instantiate the "functor" for the equal to test.
        // Apply double hashing method, one functor and the other with modulo function.
        auto end{ hashFunc( key_ ) % m_size };
        // First element in the collision list of the element to be searched for.
        auto it = m_table[end].begin();
        // Comparing keys inside the collision list.
        for (size_t i{0}; i < m_table[end].size(); i++) {
            if ( true == equalFunc( it->m_key, key_ ) ) {
                return it->m_data;
            }
            it++;
        }
        // The case where the element is not found.
        throw std::out_of_range("[HashTbl::at()]: key doesn't exist in the hash table.");
    }

    /*!
     * @brief Returns a reference to the data associated with the given key key_, if any. If the key is not in the 
     * table, the method performs the insert and returns the reference to the newly inserted data in the table.
     * @tparam KeyType type of key stored in hash table.
     * @tparam DataType data type stored in hash table.
     * @tparam KeyHash type of primary dispersion function received by the client.
     * @tparam KeyEqual type of equal key comparison function 
     * performed on the collision list received by the client.
     * @param key_ the given key.
     * @return DataType& reference to the data associated with the given key.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator[]( const KeyType & key_ )
    {
        // Tries to find the element through the at method.
        try {
            auto it = at( key_ );
            return at( key_ );
        }
        // If the exception is returned we do an insertion of the key.
        catch (const std::out_of_range& e) {
            insert( key_, DataType{} );
            return at( key_ );
        }
    }
} // Namespace ac.
