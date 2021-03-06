#pragma once

#include <sigmaengine/protocol/authority.hpp>
#include <sigmaengine/protocol/operations.hpp>
#include <sigmaengine/protocol/sigmaengine_operations.hpp>

#include <sigmaengine/chain/sigmaengine_object_types.hpp>
#include <sigmaengine/chain/bobserver_objects.hpp>

#include <boost/multi_index/composite_key.hpp>


namespace sigmaengine { namespace chain {

   class operation_object : public object< operation_object_type, operation_object >
   {
      operation_object() = delete;

      public:
         template< typename Constructor, typename Allocator >
         operation_object( Constructor&& c, allocator< Allocator > a )
            :serialized_op( a.get_segment_manager() )
         {
            c( *this );
         }

         id_type              id;

         transaction_id_type  trx_id;
         uint32_t             block = 0;
         uint32_t             trx_in_block = 0;
         uint16_t             op_in_trx = 0;
         uint64_t             virtual_op = 0;
         time_point_sec       timestamp;
         buffer_type          serialized_op;
   };

   struct by_location;
   struct by_transaction_id;

   typedef multi_index_container<
      operation_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< operation_object, operation_id_type, &operation_object::id > >,
         ordered_unique< tag< by_location >,
            composite_key< operation_object,
               member< operation_object, uint32_t, &operation_object::block>,
               member< operation_object, uint32_t, &operation_object::trx_in_block>,
               member< operation_object, uint16_t, &operation_object::op_in_trx>,
               member< operation_object, uint64_t, &operation_object::virtual_op>,
               member< operation_object, operation_id_type, &operation_object::id>
            >
         >
#ifndef SKIP_BY_TX_ID
         ,
         ordered_unique< tag< by_transaction_id >,
            composite_key< operation_object,
               member< operation_object, transaction_id_type, &operation_object::trx_id>,
               member< operation_object, operation_id_type, &operation_object::id>
            >
         >
#endif
      >,
      allocator< operation_object >
   > operation_index;

   class account_history_object : public object< account_history_object_type, account_history_object >
   {
      public:
         template< typename Constructor, typename Allocator >
         account_history_object( Constructor&& c, allocator< Allocator > a )
         {
            c( *this );
         }

         id_type           id;

         account_name_type account;
         uint32_t          sequence = 0;
         uint32_t          op_tag = 0;
         uint32_t          op_seq = 0;

         operation_id_type op;
   };

   struct by_account;
   struct by_op_tag;
   typedef multi_index_container<
      account_history_object,
      indexed_by<
         ordered_unique< tag< by_id >, member< account_history_object, account_history_id_type, &account_history_object::id > >,
         ordered_unique< tag< by_account >,
            composite_key< account_history_object,
               member< account_history_object, account_name_type, &account_history_object::account>,
               member< account_history_object, uint32_t, &account_history_object::sequence>
            >,
            composite_key_compare< std::less< account_name_type >, std::greater< uint32_t > >
         >,
         ordered_unique< tag< by_op_tag >,
            composite_key< account_history_object,
               member< account_history_object, account_name_type, &account_history_object::account>,
               member< account_history_object, uint32_t, &account_history_object::op_tag>,
               member< account_history_object, uint32_t, &account_history_object::op_seq>
            >,
            composite_key_compare< std::less< account_name_type >, std::greater< uint32_t >, std::greater< uint32_t > >
         >
      >,
      allocator< account_history_object >
   > account_history_index;
} }

FC_REFLECT( sigmaengine::chain::operation_object, (id)(trx_id)(block)(trx_in_block)(op_in_trx)(virtual_op)(timestamp)(serialized_op) )
CHAINBASE_SET_INDEX_TYPE( sigmaengine::chain::operation_object, sigmaengine::chain::operation_index )

FC_REFLECT( sigmaengine::chain::account_history_object, (id)(account)(sequence)(op_tag)(op_seq)(op) )
CHAINBASE_SET_INDEX_TYPE( sigmaengine::chain::account_history_object, sigmaengine::chain::account_history_index )
