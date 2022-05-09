#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TF1.h"
#include "TH1F.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TClass.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGFileBrowser.h"
#include "TPaveText.h"
#include "TStyle.h"


#include <vector>
#include <string>
#include <set>
#include <utility>
#include <map>
#include <tuple>
#include <fstream>
#include <iostream>
#include <algorithm>

// class Calibration;
// class Baseline;

using namespace std;

//字符串分割
std::vector<std::string> StringSplit(std::string str);
bool ReadLine(std::fstream& theFile,std::vector<std::string>& result);


void alert(bool istrue,string warning_info=string("Warning!"));
void alert(bool istrue,char * warning_info);
char high_four(char byte);
string byte2string(unsigned char byte);
int binary2int(string str);


//void Raw2ROOT_OneFile(const Char_t *inputFile);
Double_t MyGauss(Double_t *x, Double_t *par);

// void SCA_EventDisplay(TFile* ROOTFile,int Event_ID,int SCA_ID);
// void SCA_EventDisplay(const Char_t *inputFile,int Event_ID = 0,int SCA_ID = 0,const Char_t *Calibration_File="Calibration/Calibration_0120.root",const Char_t *FitOptFile="",int ifFit=1);
// void SCA_Energy_Spectrum(const Char_t *inputFile,int SCA_ID = 0,const Char_t *Calibration_File="",double AmpCut_Min=0,double AmpCut_Max=0,const Char_t *FitOptFile="",int ifFit=1);
// void SCA_Energy_Spectrum_All(const Char_t *inputFile,const Char_t *Calibration_File,double AmpCut_Min=0,double AmpCut_Max=0,const Char_t *FitOptFile="",int ifFit=1);

// void SCA_EventDisplay_all(const Char_t *inputFile,int Event_ID_Begin,int Event_ID_End,const Char_t *Calibration_File,const Char_t *FitOptFile,int ifFit);


class  AsymGuass_Npeak {
 public:
	AsymGuass_Npeak(){}
	AsymGuass_Npeak(int n){Num_peaks = n;}
   	int Num_peaks;
   	double AsymGuass_fit_Npeak(double *v,double *para) {
        //para[0] : Mean Value	
        //para[1+4(npeaks-1)] : LeftBeta
        //para[2+4(npeaks-1)] : RightBeta
        //para[3+4(npeaks-1)] : Mid Point/The peak Time
        //para[4+4(npeaks-1)] : Amp Value
        //v[0] : variable
        
        double f=para[0];
        for(int i=1;i<=Num_peaks;i++){
            double x = v[0]-para[3+4*(i-1)];
            if(x<0){
                f = f + para[4+4*(i-1)]*2/(sqrt(TMath::Pi())*(para[1+4*(i-1)]+para[2+4*(i-1)]))*TMath::Exp( -(x/para[1+4*(i-1)])*(x/para[1+4*(i-1)]) );
            }
            else{
                f = f + para[4+4*(i-1)]*2/(sqrt(TMath::Pi())*(para[1+4*(i-1)]+para[2+4*(i-1)]))*TMath::Exp( -(x/para[2+4*(i-1)])*(x/para[2+4*(i-1)]) );
            }
        }
        // if(npeaks == 2) cout<<"npeaks == 2"<<endl;
        return f;
   }

   bool isNiceFit(TF1* f1){
        int npar = f1->GetNpar();
        for(int i =0;i<npar;i++){
            double val = f1->GetParameter(i);
            double err = f1->GetParError(i);
            if(abs(err/val)>0.1) return false;
        }
        return true;
    }
    double HowGoodFit(TF1* f1){
        double fitvalue = 1;
        int npar = f1->GetNpar();
        for(int i =0;i<npar;i++){
            double val = f1->GetParameter(i);
            double err = f1->GetParError(i);
            if ((1-abs(err/val))<fitvalue) fitvalue = (1-abs(err/val));
        }
        return fitvalue;
    }
};

#endif
