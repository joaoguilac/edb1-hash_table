// @author: Selan
//
#ifndef _HASHTBL_H_
#define _HASHTBL_H_

#include <iostream>     // cout, endl, ostream
#include <list> // list
#include <algorithm>    // copy, find_if, for_each
#include <cmath>        // sqrt
#include <iterator>     // std::begin(), std::end()
#include <initializer_list>
#include <utility> // std::pair
#include <memory>
#include <stdexcept> // std::out_of_range

namespace ac // Associative container
{
	template<class KeyType, class DataType>
	struct HashEntry {
        KeyType m_key;   //! Data key
        DataType m_data; //! The data

        // Regular constructor.
        HashEntry( KeyType kt_, DataType dt_ ) : m_key{kt_} , m_data{dt_}
        {/*Empty*/}

        friend std::ostream & operator<<( std::ostream & os_, const HashEntry & he_ ) {
            // os_ << "{" << he_.m_key << "," << he_.m_data << "}";
            os_ << he_.m_data;
            return os_;
        }
    };

	template< class KeyType,
		      class DataType,
		      class KeyHash = std::hash< KeyType >,
		      class KeyEqual = std::equal_to< KeyType > >
	class HashTbl {
        public:
            // Aliases
            using entry_type = HashEntry<KeyType,DataType>;
            using list_type = std::list< entry_type >;
            using size_type = std::size_t;

            explicit HashTbl( size_type table_sz_ = DEFAULT_SIZE );
            HashTbl( const HashTbl& );
            HashTbl( const std::initializer_list< entry_type > & );
            HashTbl& operator=( const HashTbl& );
            HashTbl& operator=( const std::initializer_list< entry_type > & );

            virtual ~HashTbl();

            bool insert( const KeyType &, const DataType &  );
            bool retrieve( const KeyType &, DataType & ) const;
            bool erase( const KeyType & );
            void clear();
            bool empty() const;
            inline size_type size() const { return m_count; };
            DataType& at( const KeyType& );
            DataType& operator[]( const KeyType& );
            size_type count( const KeyType& ) const;
            // Returns the maximum load factor of the hash table.
            float max_load_factor() const { return m_max_load_factor; };
            // Changes the maximum load factor of the hash table.
            void max_load_factor(float mlf) { m_max_load_factor = mlf; };

            //* Generates a textual representation of the table and its elements.
            friend std::ostream & operator<<( std::ostream & os_, const HashTbl & ht_ ) {
                // Run through all elements
                for (size_t i{0}; i < ht_.m_size; i++) {
                    auto element = ht_.m_table[i].begin(); // First element of collision list i.
                    auto sz = ht_.m_table[i].size(); // Size of collision list i.
                    for (size_t j{0}; j < sz; j++) {
                        os_ << *element << std::endl;
                        element++; // Next element of collision list i.
                    }
                }
                return os_;
            }

        private:
            bool is_prime(size_type n);
            size_type find_next_prime(size_type N);
            void rehash( void );

        private:
            size_type m_size; //!< Tamanho da tabela.
            size_type m_count; //!< Numero de elementos na tabela.
            float m_max_load_factor = 1.0; //!< Fator de carga da tabela.
            std::unique_ptr<list_type[]> m_table;
            //std::list< entry_type > *mpDataTable; //!< Tabela de listas para entradas de tabela.
            static const short DEFAULT_SIZE = 10;
    };

} // MyHashTable
#include "hashtbl.inl"
#endif
