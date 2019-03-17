// stgenDlg.h : header file
//

#pragma once

#include <util/common/gui/SimulationDialog.h>
#include <util/common/gui/PlotControl.h>

#include "model.h"

// CstgenDlg dialog
class CstgenDlg : public CSimulationDialog
{
// Construction
public:
    CstgenDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    enum { IDD = IDD_STGEN_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    CPlotControl m_plot;
    model::model_data m_data;
    model::sine_generator m_gen;
    model::sine_generator::cfg_t m_gencfg;
    size_t m_gencnt;
    double m_nWnd;
    afx_msg void OnBnClickedButton1();
};
