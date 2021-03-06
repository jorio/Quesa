/*  NAME:
        OptimizedTriMeshElement.cpp

    DESCRIPTION:
        Source for a custom element to hold an optimized version of a TriMesh.

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

//=============================================================================
//      Include files
//-----------------------------------------------------------------------------

#include "OptimizedTriMeshElement.h"



//=============================================================================
//      Internal constants
//-----------------------------------------------------------------------------
namespace
{
	const char*		kCacheOptimizedTriMeshElementClassName	=
								"Quesa:IR:OptTriMeshCache";

	TQ3ElementType	sCacheOptimizedTriMeshElementType		= 0;
}



//=============================================================================
//      Internal types
//-----------------------------------------------------------------------------
struct TQ3CacheOptimizedTriMeshElementData
{
	TQ3GeometryObject	optimizedGeom;
	TQ3Uns32			editIndex;
};



//=============================================================================
//      Internal functions
//-----------------------------------------------------------------------------

/*!
	@function	ElementCopyAdd
	@abstract	CopyAdd and CopyGet method for cache element.
*/
static TQ3Status
ElementCopyAdd( const TQ3CacheOptimizedTriMeshElementData* inFromAPIElement,
						TQ3CacheOptimizedTriMeshElementData* ioToInternal )
{
	if (inFromAPIElement->optimizedGeom == nullptr)
	{
		ioToInternal->optimizedGeom = nullptr;
	}
	else
	{
		ioToInternal->optimizedGeom = Q3Shared_GetReference( inFromAPIElement->optimizedGeom );
	}
	ioToInternal->editIndex = inFromAPIElement->editIndex;

	return kQ3Success;
}





/*!
	@function	ElementCopyReplace
	@abstract	CopyReplace method for cache element.
*/
static TQ3Status
ElementCopyReplace( const TQ3CacheOptimizedTriMeshElementData* inFromAPIElement,
						TQ3CacheOptimizedTriMeshElementData* ioToInternal )
{
	if (ioToInternal->optimizedGeom != nullptr)
	{
		Q3Object_Dispose( ioToInternal->optimizedGeom );
	}

	if (inFromAPIElement->optimizedGeom == nullptr)
	{
		ioToInternal->optimizedGeom = nullptr;
	}
	else
	{
		ioToInternal->optimizedGeom = Q3Shared_GetReference( inFromAPIElement->optimizedGeom );
	}
	ioToInternal->editIndex = inFromAPIElement->editIndex;

	return kQ3Success;
}





/*!
	@function	ElementCopyDuplicate
	@abstract	Duplicate method for cache element.
*/
static TQ3Status
ElementCopyDuplicate( const TQ3CacheOptimizedTriMeshElementData* inFromInternal,
						TQ3CacheOptimizedTriMeshElementData* ioToInternal )
{
#pragma unused( inFromInternal )

	// If we duplicate an object, just clear the cache.
	ioToInternal->optimizedGeom = nullptr;
	ioToInternal->editIndex = 0;

	return kQ3Success;
}





/*!
	@function	ElementDelete
	@abstract	Delete method for cache element.
*/
static TQ3Status
ElementDelete( TQ3CacheOptimizedTriMeshElementData* ioInternal )
{
	if (ioInternal->optimizedGeom != nullptr)
	{
		Q3Object_Dispose( ioInternal->optimizedGeom );
		ioInternal->optimizedGeom = nullptr;
	}

	return kQ3Success;
}





