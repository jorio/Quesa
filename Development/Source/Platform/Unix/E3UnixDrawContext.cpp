/*  NAME:
        E3UnixDrawContext.c

    DESCRIPTION:
        Unix specific Draw Context calls.

    COPYRIGHT:
        Copyright (c) 1999-2005, Quesa Developers. All rights reserved.

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
#include "E3Prefix.h"
#include "E3DrawContext.h"
#include "E3Main.h"





//=============================================================================
//      Internal types
//-----------------------------------------------------------------------------



class E3XDrawContext : public E3DrawContext  // This is a leaf class so no other classes use this,
								// so it can be here in the .c file rather than in
								// the .h file, hence all the fields can be public
								// as nobody should be including this file
	{
	Q3_CLASS_ENUMS ( kQ3DrawContextTypeX11, E3XDrawContext, E3DrawContext )

public :

	TQ3DrawContextUnionData				instanceData ;
	} ;
	


//=============================================================================
//      Internal functions
//-----------------------------------------------------------------------------
//      e3drawcontext_x_new : X11 draw context new method.
//-----------------------------------------------------------------------------
static TQ3Status
e3drawcontext_x_new(TQ3Object theObject, void *privateData, const void *paramData)
{	TQ3DrawContextUnionData			*instanceData = (TQ3DrawContextUnionData *) privateData;
	const TQ3XDrawContextData		*x11Data      = (const TQ3XDrawContextData *) paramData;



	// Initialise our instance data
	instanceData->data.x11Data.theData = *x11Data;

	E3DrawContext_InitialiseData(&instanceData->data.x11Data.theData.contextData);

	return(kQ3Success);
}





//=============================================================================
//      e3drawcontext_x_get_dimensions : X11 draw context dimensions.
//-----------------------------------------------------------------------------
static void
e3drawcontext_x_get_dimensions(TQ3DrawContextObject theDrawContext, TQ3Area *thePane)
{
	// Return our dimensions (not currently implemented on X11)
	thePane->min.x = 0.0f;
	thePane->min.y = 0.0f;
	thePane->max.x = 0.0f;
	thePane->max.y = 0.0f;
}





//=============================================================================
//      e3drawcontext_x_metahandler : X11 draw context metahandler.
//-----------------------------------------------------------------------------
static TQ3XFunctionPointer
e3drawcontext_x_metahandler(TQ3XMethodType methodType)
{	TQ3XFunctionPointer		theMethod = NULL;



	// Return our methods
	switch (methodType) {
		case kQ3XMethodTypeObjectNew:
			theMethod = (TQ3XFunctionPointer) e3drawcontext_x_new;
			break;

		case kQ3XMethodTypeDrawContextGetDimensions:
			theMethod = (TQ3XFunctionPointer) e3drawcontext_x_get_dimensions;
			break;
		}
	
	return(theMethod);
}





//=============================================================================
//      Public functions
//-----------------------------------------------------------------------------
//      E3XDrawContext_RegisterClass : Register the class.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_RegisterClass(void)
{	TQ3Status		qd3dStatus;



	// Register the class
	qd3dStatus = Q3_REGISTER_CLASS(kQ3ClassNameDrawContextX11,
											e3drawcontext_x_metahandler,
											E3XDrawContext);

	return(qd3dStatus);
}





//=============================================================================
//      E3XDrawContext_UnregisterClass : Unregister the class.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_UnregisterClass(void)
{	TQ3Status		qd3dStatus;



	// Unregister the classes
	qd3dStatus = E3ClassTree::UnregisterClass(kQ3DrawContextTypeX11, kQ3True);

	return(qd3dStatus);
}





//=============================================================================
//      E3MacDrawContext_New : Create a new X11 draw context.
//-----------------------------------------------------------------------------
TQ3DrawContextObject
E3XDrawContext_New(const TQ3XDrawContextData *drawContextData)
{	TQ3Object		theObject;



	// Create the object
	theObject = E3ClassTree::CreateInstance(kQ3DrawContextTypeX11, kQ3False, drawContextData);

	return(theObject);
}





//=============================================================================
//      E3XDrawContext_NewWithWindow : Create a new X11 Draw Context object.
//-----------------------------------------------------------------------------
TQ3DrawContextObject
E3XDrawContext_NewWithWindow(TQ3ObjectType drawContextType, void *drawContextTarget)
{	TQ3XDrawContextData		x11DrawContextData;
	TQ3DrawContextData		drawContextData;
	TQ3DrawContextObject	drawContext;



	// Check we have a suitable target for the draw context
	if (drawContextType != kQ3DrawContextTypeX11)
		return(NULL);



	// Prepare the draw context
	Q3ColorARGB_Set(&drawContextData.clearImageColor, kQ3DrawContextDefaultBackgroundColour);

	drawContextData.clearImageMethod  = kQ3ClearMethodWithColor;
	drawContextData.paneState         = kQ3False;
	drawContextData.maskState		  = kQ3False;
	drawContextData.doubleBufferState = kQ3True;
	drawContextData.pane.min.x        = 0.0f;
	drawContextData.pane.min.y        = 0.0f;
	drawContextData.pane.max.x        = 0.0f;
	drawContextData.pane.max.y        = 0.0f;

	Q3Memory_Clear(&x11DrawContextData, sizeof(x11DrawContextData));
	x11DrawContextData.contextData  = drawContextData;
	x11DrawContextData.visual       = (Visual *) drawContextTarget;
	


	// Create the draw context
	drawContext = Q3XDrawContext_New(&x11DrawContextData);

	return(drawContext);
}





//=============================================================================
//      E3XDrawContext_SetDisplay : Set the display for an X11 draw context.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_SetDisplay(TQ3DrawContextObject drawContext, const Display *display)
	{
	// Set the field and reset our flag. We don't compare the current
	// state, and assume that setting a new display may cause a rebuild.
	( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.display = (Display *) display;
	( (E3XDrawContext*) drawContext )->instanceData.theState                    |= kQ3XDrawContextValidationAll;
	Q3Shared_Edited(drawContext);

	return(kQ3Success);
	}





//=============================================================================
//      E3XDrawContext_GetDisplay : Get the display for an X11 draw context.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_GetDisplay(TQ3DrawContextObject drawContext, Display **display)
	{
	// Get the field
	*display = ( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.display;

	return(kQ3Success);
	}





//=============================================================================
//      E3XDrawContext_SetDrawable : Set the drawable for an X11 draw context.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_SetDrawable(TQ3DrawContextObject drawContext, Drawable drawable)
	{
	// Set the field and reset our flag
	if (( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.drawable != drawable)
		{
		( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.drawable = drawable;
		( (E3XDrawContext*) drawContext )->instanceData.theState                     |= kQ3XDrawContextValidationAll;
		Q3Shared_Edited(drawContext);
		}

	return(kQ3Success);
	}





//=============================================================================
//      E3XDrawContext_GetDrawable : Get the drawable for an X11 draw context.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_GetDrawable(TQ3DrawContextObject drawContext, Drawable *drawable)
	{
	// Get the field
	*drawable = ( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.drawable;

	return(kQ3Success);
	}





//=============================================================================
//      E3XDrawContext_SetVisual : Set the visual for an X11 draw context.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_SetVisual(TQ3DrawContextObject drawContext, const Visual *visual)
	{
	// Set the field and reset our flag. We don't compare the current
	// state, and assume that setting a new visual may cause a rebuild.
	( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.visual = (Visual *) visual;
	( (E3XDrawContext*) drawContext )->instanceData.theState                   |= kQ3XDrawContextValidationAll;
	Q3Shared_Edited(drawContext);

	return(kQ3Success);
	}





//=============================================================================
//      E3XDrawContext_GetVisual : Get the visual for an X11 draw context.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_GetVisual(TQ3DrawContextObject drawContext, Visual **visual)
	{
	// Get the field
	*visual = ( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.visual;

	return(kQ3Success);
	}





//=============================================================================
//      E3XDrawContext_SetColormap : Set the color map for an X11 draw context.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_SetColormap(TQ3DrawContextObject drawContext, Colormap colormap)
	{
	// Set the field and reset our flag. We don't compare the current
	// state, and assume that setting a new colormap may cause a rebuild.
	( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.cmap = colormap;
	( (E3XDrawContext*) drawContext )->instanceData.theState                 |= kQ3XDrawContextValidationAll;
	Q3Shared_Edited(drawContext);

	return(kQ3Success);
	}





//=============================================================================
//      E3XDrawContext_GetColormap : Get the color map for an X11 draw context.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_GetColormap(TQ3DrawContextObject drawContext, Colormap *colormap)
	{
	// Get the field
	*colormap = ( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.cmap;

	return(kQ3Success);
	}





//=============================================================================
//      E3XDrawContext_SetColormapData : Set the colour map data.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_SetColormapData(TQ3DrawContextObject drawContext, const TQ3XColormapData *colormapData)
	{
	// Set the field and reset our flag. We don't compare the current
	// state, and assume that setting a new colormap may cause a rebuild.
	( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.colorMapData = (TQ3XColormapData *) colormapData;
	( (E3XDrawContext*) drawContext )->instanceData.theState                         |= kQ3XDrawContextValidationAll;
	Q3Shared_Edited(drawContext);

	return(kQ3Success);
	}





//=============================================================================
//      E3XDrawContext_GetColormapData : Get the colour map data.
//-----------------------------------------------------------------------------
TQ3Status
E3XDrawContext_GetColormapData(TQ3DrawContextObject drawContext, TQ3XColormapData *colormapData)
	{
	// Get the field
	Q3Memory_Copy(( (E3XDrawContext*) drawContext )->instanceData.data.x11Data.theData.colorMapData, colormapData, sizeof(TQ3XColormapData));

	return(kQ3Success);
	}





//=============================================================================
//      E3XBuffers_New : Create a TQ3XBufferObject 'object'.
//-----------------------------------------------------------------------------
//		Note :	Unclear what the purpose of this object is for, so we just
//				create it as a simple structure for now - i.e., this, like the
//				viewer is not really a TQ3Object.
//-----------------------------------------------------------------------------
TQ3XBufferObject
E3XBuffers_New(Display *dpy, TQ3Uns32 numBuffers, Window window)
{	TQ3XBufferObject		theBuffer;



	// Allocate the 'object'
	theBuffer = (TQ3XBufferObject) Q3Memory_Allocate(sizeof(OpaqueTQ3XBufferObject));
	if (theBuffer == NULL)
		return(NULL);



	// Initialise the 'object'
	theBuffer->theDisplay = dpy;
	theBuffer->theWindow  = window;
	theBuffer->numBuffers = numBuffers;

	return(theBuffer);
}





//=============================================================================
//      E3XBuffers_Swap : Swap the buffers.
//-----------------------------------------------------------------------------
//		Note :	Currently just flushes the display, since swapping is handled
//				by the renderer under Quesa.
//-----------------------------------------------------------------------------
void
E3XBuffers_Swap(Display *dpy, TQ3XBufferObject buffers)
{


	// Just flush the display?
	XFlush(dpy);
}





//=============================================================================
//      E3X_GetVisualInfo : Get an XVisualInfo.
//-----------------------------------------------------------------------------
//		Note :	No documentation on what this function would be for, so we just
//				do what seems reasonable... We assume that the intention is to
//				return an array that will be freed by our caller with XFree.
//
//				There's no way we can return the size of the array, so I assume
//				the idea was that the first item was the one to be used.
//-----------------------------------------------------------------------------
XVisualInfo *
E3X_GetVisualInfo(Display *dpy, Screen *screen)
{	XVisualInfo		visualInfoTemplate;
	long			visualInfoMask;
	int				numberVisuals;
	XVisualInfo		*visualArray;



	// Fill in the template
	visualInfoMask            = VisualClassMask | VisualScreenMask;
#ifdef __cplusplus
	visualInfoTemplate.c_class  = PseudoColor;
#else
	visualInfoTemplate.class  = PseudoColor;
#endif
	visualInfoTemplate.screen = DefaultScreen(dpy);



	// Try and find a visual
	visualArray = XGetVisualInfo(dpy,
								 visualInfoMask,
								 &visualInfoTemplate,
								 &numberVisuals);

	return(visualArray);
}
