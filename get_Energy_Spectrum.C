// written by He Zhixuan, hezhx21@lzu.edu.cn

#include <iostream>

using namespace std;

int get_Energy_Spectrum(const char *rootfile){
    TFile *f1 = new TFile(rootfile);

    Int_t event;
    Int_t time;
    Int_t board;
    Int_t chip;
    Int_t channel;
    Double_t amplitude;
    Double_t baseline;
    Double_t adc[500];

    Double_t x[500] = {0};
    Double_t adc_code[500] = {0};

    TTree *T;
    T = (TTree *)f1->Get("T");
    T->SetBranchAddress("event", &event);
    T->SetBranchAddress("time", &time);
    T->SetBranchAddress("board", &board);
    T->SetBranchAddress("chip", &chip);
    T->SetBranchAddress("channel", &channel);
    T->SetBranchAddress("amplitude", &amplitude);
    T->SetBranchAddress("baseline", &baseline);
    T->SetBranchAddress("adc", adc);

	TH1F*h1=new TH1F("h1","Energy Sum of All Pads",512,0,20000);

    Int_t nentries = T->GetEntries();
    cout << rootfile << " : " << nentries << endl;

    Int_t nevent_begin = T->GetEntry(0);
	Int_t eventID_begin = event;
    cout << "Begin Event ID : " << eventID_begin << endl;

    Int_t nevent_end = T->GetEntry(nentries-1);
    Int_t eventID_end = event;
    cout << "End Event ID : " << eventID_end << endl;

    Int_t triggers = eventID_end-eventID_begin+1;
    cout << "Total Trigger Events : " << triggers << endl;
    Int_t onetrigger_chs = (nentries+1)/triggers;
    cout << " Channels per Trigger : " << onetrigger_chs << endl;

    // nentries = 1;
    Double_t tot_energy = 0.;
    for(Int_t i=0; i<nentries; i=i+onetrigger_chs)
    {
        tot_energy = 0.;
        T->GetEntry(i);
        // cout << event << " ";
        for (Int_t j=i; j<i+onetrigger_chs; j++)
        {
            T->GetEntry(j);
            // cout << j+1 << " " <<event << " " << amplitude << " - ";
            if(amplitude>=50.)
            {
                // cout << event << "-" 
                //      << board << "-" 
                //      << chip << "-"  
                //      << channel << "-"
                //      << amplitude << endl;
                tot_energy = tot_energy + amplitude;
                // cout << j+1 << " " <<event << " " << tot_energy << " - ";
            }
        }

        // cout << tot_energy << "-";
        h1->Fill(tot_energy);
    }

	TCanvas *c1=new TCanvas("c1","energy",600,600);
    c1->cd();
    h1->Draw();

    return 1;
}