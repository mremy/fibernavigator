/////////////////////////////////////////////////////////////////////////////
// Name:            ClusteringWindow.h
// Author:          ---
// Creation Date:   ---
//
// Description: This is the implementation file for the Clustering window.
/////////////////////////////////////////////////////////////////////////////


#ifndef CLUSTERINGWINDOW_H_
#define CLUSTERINGWINDOW_H_

#include "MainFrame.h"
#include "MainCanvas.h"
#include "MyListCtrl.h"

//#include "../mis/Algorithms/Helper.h"

#include <wx/scrolwin.h>

class MainFrame;
class wxToggleButton;

class ClusteringWindow: public wxScrolledWindow
{
public:
    ClusteringWindow(){};
    ClusteringWindow( wxWindow *pParent, MainFrame *pMf, wxWindowID id, const wxPoint &pos, const wxSize &size );

    ~ClusteringWindow(){};
    void OnPaint( wxPaintEvent &event );
    void OnSize( wxSizeEvent &event );
    wxSizer* getWindowSizer();
    
    void doClustering( vector<Fibers*> selectedFibers, wxString basename );

    void OnClusterFile                         ( wxCommandEvent& event );
    void OnClusterSelected                     ( wxCommandEvent& event );
    void OnClusterVisible                      ( wxCommandEvent& event );

    void OnSliderThresholdMoved                ( wxCommandEvent& event );
    void OnSliderWeightMDFMoved                ( wxCommandEvent& event );
    void OnSliderWeightGeoMoved                ( wxCommandEvent& event );
    void OnSliderWeightOrientationMoved        ( wxCommandEvent& event );
    void OnSliderWeightSpatialMoved            ( wxCommandEvent& event );
    void OnSliderWeightLengthMoved             ( wxCommandEvent& event );
    void OnSliderMinNbStreamlinesMoved         ( wxCommandEvent& event );
    
    void OnTextBoxThresholdFocusLost           ( wxFocusEvent& event );
    void OnTextBoxWeightMDFFocusLost           ( wxFocusEvent& event );
    void OnTextBoxWeightGeoFocusLost           ( wxFocusEvent& event );
    void OnTextBoxWeightOrientationFocusLost   ( wxFocusEvent& event );
    void OnTextBoxWeightSpatialFocusLost       ( wxFocusEvent& event );
    void OnTextBoxWeightLengthFocusLost        ( wxFocusEvent& event );
    void OnTextBoxMinNbStreamlinesFocusLost    ( wxFocusEvent& event );

private:
    MainFrame           *m_pMainFrame;

    wxButton            *m_pBtnCluster;
    //wxButton            *m_pBtnMergeVisible;
    wxButton            *m_pBtnClusterVisible;
    wxButton            *m_pBtnClusterSelected;

    wxSlider            *m_pSliderThreshold;
    wxTextCtrl          *m_pTxtThresholdBox;
    wxStaticText        *m_pTextThreshold;

    wxSlider            *m_pSliderWeightMDF;
    wxTextCtrl          *m_pTxtWeightMDFBox;
    wxStaticText        *m_pTextWeightMDF;

    wxSlider            *m_pSliderWeightGeo;
    wxTextCtrl          *m_pTxtWeightGeoBox;
    wxStaticText        *m_pTextWeightGeo;

    wxSlider            *m_pSliderWeightOrientation;
    wxTextCtrl          *m_pTxtWeightOrientationBox;
    wxStaticText        *m_pTextWeightOrientation;

    wxSlider            *m_pSliderWeightSpatial;
    wxTextCtrl          *m_pTxtWeightSpatialBox;
    wxStaticText        *m_pTextWeightSpatial;

    wxSlider            *m_pSliderWeightLength;
    wxTextCtrl          *m_pTxtWeightLengthBox;
    wxStaticText        *m_pTextWeightLength;

    wxSlider            *m_pSliderMinNbStreamlines;
    wxTextCtrl          *m_pTxtMinNbStreamlinesBox;
    wxStaticText        *m_pTextMinNbStreamlines;
    //wxButton            *m_pBtnSelectFile;
    //wxButton            *m_pBtnSelectShell;
    //wxToggleButton      *m_pToggleShell;
    //wxButton            *m_pBtnSelectMap;
    //wxButton            *m_pBtnConvert;
    //wxBitmapButton      *m_pPlayPause;
    //wxBitmapButton      *m_pBtnStop;
    //wxImage             m_bmpPause;
    //wxImage             m_bmpPlay;
    


private:
    wxSizer *m_pClusteringSizer;
    ClusteringWindow( wxWindow *pParent, wxWindowID id, const wxPoint &pos, const wxSize &size );
    DECLARE_DYNAMIC_CLASS( ClusteringWindow )
    DECLARE_EVENT_TABLE()
};

#endif /*CLUSTERINGWINDOW_H_*/