/*!
	@function	ElementMetahandler
	@abstract	Metahandler for cache element.
*/
static TQ3XFunctionPointer
ElementMetahandler( TQ3XMethodType methodType )
{
	TQ3XFunctionPointer		theMethod = nullptr;
	
	switch (methodType)
	{
		case kQ3XMethodTypeElementCopyAdd:
		case kQ3XMethodTypeElementCopyGet:
			theMethod = (TQ3XFunctionPointer) ElementCopyAdd;
			break;

		case kQ3XMethodTypeElementCopyReplace:
			theMethod = (TQ3XFunctionPointer) ElementCopyReplace;
			break;

		case kQ3XMethodTypeElementCopyDuplicate:
			theMethod = (TQ3XFunctionPointer) ElementCopyDuplicate;
			break;

		case kQ3XMethodTypeElementDelete:
			theMethod = (TQ3XFunctionPointer) ElementDelete;
			break;

		case kQ3XMethodTypeObjectClassVersion:
			theMethod = (TQ3XFunctionPointer)0x01008000;
			break;
		
		// We deliberately do not support reading/writing methods, as this element
		// should not be stored in 3DMF.
	}
	
	return theMethod;
}





/*!
	@function	RegisterElement
	@abstract	Register function for cache element.
*/
static void RegisterElement()
{
	Q3XElementClass_Register(
			&sCacheOptimizedTriMeshElementType,
			kCacheOptimizedTriMeshElementClassName,
			sizeof(TQ3CacheOptimizedTriMeshElementData),
			ElementMetahandler );
}



//=============================================================================
//      Public function implementations
//-----------------------------------------------------------------------------

/*!
	@function	GetCachedOptimizedTriMesh
	@abstract	Get a reference to the optimized TriMesh attached to another
				TriMesh.
	@discussion	There are several possible states:
	
				1. We have never recorded an optimized TriMesh on this TriMesh.
				2. An optimized TriMesh has been set but is stale.
				3. We recorded nullptr as the optimized TriMesh, meaning that
					the TriMesh was already optimized.
				4. We recorded a non-nullptr optimized TriMesh and it is up to
					date.
				
				The output parameter outWasValid is true in cases (3) and (4).
	@param		inTriMesh		A TriMesh object.
	@param		outWasValid		Receives true if the cache was set and is not
								stale.
	@result		A reference to an optimized TriMesh, or nullptr.
*/
CQ3ObjectRef	GetCachedOptimizedTriMesh( TQ3GeometryObject inTriMesh,
											bool& outWasValid )
{
	CQ3ObjectRef	resultRef;
	
	if (sCacheOptimizedTriMeshElementType == 0)
	{
		RegisterElement();
	}
	
	outWasValid = false;
	
	if (inTriMesh != nullptr)
	{
		TQ3CacheOptimizedTriMeshElementData	theData;
		
		TQ3Status	theStatus = Q3Shape_GetElement( inTriMesh,
			sCacheOptimizedTriMeshElementType, &theData );
		
		if (theStatus == kQ3Success)
		{
			CQ3ObjectRef	geomRef( theData.optimizedGeom );
			TQ3Uns32	curIndex = Q3Shared_GetEditIndex( inTriMesh );
			
			if (theData.editIndex == curIndex) // cache up to date?
			{
				resultRef = geomRef;
				outWasValid = true;
			}
		}
	}
	
	return resultRef;
}

/*!
	@function	SetCachedOptimizedTriMesh
	@abstract	Set or replace the optimized TriMesh attached to another
				TriMesh.
	@param		ioTriMesh		A TriMesh object.
	@param		inOptimized		An optimized TriMesh.
*/
void			SetCachedOptimizedTriMesh( TQ3GeometryObject ioTriMesh,
										TQ3GeometryObject inOptimized )
{
	if (sCacheOptimizedTriMeshElementType == 0)
	{
		RegisterElement();
	}

	// Lock the edit index, so that adding an element won't change it.
	StLockEditIndex lockIndex( ioTriMesh );

	TQ3Uns32	editIndex = Q3Shared_GetEditIndex( ioTriMesh );
	TQ3CacheOptimizedTriMeshElementData	theData = {
		inOptimized,
		editIndex
	};
	
	TQ3Status	theStatus =
	Q3Object_AddElement( ioTriMesh, sCacheOptimizedTriMeshElementType, &theData );
	Q3_ASSERT( theStatus == kQ3Success );
}
