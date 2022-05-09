// draw 4 waves from .dat.root
// written by He Zhixuan, hezhx21@lzu.edu.cn

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <TH2F.h>
#include <TH1F.h>
#include <iostream>
#include <fstream>
#include "TSpectrum.h"
#include "TF1.h"
#include "TString.h"
#include "TPolyMarker.h"
#include "TVirtualFitter.h"
#include "Functions.h"

//0~500 cells chosen from 0~512
using namespace std;

int WaveFit(const char *rootfile, int ev_ID){
    TFile *f1 = new TFile(rootfile);

    Int_t event;
    Int_t time;
    Int_t board;
    Int_t chip;
    Int_t channel;
    Double_t baseline;
    Double_t adc[513];

    Double_t x[500] = {0};
    Double_t adc_code[500] = {0};
    // Double_t adc_code2[500] = {0};
    // Double_t adc_code3[500] = {0};
    // Double_t adc_code4[500] = {0};
    TH1D *th1 = new TH1D("th1","th1",500,0,511);
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
            th1->SetBinContent(i,adc_code[i]);
            // cout << adc[i] << endl;
        }
    }

    // for (Int_t j = ev_ID; j <= ev_ID; j++)
    // {
    //     T->GetEntry(j+1);
    //     // cout << adc[1] << endl;
    //     for(int i = 0; i < 500; i++){
    //         adc_code2[i] = adc[i] - baseline;
    //         // cout << adc[i] << endl;
    //     }
    // }

    // for (Int_t j = ev_ID; j <= ev_ID; j++)
    // {
    //     T->GetEntry(j+2);
    //     // cout << adc[1] << endl;
    //     for(int i = 0; i < 500; i++){
    //         adc_code3[i] = adc[i] - baseline;
    //         // cout << adc[i] << endl;
    //     }
    // }

    // for (Int_t j = ev_ID; j <= ev_ID; j++)
    // {
    //     T->GetEntry(j+3);
    //     // cout << adc[1] << endl;
    //     for(int i = 0; i < 500; i++){
    //         adc_code4[i] = adc[i] - baseline;
    //         // cout << adc[i] << endl;
    //     }
    // }

    for(int i = 0; i < 500; i++){
        x[i] = i + 1;
    }

    int n0 = 500;

    TCanvas *c1 = new TCanvas("c1", "Wave Display", 800, 600);
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
    gr1->SetMarkerColor(9);
    gr1->SetLineColor(9);

    int mean=th1->GetMaximumBin();
    int high=th1->GetBinContent(mean);
    cout<<"th1_mean =="<<mean<<endl;
    // gr1->Draw("");

    // Find peaks
    Int_t Point_Num = 500;
    TSpectrum *spectrum = new TSpectrum();
    Double_t dest[Point_Num];
    Double_t fPositionX[100];
    Double_t fPositionY[100];
    Double_t sigma = 1;
    Double_t threshold = 5;
    bool backgroundRemove = kTRUE;
    Int_t nfound = spectrum->SearchHighRes(adc_code, dest, 
                                         Point_Num, 
                                         sigma, threshold, 
                                         backgroundRemove, 
                                         3, kTRUE, 3);
    printf("Found %d candidate peaks \n",nfound);
    Double_t *xpeaks;
    xpeaks = spectrum->GetPositionX();

    // If find n peaks, there is n circles
    for (Int_t i = 0; i < nfound; i++) 
    {
        double a = xpeaks[i];
        cout << "Find peak channel : " << a << endl;
        int bin = int(round(a)); 
        // fPositionX[i] = bin*1.0;
        fPositionX[i] = mean;
        // fPositionY[i] = adc_code[bin];
        fPositionY[i] = high;
        cout << "Peak ch - Peak amplitude : ";
        cout << fPositionX[i] << " - " << fPositionY[i] << endl;
    }

    // Put a marker in the peak
    TPolyMarker *pm = (TPolyMarker*)gr1->GetListOfFunctions()->FindObject("TPolyMarker");
    if (pm)
    {
        gr1->GetListOfFunctions()->Remove(pm);
        delete pm;
    }
    pm = new TPolyMarker(nfound, fPositionX, fPositionY);
    gr1->GetListOfFunctions()->Add(pm);
    pm->SetMarkerStyle(23);
    pm->SetMarkerColor(kRed);
    pm->SetMarkerSize(1.3);

    // Fit
    Int_t n_of_para = 5;
    Double_t par[1000];
    par[0] = 0;
    par[1] = 0;
    par[2] = 0;
    par[3] = 0; 
    par[4] = 0;

    Int_t npeaks = nfound;
    if (npeaks==1)
    {
        par[1] = 2.;
        par[2] = 10.;
        par[3] = fPositionX[0];
        cout << " fPositionX[0] "<< fPositionX[0] << endl;  
        par[4] = fPositionY[0]*400;
         cout << par[4] << endl;
    }

    AsymGuass_Npeak AsymGuassNP = AsymGuass_Npeak(1);  // create the user function class
    TF1 *fitFunction = new TF1("fitFunction",
                                &AsymGuassNP,
                                &AsymGuass_Npeak::AsymGuass_fit_Npeak,
                                fPositionX[0]-20,
                                fPositionX[0]+100,
                                // mean-20,
                                // mean+20,
                                n_of_para,
                                "AsymGuass_Npeak",
                                "AsymGuass_fit_Npeak");
    fitFunction->SetParameters(par);
    fitFunction->SetParLimits(0,-100,100);
    // gr1->Fit("fitFunction","","",fPositionX[0]-20,fPositionX[0]+100);
    // th1->Fit("fitFunction","","",55,80);
    gr1->Fit("fitFunction","","",50,90);

    // c1->Divide(2);
    // c1->cd(1);
    // TF1 *fitFunc = new TF1();
    // fitFunc = th1->GetFunction("fitFunction");
    // Double_t LeftBeta = fitFunc->GetParameter(1);
    // Double_t RightBeta = fitFunc->GetParameter(2);
    // Double_t peakTime = fitFunc->GetParameter(3);
    // Double_t AmpValue = fitFunc->GetParameter(4)*2/(sqrt(3.14159)*(LeftBeta + RightBeta)); 
    
    // th1->Draw();
    // fitFunc->DrawClone("same");
    // // fitFunc->Draw();
    // TLatex *lex = new TLatex();
    // lex->SetTextSize(0.03);
    // lex->DrawLatexNDC(0.6,0.8,Form("LeftBeta: %6.3f",LeftBeta));
    // lex->DrawLatexNDC(0.6,0.7,Form("RightBeta: %6.3f",RightBeta));
    // lex->DrawLatexNDC(0.6,0.6,Form("peakTime: %6.3f",peakTime-1.50*LeftBeta));
    // lex->DrawLatexNDC(0.6,0.9,Form("AmpValue: %6.3f",AmpValue));
    // lex->DrawLatexNDC(0.6,0.5,Form("#chi^{2}/ndf: %4.3f",fitFunc->GetChisquare()/fitFunc->GetNDF()));
    
    // c1->cd(2);
    c1->cd();
    TF1 *fitFunc = new TF1();
    fitFunc = gr1->GetFunction("fitFunction");
    Double_t LeftBeta1 = fitFunc->GetParameter(1);
    Double_t RightBeta1 = fitFunc->GetParameter(2);
    Double_t peakTime1 = fitFunc->GetParameter(3);
    Double_t AmpValue1 = fitFunc->GetParameter(4)*2/(sqrt(3.14159)*(LeftBeta1 + RightBeta1)); 
    
    gr1->Draw();
    fitFunc->DrawClone("same");
    // fitFunc->Draw();
    TLatex *lex1 = new TLatex();
    lex1->SetTextSize(0.03);
    lex1->DrawLatexNDC(0.6,0.8,Form("LeftBeta: %6.3f",LeftBeta1));
    lex1->DrawLatexNDC(0.6,0.7,Form("RightBeta: %6.3f",RightBeta1));
    lex1->DrawLatexNDC(0.6,0.6,Form("peakTime: %6.3f",peakTime1-1.50*LeftBeta1));
    lex1->DrawLatexNDC(0.6,0.9,Form("AmpValue: %6.3f",AmpValue1));
    lex1->DrawLatexNDC(0.6,0.5,Form("#chi^{2}/ndf: %4.3f",fitFunc->GetChisquare()/fitFunc->GetNDF()));
    c1->Update();

    // c1->cd(2);
    // TGraph *gr2 = new TGraph(n0, x, adc_code2);

    // // gr2->SetTitle("Wave Display (event ID+1)");
    // // gr2->GetXaxis()->SetTitle("cell number");
    // // gr2->GetXaxis()->CenterTitle();
    // // gr2->GetYaxis()->SetTitle("ADC code");
    // // gr2->GetYaxis()->CenterTitle();
    // gr2->SetLineWidth(4);    
    // gr2->SetMarkerColor(3);
    // gr2->SetLineColor(3);
    // // gr2->Draw("");

    // // c1->cd(3);
    // TGraph *gr3 = new TGraph(n0, x, adc_code3);
    // // gr3->SetTitle("Wave Display (event ID+2)");
    // // gr3->GetXaxis()->SetTitle("cell number");
    // // gr3->GetXaxis()->CenterTitle();
    // // gr3->GetYaxis()->SetTitle("ADC code");
    // // gr3->GetYaxis()->CenterTitle();
    // gr3->SetLineWidth(4);
    // gr3->SetMarkerColor(9);
    // gr3->SetLineColor(9);
    // // gr3->Draw("");

    // // c1->cd(4);
    // TGraph *gr4 = new TGraph(n0, x, adc_code4);
    // // gr4->SetTitle("Wave Display (event ID+3)");
    // // gr4->GetXaxis()->SetTitle("cell number");
    // // gr4->GetXaxis()->CenterTitle();
    // // gr4->GetYaxis()->SetTitle("ADC code");
    // // gr4->GetYaxis()->CenterTitle();
    // gr4->SetLineWidth(4);
    // gr4->SetMarkerColor(7);
    // gr4->SetLineColor(7);
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
    // gr2->Draw("same");
    // gr3->Draw("same");
    // gr4->Draw("same");
    return 1;
}