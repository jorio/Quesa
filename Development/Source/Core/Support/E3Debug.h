/*  NAME:
        E3Debug.h

    DESCRIPTION:
        Debugging routines.

    COPYRIGHT:
        Copyright (c) 1999-2018, Quesa Developers. All rights reserved.

        For the current release of Quesa, please see:

            <https://github.com/jwwalker/Quesa>
        
        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions
        are met:
        
            o Redistributions of source code must retain the above copyright
              notice, this list of conditions and the following disclaimer.
        
            o Redistributions in binary form must reproduce the above
              copyright notice, this list of conditions and the following
              disclaimer in the documentation and/or other materials provided
              with the distribution.
        
            o Neither the name of Quesa nor the names of its contributors
              may be used to endorse or promote products derived from this
              software without specific prior written permission.
        
        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
        OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
        TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
        PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
        LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
        NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
        SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    ___________________________________________________________________________
*/
#ifndef E3DEBUG_HDR
#define E3DEBUG_HDR
//=============================================================================
//		C++ preamble
//-----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __has_attribute
	#define __has_attribute(x) 0  // Compatibility with non-clang compilers.
#endif

#if __has_attribute(noreturn)
	#define	Q3_NORETURN		__attribute__((noreturn))
#else
	#define	Q3_NORETURN
#endif


//=============================================================================
//      Function prototypes
//-----------------------------------------------------------------------------
// Report failed assertions
void		E3Assert(const char *srcFile, TQ3Uns32 lineNum,
					const char *theAssertion) Q3_NORETURN;


// Check a pointer for validity
TQ3Boolean	E3IsValidPtr(const void *thePtr);


// Write something to a log file
void		E3LogMessage( const char* inMessage );


// Close the log file.
void		E3CloseLog( void );

/*!
	@function	E3GetLogStream
	
	@abstract	Create or retrieve a file stream for a log file.
	
	@discussion	The log will be created in the user's Documents folder.
	
	@param		inCreate	Whether to create and open the log stream if it is
							not already open.
	
	@result		A stream for the log.
*/
FILE*	E3GetLogStream( bool inCreate );



//=============================================================================
//		Misc macros
//-----------------------------------------------------------------------------
// Check to see if a pointer is valid
#if Q3_DEBUG
	#define Q3_VALID_PTR(_thePtr)				E3IsValidPtr((const void *) (_thePtr))
#else
	#define Q3_VALID_PTR(_thePtr)				(_thePtr != nullptr)
#endif

// Write to the log file.  Unlike Q3_MESSAGE_FMT, this writes even if Q3_DEBUG is off.
#define	Q3_LOG_FMT(...)		do {	\
								char msg_[1024];	\
								snprintf( msg_, sizeof(msg_), __VA_ARGS__ );	\
								strncat( msg_, "\n", 1023 );	\
								msg_[1023] = '\0';  \
								E3LogMessage( msg_ );	\
							} while (0)




//=============================================================================
//		Assertion and message macros
//-----------------------------------------------------------------------------
	// Generate an assertion error if a test fails
	#define Q3_ASSERT(_theTest)													\
				((_theTest) ?													\
					((void) 0) :												\
					E3Assert(__FILE__, __LINE__, #_theTest)						\
				)


	#define Q3_ASSERT_FMT( _condition, ... ) \
		do { \
			if ( ! (_condition) ) \
			{ \
				char _xBuf[1024]; \
				char _yBuf[1024]; \
				snprintf( _yBuf, sizeof(_yBuf), "" __VA_ARGS__ ); \
				snprintf( _xBuf, sizeof(_xBuf), "%s %s", #_condition, _yBuf ); \
				E3Assert( __FILE__, __LINE__, _xBuf );					\
			} \
		} while (false)

	#define	Q3_ASSERT_MESSAGE(_condition, _message)								\
				((_condition) ?													\
					((void) 0) :												\
					E3Assert(__FILE__, __LINE__, (_message))					\
				)

	// Generates an assertion error for a bad pointer
	#define Q3_ASSERT_VALID_PTR(_thePtr)										\
				Q3_ASSERT(Q3_VALID_PTR(_thePtr))

#if Q3_DEBUG
	
	#define	Q3_MESSAGE( _msg )	E3LogMessage( _msg )
	
	#define	Q3_MESSAGE_FMT(...)		do {	\
										char msg_[1024];	\
										snprintf( msg_, sizeof(msg_), __VA_ARGS__ );	\
										strncat( msg_, "\n", 1 );	\
										E3LogMessage( msg_ );	\
									} while (0)


	#if Q3_VALIDATE_DRAW_ELEMENTS
		#define	Q3_CHECK_DRAW_ELEMENTS( _numPts, _numIndices, _indexArray )	\
			GLUtils_ValidateElements( (_numPts), (_numIndices), (_indexArray) )
	#else
		#define	Q3_CHECK_DRAW_ELEMENTS( _numPts, _numIndices, _indexArray )
	#endif
#else
	#define	Q3_MESSAGE( _msg )
	
	#define	Q3_MESSAGE_FMT(...)

	#define	Q3_CHECK_DRAW_ELEMENTS( _numPts, _numIndices, _indexArray )
#endif





//=============================================================================
//		Requirement macros
//-----------------------------------------------------------------------------
#if Q3_DEBUG

// Check a condition or return
#define Q3_REQUIRE(_theTest)													\
			do																	\
				{																\
				TQ3Boolean	_cond = (TQ3Boolean)(_theTest); 					\
				Q3_ASSERT_MESSAGE(_cond, #_theTest);							\
				if (!(_cond))													\
					return;														\
				}																\
			while (0)

// Check a condition or return a value
#define Q3_REQUIRE_OR_RESULT(_theTest, _theResult)								\
			do																	\
				{																\
				TQ3Boolean	_cond = (TQ3Boolean)(_theTest); 					\
				Q3_ASSERT_MESSAGE(_cond, #_theTest);							\
				if (!(_cond))													\
					return(_theResult);											\
				}																\
			while (0)
			
#else

// Check a condition or return
#define Q3_REQUIRE(_theTest)													\
			do																	\
				{																\
				if (!((TQ3Boolean)(_theTest)))													\
					return;														\
				}																\
			while (0)

// Check a condition or return a value
#define Q3_REQUIRE_OR_RESULT(_theTest, _theResult)								\
			do																	\
				{																\
				if (!((TQ3Boolean)(_theTest)))													\
					return(_theResult);											\
				}																\
			while (0)
			
#endif





//=============================================================================
//		C++ postamble
//-----------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif



#endif

