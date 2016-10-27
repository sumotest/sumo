/****************************************************************************/
/// @file    GNEFrameDesigns.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id: GNEFrameDesigns.h 21773 2016-10-25 09:07:56Z palcraft $
///
/// File with the definitions of standard style of FXObjects in netedit
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEFrameDesigns_h
#define GNEFrameDesigns_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>

// ===========================================================================
// Definitions
// ===========================================================================
#define GNEDesignTextFieldAttributeInt  (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | TEXTFIELD_INTEGER)
#define GNEDesignTextFieldAttributeReal (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | TEXTFIELD_REAL)
#define GNEDesignTextFieldAttributeStr  (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X)

#define GNEDesignComboBox               (COMBOBOX_STATIC | FRAME_THICK | LAYOUT_FILL_COLUMN| LAYOUT_FILL_X)

#define GNEDesignButton                 (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X)
#define GNEDesignButtonDialog           (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_CENTER_X | JUSTIFY_NORMAL)
#define GNEDesignButtonLittle           (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | JUSTIFY_NORMAL)

#define GNEDesignCheckButton            (JUSTIFY_LEFT | ICON_BEFORE_TEXT  | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X)
#define GNEDesignRadioButton            (RADIOBUTTON_NORMAL)

#define GNEDesignGroupBox               (GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X)
#define GNEDesignDialogBox              (DECOR_CLOSE | DECOR_TITLE)

#define GNEDesignListBox1               (FRAME_GROOVE | LAYOUT_FILL_X) // Standard
#define GNEDesignListBox2               (FRAME_SUNKEN | FRAME_THICK | LISTBOX_NORMAL | LAYOUT_FIX_WIDTH) // must be unified
#define GNEDesignListBox3               (LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT)

#define GNEDesignLabel1                 (JUSTIFY_LEFT)
#define GNEDesignLabel2                 (JUSTIFY_LEFT | LAYOUT_FILL_X)
#define GNEDesignLabel3                 (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X)

#define GNEDesignMatrix                 (MATRIX_BY_COLUMNS)
#define GNEDesignMatrix2                (MATRIX_BY_COLUMNS | LAYOUT_FILL_X)
#define GNEDesignMatrix3                (MATRIX_BY_COLUMNS | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH)

#define GNEDesignHorizontalSeparator    (SEPARATOR_GROOVE | LAYOUT_FILL_X)

#define GNEDesignHorizontalFrame       (LAYOUT_FILL_X | LAYOUT_LEFT)

#define GNEDesignDial                   (LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_Y)
#define GNEDesignMenuCheck              (LAYOUT_LEFT | LAYOUT_FILL_X)

#endif

/****************************************************************************/
