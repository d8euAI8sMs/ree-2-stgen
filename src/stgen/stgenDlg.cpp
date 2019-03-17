// stgenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "stgen.h"
#include "stgenDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CstgenDlg dialog

CstgenDlg::CstgenDlg(CWnd* pParent /*=NULL*/)
    : CSimulationDialog(CstgenDlg::IDD, pParent)
    , m_data(model::make_model_data())
    , m_gencnt(100)
    , m_nWnd(300)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_gencfg = m_gen.get_cfg();
}

void CstgenDlg::DoDataExchange(CDataExchange* pDX)
{
    CSimulationDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PLOT, m_plot);
    DDX_Text(pDX, IDC_EDIT1, m_gencfg.mag);
    DDX_Text(pDX, IDC_EDIT2, m_gencfg.freq);
    DDX_Text(pDX, IDC_EDIT3, m_gencfg.dfreq);
    DDX_Text(pDX, IDC_EDIT4, m_gencfg.per);
    DDX_Text(pDX, IDC_EDIT7, m_gencfg.dt);
    DDX_Text(pDX, IDC_EDIT5, m_gencnt);
    DDX_Text(pDX, IDC_EDIT6, m_nWnd);
}

BEGIN_MESSAGE_MAP(CstgenDlg, CSimulationDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CstgenDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

// CstgenDlg message handlers

BOOL CstgenDlg::OnInitDialog()
{
    CSimulationDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here

    m_plot.plot_layer.with(
        model::make_root_drawable(
            m_data.sine_data.plot.viewport_mapper,
            {{ m_data.sine_data.plot.view }}));

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CstgenDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CSimulationDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CstgenDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CstgenDlg::OnBnClickedButton1()
{
    UpdateData(TRUE);

    m_gen.next(m_gencfg, m_gencnt, [this] (size_t i, geom::point2d_t v) {
        m_data.sine_data.plot.data->emplace_back(v);
    });
    if ((m_data.sine_data.plot.data->back().x - m_data.sine_data.plot.data->front().x) > m_nWnd)
    {
        double thres = m_data.sine_data.plot.data->back().x - m_nWnd;
        size_t i = 0;
        while (m_data.sine_data.plot.data->at(i).x < thres) ++i;
        m_data.sine_data.plot.data->erase(
            m_data.sine_data.plot.data->begin(),
            m_data.sine_data.plot.data->begin() + i);
    }
    m_data.sine_data.plot.refresh();

    m_plot.RedrawWindow();
}
