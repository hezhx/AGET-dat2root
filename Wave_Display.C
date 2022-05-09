// draw 4 waves from .dat.root
// written by He Zhixuan, hezhx21@lzu.edu.cn

// 0~500 cells chosen from 0~512

#include <iostream>

using namespace std;

int Wave_Display(const char *rootfile, int ev_ID){
    TFile *f1 = new TFile(rootfile);

    Int_t event;
    Int_t time;
    Int_t board;
    Int_t chip;
    Int_t channel;
    Double_t baseline;
    Double_t adc[501];

    Double_t x[500] = {0};
    Double_t adc_code[500] = {0};
    Double_t adc_code2[500] = {0};
    Double_t adc_code3[500] = {0};
    Double_t adc_code4[500] = {0};

    TTree *T;
    T = (TTree *)f1->Get("T");
    T->SetBranchAddress("event", &event);
    T->SetBranchAddress("time", &time);
    T->SetBranchAddress("board", &board);
    T->SetBranchAddress("chip", &chip);
    T->SetBranchAddress("channel", &channel);
    T->SetBranchAddress("baseline", &baseline);
    T->SetBranchAddress("adc", adc);

    Int_t nentries = T->GetEntries();

    cout << rootfile << " : " << nentries << endl;

    for (Int_t j = ev_ID; j <= ev_ID; j++)
    {
        T->GetEntry(j);
        // cout << baseline << "\n";
        // cout << adc[1] << endl;
        for(int i = 0; i < 500; i++){
            adc_code[i] = adc[i] - baseline;
            // cout << adc[i] << endl;
        }
    }

    for (Int_t j = ev_ID; j <= ev_ID; j++)
    {
        T->GetEntry(j+1);
        // cout << adc[1] << endl;
        for(int i = 0; i < 500; i++){
            adc_code2[i] = adc[i] - baseline;
            // cout << adc[i] << endl;
        }
    }

    for (Int_t j = ev_ID; j <= ev_ID; j++)
    {
        T->GetEntry(j+2);
        // cout << adc[1] << endl;
        for(int i = 0; i < 500; i++){
            adc_code3[i] = adc[i] - baseline;
            // cout << adc[i] << endl;
        }
    }

    for (Int_t j = ev_ID; j <= ev_ID; j++)
    {
        T->GetEntry(j+3);
        // cout << adc[1] << endl;
        for(int i = 0; i < 500; i++){
            adc_code4[i] = adc[i] - baseline;
            // cout << adc[i] << endl;
        }
    }

    for(int i = 0; i < 500; i++){
        x[i] = i + 1;
    }

    int n0 = 500;

    TCanvas *c1 = new TCanvas("c1", "Wave Display", 1200, 1200);
    // c1->Divide(2,2);
    c1->cd();
    // c1->cd(1);
    TGraph *gr1 = new TGraph(n0, x, adc_code);
    // gr1->SetTitle("Wave Display (event ID)");
    gr1->SetTitle("Wave Display");
    gr1->GetXaxis()->SetTitle("cell number");
    gr1->GetXaxis()->CenterTitle();
    gr1->GetYaxis()->SetTitle("ADC code");
    gr1->GetYaxis()->CenterTitle();
    gr1->SetLineWidth(4);
    gr1->SetMarkerColor(2);
    gr1->SetLineColor(2);
    // gr1->Draw("");

    // c1->cd(2);
    TGraph *gr2 = new TGraph(n0, x, adc_code2);

    // gr2->SetTitle("Wave Display (event ID+1)");
    // gr2->GetXaxis()->SetTitle("cell number");
    // gr2->GetXaxis()->CenterTitle();
    // gr2->GetYaxis()->SetTitle("ADC code");
    // gr2->GetYaxis()->CenterTitle();
    gr2->SetLineWidth(4);    
    gr2->SetMarkerColor(3);
    gr2->SetLineColor(3);
    // gr2->Draw("");

    // c1->cd(3);
    TGraph *gr3 = new TGraph(n0, x, adc_code3);
    // gr3->SetTitle("Wave Display (event ID+2)");
    // gr3->GetXaxis()->SetTitle("cell number");
    // gr3->GetXaxis()->CenterTitle();
    // gr3->GetYaxis()->SetTitle("ADC code");
    // gr3->GetYaxis()->CenterTitle();
    gr3->SetLineWidth(4);
    gr3->SetMarkerColor(9);
    gr3->SetLineColor(9);
    // gr3->Draw("");

    // c1->cd(4);
    TGraph *gr4 = new TGraph(n0, x, adc_code4);
    // gr4->SetTitle("Wave Display (event ID+3)");
    // gr4->GetXaxis()->SetTitle("cell number");
    // gr4->GetXaxis()->CenterTitle();
    // gr4->GetYaxis()->SetTitle("ADC code");
    // gr4->GetYaxis()->CenterTitle();
    gr4->SetLineWidth(4);
    gr4->SetMarkerColor(7);
    gr4->SetLineColor(7);
    // gr4->Draw("");

    // TMultiGraph *mg = new TMultiGraph();
    // mg->SetTitle("Wave Display");
    // mg->GetXaxis()->SetTitle("cell number");
    // mg->GetXaxis()->CenterTitle();
    // mg->GetYaxis()->SetTitle("ADC code");
    // mg->GetYaxis()->CenterTitle();
    // cout << "d\n";

    // mg->Add(gr1);
    // mg->Add(gr2);
    // mg->Add(gr3);
    // mg->Add(gr4);
    // mg->Draw("ALP");
    gr1->Draw("ALP");
    gr2->Draw("same");
    gr3->Draw("same");
    gr4->Draw("same");
    return 1;
}
