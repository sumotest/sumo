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

/// @name FXTextFields
/// @{
    /// @brief text field extended over GNEFrame with thick frame
    #define GNEDesignTextField              (FRAME_THICK | LAYOUT_FILL_X)

    /// @brief text field extended over the matrix column with thick frame and limited to integers
    #define GNEDesignTextFieldAttributeInt  (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | TEXTFIELD_INTEGER)

    /// @brief text field extended over the matrix column with thick frame and limited to real
    #define GNEDesignTextFieldAttributeReal (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X | TEXTFIELD_REAL)

    /// @brief text field extended over the matrix column with thick frame
    #define GNEDesignTextFieldAttributeStr  (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X)
/// @}

/// @name FXButtons
/// @{
    /// @brief button extended over over GNEFrame with thick and raise frame 
    #define GNEDesignButton       (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X)

    /// @brief button with size limited by text, centered over GNEFrame with thick and raise frame 
    #define GNEDesignButtonDialog (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | LAYOUT_CENTER_X | JUSTIFY_NORMAL)

    /// @brief little button for help in frames
    #define GNEDesignButtonLittle (FRAME_THICK | FRAME_RAISED | ICON_BEFORE_TEXT | JUSTIFY_NORMAL)
/// @}

/// @name FXCheckBoxs
/// @{
    /// @brief Check button for GNEFrames extended over the frame
    #define GNEDesignCheckButton          (LAYOUT_LEFT | ICON_BEFORE_TEXT)
    
    /// @brief Check button for attributes extended over the matrix column
    #define GNEDesignCheckButtonAttribute (LAYOUT_LEFT | LAYOUT_FILL_COLUMN | ICON_BEFORE_TEXT)
/// @}

/// @name FXLabels
/// @{
    /// @brief label extended over over GNEFrame without thick
    #define GNEDesignLabel          (JUSTIFY_LEFT | LAYOUT_FILL_X)

    /// @brief label extended over over GNEFrame with thick
    #define GNEDesignLabelThick     (FRAME_THICK | JUSTIFY_LEFT | LAYOUT_FILL_X)

    /// @brief label extended over the matrix column with thick frame
    #define GNEDesignLabelAttribute (FRAME_THICK | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X)
/// @}

/// @name FXPackers
/// @{
    /// @brief Horizontal design extended over frame
    #define GNEDesignHorizontalFrame        (LAYOUT_FILL_X | LAYOUT_LEFT)
    
    /// @brief Matrix design extended over frame
    #define GNEDesignMatrix                 (LAYOUT_FILL_X | MATRIX_BY_COLUMNS | PACK_UNIFORM_WIDTH)
    
    /// @brief Group box design extended over frame
    #define GNEDesignGroupBox               (LAYOUT_FILL_X | FRAME_GROOVE | GROUPBOX_TITLE_CENTER)

    /// @brief Horizontal separator
    #define GNEDesignHorizontalSeparator    (SEPARATOR_GROOVE | LAYOUT_FILL_X), 0, 0, 0, 2, 2, 2, 4, 4
/// @}


/// @brief Combo box static (cannot be dited) extended over the matrix column or all frame with thick frame and limited to integers
#define GNEDesignComboBox               (FRAME_THICK | LAYOUT_FILL_COLUMN| LAYOUT_FILL_X | COMBOBOX_STATIC)

#define GNEDesignRadioButton            (RADIOBUTTON_NORMAL)

#define GNEDesignDialogBox              (DECOR_CLOSE | DECOR_TITLE)

#define GNEDesignList                   (FRAME_NORMAL | LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT | LIST_NORMAL)

#define GNEDesignDial                   (LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK | LAYOUT_FILL_Y)

#endif

/****************************************************************************/
