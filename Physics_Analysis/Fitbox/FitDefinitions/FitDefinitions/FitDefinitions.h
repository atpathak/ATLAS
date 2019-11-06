/**
 * @class FitDefinitions
 * @author Z. Zinonos - zenon@cern.ch
 * @date April 2016
 */

#ifndef _FITDEFINITIONS_
#define _FITDEFINITIONS_

#include "xAODRootAccess/tools/Message.h" // xaod msg
#include "EventLoop/StatusCode.h"


/**
 *@brief assert with message
 */
#define ASSERT_WITH_MESSAGE(message, condition) \
  do {                                          \
    if (!(condition)) { printf((message)); }    \
    assert ((condition));                       \
  } while(false)


/**
 * @short Helper function to assert in case of failures
 */
#define EL_ASSERT_WITH_MESSAGE(message, status)                 \
  do {                                                          \
    const bool condition (status == EL::StatusCode::SUCCESS);   \
    if ( ! condition ) { printf((message)); }                   \
    assert ((condition));                                       \
  }while(false)

/**
 * @short Helper function to check failure returns
 */
#define EL_RETURN_CONDITION( WHERE, WHAT, COND )        \
  do {                                                  \
    const bool result = COND;                           \
    if( !result ) {                                     \
      Error( WHERE,                                     \
             XAOD_MESSAGE( "Failure: %s"),              \
             #WHAT );                                   \
      return EL::StatusCode::FAILURE;                   \
    }                                                   \
  } while( false )

/**
 * @short Helper function to check failure returns
 */
#define CHECK_FAILURE( WHERE, WHAT, ARG )                       \
  do {                                                          \
    const bool bad_result = (ARG == EL::StatusCode::FAILURE);   \
    if( bad_result ) {                                          \
      Error( WHERE,                                             \
             XAOD_MESSAGE( "Failure: %s"),                      \
             #WHAT );                                           \
      return EL::StatusCode::FAILURE;                           \
    }                                                           \
  } while( false )

/**
 * @short macro to "continue" if the logic statement is true
 */
#define EL_CONTINUE( COND )                     \
  do {                                          \
    const bool result = COND;                   \
    if( result )                                \
      return EL::StatusCode::SUCCESS;           \
  } while( false )

#endif
