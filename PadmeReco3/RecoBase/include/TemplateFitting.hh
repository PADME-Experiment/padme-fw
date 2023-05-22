#ifndef TemplateFitting_H
#define TemplateFitting_H

#include "TH1D.h"
#include "TF1.h"

class FitTemp8Par {
public:

  FitTemp8Par() {

    fVerbose = 0;
    fGoodSamples = 994;

    DefineFunction();

  }

  ~FitTemp8Par(){
    if (fTemplateFunction) delete fTemplateFunction;
  }

  void SetWaveformHisto(TH1D* h) { fWaveformHisto = h; }

  void SetVerbose(Int_t v) { fVerbose = v; }

  void SetGoodSamples(Int_t s) { fGoodSamples = s; }

  void SetScale(Double_t s)    { fScale    = s; }
  void SetShift(Double_t s)    { fShift    = s; }
  void SetLStart(Double_t s)   { fLStart   = s; }
  void SetLLength(Double_t s)  { fLLength  = s; }
  void SetDeltaMax(Double_t s) { fDeltaMax = s; }
  void SetTauMix(Double_t s)   { fTauMix   = s; }
  void SetTauFast(Double_t s)  { fTauFast  = s; }
  void SetTauSlow(Double_t s)  { fTauSlow  = s; }

  Double_t GetScale()    { return fScale;    }
  Double_t GetShift()    { return fShift;    }
  Double_t GetLStart()   { return fLStart;   }
  Double_t GetLLength()  { return fLLength;  }
  Double_t GetDeltaMax() { return fDeltaMax; }
  Double_t GetTauMix()   { return fTauMix;   }
  Double_t GetTauFast()  { return fTauFast;  }
  Double_t GetTauSlow()  { return fTauSlow;  }

  Int_t GetFitStatus() { return fFitStatus; }
  Double_t GetFitChiSquare()  { return fFitChiSquare;  }

  void DoFit() { DoFit(""); }

  void DoFit(TString opt) {

    if (fVerbose == 0) {
      opt += "Q";
    } else if (fVerbose>1) {
      opt += "V";
    }

    Double_t pars[] = {fScale,fShift,fLStart,fLLength,fDeltaMax,fTauMix,fTauFast,fTauSlow};
    fTemplateFunction->SetParameters(pars);
    if (fVerbose) printf("Initial fit parameters - Scale %f Shift %f LIni %f LLen %f DMax %f Mix %f TauF %f TauS %f\n",
			 pars[0],pars[1],pars[2],pars[3],pars[4],pars[5],pars[6],pars[7]);

    fFitStatus = fWaveformHisto->Fit(fTemplateFunction,"","",0.,fGoodSamples);
    fFitChiSquare = fTemplateFunction->GetChisquare();
    if (fVerbose) printf("Fit status %d ChiSq %f\n",fFitStatus,fFitChiSquare);

    fScale    = fTemplateFunction->GetParameter(0);
    fShift    = fTemplateFunction->GetParameter(1);
    fLStart   = fTemplateFunction->GetParameter(2);
    fLLength  = fTemplateFunction->GetParameter(3);
    fDeltaMax = fTemplateFunction->GetParameter(4);
    fTauMix   = fTemplateFunction->GetParameter(5);
    fTauFast  = fTemplateFunction->GetParameter(6);
    fTauSlow  = fTemplateFunction->GetParameter(7);

    if (fVerbose) printf("Final fit parameters - Scale %f Shift %f LIni %f LLen %f DMax %f Mix %f TauF %f TauS %f\n",
			 fScale,fShift,fLStart,fLLength,fDeltaMax,fTauMix,fTauFast,fTauSlow);

  }

private:

  Int_t fVerbose;

  TF1* fTemplateFunction;

  TH1D* fWaveformHisto;

  Int_t fGoodSamples;

  // Fit parameters
  Double_t fScale;
  Double_t fShift;
  Double_t fLStart;
  Double_t fLLength;
  Double_t fDeltaMax;
  Double_t fTauMix;
  Double_t fTauFast;
  Double_t fTauSlow;

  Int_t fFitStatus;
  Double_t fFitChiSquare;

  void DefineFunction() {

    // Analytical template function with double BGO decay time
    fTemplateFunction = new TF1("tempfunc",[&](double* x, double* p){

	// Template has an initial parabolic rise followed by a linear slope, a parabolic maximum and a double exponential decay
	// Function and first derivative are continuous at the conjunction points of the four sections

	static double ym = 1.; // Value at maximum point of the parabola

	double scale = p[0];                          // Global scale to wavefunction
	double shift = p[1];                          // Global shift to wavefunction
	double xs    = p[2];                          // Initial point of linear section
	double ll    = p[3];                          // Length (from initial point to final point) of linear section (must be >0)
	double xp    = p[4];                          // Distance from end of linear section to parabolic maximum (must be >0)
	double ct    = p[5];                          // Mix coefficient for decay times (between 0. and 1.)
	double tau1  = p[6];                          // BGO decay time - fast component
	double tau2  = p[7];                          // BGO decay time - slow component

	// Compute derived quantities
	double xl    = xs+ll;                         // Final point of linear section
	double xm    = xl+xp;                         // Vertex position of upper parabola (template maximum)
	double yl    = 2.*ym*xl/(xl+xm);              // Value at end of linear section
	double m     = yl/xl;                         // Angular coefficient of linear section
	double al    = 0.25*yl/(xs*xl);               // Parabolic coefficient for lower parabola
	double au    = ym/(xl*xl-xm*xm);              // Parabolic coefficient for upper parabola
	double xv    = -xs;                           // Vertex position of lower parabola
	//double ys    = m*xs;                          // Value at start of linear section
	double taum  = 1./(ct/tau1+(1.-ct)/tau2);     // Combined tau coefficient
	double xn    = xm-taum+sqrt(taum*taum-ym/au); // Start of exponential section
	double yn    = ym+au*(xn-xm)*(xn-xm);         // Value at start of exponential section

	double xx    = x[0]-shift;                    // Shift template to waveform position

	//printf("p0 %f p1 %f p2 %f p3 %f p4 %f p5 %f p6 %f p7 %f\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
	if (xx<=xv) return 0.;                                              // Pedestal section
	if (xx<=xs) return scale*al*(xx-xv)*(xx-xv);                        // Parabolic rise section
	if (xx<=xl) return scale*m*xx;                                      // Linear rise section
	//if (xx<=xn) return scale*ym*(1.-(xm-xx)*(xm-xx)/((xm+xl)*(xm-xl))); // Parabolic section (waveform maximum)
	if (xx<=xn) return scale*(ym+au*(xx-xm)*(xx-xm));                   // Parabolic section (waveform maximum)
	return scale*yn*(ct*exp(-(xx-xn)/tau1)+(1.-ct)*exp(-(xx-xn)/tau2)); // Exponential section (BGO decay)

      },0.,1024.,8);

    // Define names of parameters used in the function
    fTemplateFunction->SetParNames("Scale","Shift","LStart","LLen","dMax","Mix","Tau1","Tau2");

    // Define limits for parameters used in the function
    fTemplateFunction->SetParLimits(0,0.,1000.);  // Scale (mV)
    fTemplateFunction->SetParLimits(1,0.,1000.);  // Time shift (ns)
    fTemplateFunction->SetParLimits(2,0.,5.);     // Start of linear section (ns)
    fTemplateFunction->SetParLimits(3,0.,20.);    // Length of linear section (ns)
    fTemplateFunction->SetParLimits(4,0.,20.);    // Distance from end of linear section to maximum (ns)
    fTemplateFunction->SetParLimits(5,0.1,0.9);   // Tau mix (p5*fast+(1-p5)*slow)
    fTemplateFunction->SetParLimits(6,5.,100.);   // Tau1 (ns) - Fast BGO component
    fTemplateFunction->SetParLimits(7,200.,350.); // Tau2 (ns) - Slow BGO component

  }

};

class FitTemp7Par {
public:

  FitTemp7Par() {

    fVerbose = 0;
    fGoodSamples = 994;

    DefineFunction();

  }

  ~FitTemp7Par(){
    if (fTemplateFunction) delete fTemplateFunction;
  }

  void SetWaveformHisto(TH1D* h) { fWaveformHisto = h; }

  void SetVerbose(Int_t v) { fVerbose = v; }

  void SetGoodSamples(Int_t s) { fGoodSamples = s; }

  void SetScale(Double_t s)    { fScale    = s; }
  void SetShift(Double_t s)    { fShift    = s; }
  void SetLLength(Double_t s)  { fLLength  = s; }
  void SetDeltaMax(Double_t s) { fDeltaMax = s; }
  void SetTauMix(Double_t s)   { fTauMix   = s; }
  void SetTauFast(Double_t s)  { fTauFast  = s; }
  void SetTauSlow(Double_t s)  { fTauSlow  = s; }

  Double_t GetScale()    { return fScale;    }
  Double_t GetShift()    { return fShift;    }
  Double_t GetLLength()  { return fLLength;  }
  Double_t GetDeltaMax() { return fDeltaMax; }
  Double_t GetTauMix()   { return fTauMix;   }
  Double_t GetTauFast()  { return fTauFast;  }
  Double_t GetTauSlow()  { return fTauSlow;  }

  Int_t GetFitStatus() { return fFitStatus; }
  Double_t GetFitChiSquare()  { return fFitChiSquare;  }

  void DoFit() { DoFit(""); }

  void DoFit(TString opt) {

    if (fVerbose == 0) {
      opt += "Q";
    } else if (fVerbose>1) {
      opt += "V";
    }

    Double_t pars[] = {fScale,fShift,fLLength,fDeltaMax,fTauMix,fTauFast,fTauSlow};
    fTemplateFunction->SetParameters(pars);
    if (fVerbose) printf("Initial fit parameters - Scale %f Shift %f LLen %f DMax %f Mix %f TauF %f TauS %f\n",
			 pars[0],pars[1],pars[2],pars[3],pars[4],pars[5],pars[6]);

    fFitStatus = fWaveformHisto->Fit(fTemplateFunction,opt,"",0.,fGoodSamples);
    fFitChiSquare = fTemplateFunction->GetChisquare();
    if (fVerbose) printf("Fit status %d ChiSq %f\n",fFitStatus,fFitChiSquare);

    fScale    = fTemplateFunction->GetParameter(0);
    fShift    = fTemplateFunction->GetParameter(1);
    fLLength  = fTemplateFunction->GetParameter(2);
    fDeltaMax = fTemplateFunction->GetParameter(3);
    fTauMix   = fTemplateFunction->GetParameter(4);
    fTauFast  = fTemplateFunction->GetParameter(5);
    fTauSlow  = fTemplateFunction->GetParameter(6);

    if (fVerbose) printf("Final fit parameters - Scale %f Shift %f LLen %f DMax %f Mix %f TauF %f TauS %f\n",
			 fScale,fShift,fLLength,fDeltaMax,fTauMix,fTauFast,fTauSlow);

  }

private:

  Int_t fVerbose;

  TF1* fTemplateFunction;

  TH1D* fWaveformHisto;

  Int_t fGoodSamples;

  // Fit parameters
  Double_t fScale;
  Double_t fShift;
  Double_t fLLength;
  Double_t fDeltaMax;
  Double_t fTauMix;
  Double_t fTauFast;
  Double_t fTauSlow;

  Int_t fFitStatus;
  Double_t fFitChiSquare;

  void DefineFunction() {

    // Analytical template function with double BGO decay time
    fTemplateFunction = new TF1("tempfunc",[&](double* x, double* p){

	// Template has an initial linear rise, a parabolic maximum and a double exponential decay
	// Function and first derivative are continuous at the conjunction points of the three sections

	static double ym = 1.; // Value at maximum point of the parabola

	double scale = p[0];                          // Global scale to wavefunction
	double shift = p[1];                          // Global shift to wavefunction
	double ll    = p[2];                          // Length (from initial point to final point) of linear section (must be >0)
	double xp    = p[3];                          // Distance from end of linear section to parabolic maximum (must be >0)
	double ct    = p[4];                          // Mix coefficient for decay times (between 0. and 1.)
	double tau1  = p[5];                          // BGO decay time - fast component
	double tau2  = p[6];                          // BGO decay time - slow component

	// Compute derived quantities
	double xl    = ll;                            // Final point of linear section
	double xm    = xl+xp;                         // Vertex position of upper parabola (template maximum)
	double yl    = 2.*ym*xl/(xl+xm);              // Value at end of linear section
	double m     = yl/xl;                         // Angular coefficient of linear section
	double au    = ym/(xl*xl-xm*xm);              // Parabolic coefficient for upper parabola
	double taum  = 1./(ct/tau1+(1.-ct)/tau2);     // Combined tau coefficient
	double xn    = xm-taum+sqrt(taum*taum-ym/au); // Start of exponential section
	double yn    = ym+au*(xn-xm)*(xn-xm);         // Value at start of exponential section

	double xx    = x[0]-shift;                    // Shift template to waveform position

	//printf("p0 %f p1 %f p2 %f p3 %f p4 %f p5 %f p6 %f p7 %f\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
	if (xx<=0.) return 0.;                                              // Pedestal section
	if (xx<=xl) return scale*m*xx;                                      // Linear rise section
	if (xx<=xn) return scale*(ym+au*(xx-xm)*(xx-xm));                   // Parabolic section (waveform maximum)
	return scale*yn*(ct*exp(-(xx-xn)/tau1)+(1.-ct)*exp(-(xx-xn)/tau2)); // Exponential section (BGO decay)

      },0.,1024.,7);

    // Define names of parameters used in the function
    fTemplateFunction->SetParNames("Scale","Shift","LLen","dMax","Mix","Tau1","Tau2");

    // Define limits for parameters used in the function
    fTemplateFunction->SetParLimits(0,0.,1000.);  // Scale (mV)
    fTemplateFunction->SetParLimits(1,0.,1000.);  // Time shift (ns)
    fTemplateFunction->SetParLimits(2,0.,20.);    // Length of linear section (ns)
    fTemplateFunction->SetParLimits(3,0.,5.);    // Distance from end of linear section to maximum (ns)
    fTemplateFunction->SetParLimits(4,0.1,0.9);   // Tau mix (p5*fast+(1-p5)*slow)
    fTemplateFunction->SetParLimits(5,5.,100.);   // Tau1 (ns) - Fast BGO component
    fTemplateFunction->SetParLimits(6,200.,350.); // Tau2 (ns) - Slow BGO component

  }

};

class FitTemp6Par {
public:

  FitTemp6Par() {

    fVerbose = 0;
    fGoodSamples = 994;

    DefineFunction();

  }

  ~FitTemp6Par(){
    if (fTemplateFunction) delete fTemplateFunction;
  }

  void SetWaveformHisto(TH1D* h) { fWaveformHisto = h; }

  void SetVerbose(Int_t v) { fVerbose = v; }

  void SetGoodSamples(Int_t s) { fGoodSamples = s; }

  void SetScale(Double_t s)    { fScale    = s; }
  void SetShift(Double_t s)    { fShift    = s; }
  void SetLLength(Double_t s)  { fLLength  = s; }
  void SetTauMix(Double_t s)   { fTauMix   = s; }
  void SetTauFast(Double_t s)  { fTauFast  = s; }
  void SetTauSlow(Double_t s)  { fTauSlow  = s; }

  Double_t GetScale()    { return fScale;    }
  Double_t GetShift()    { return fShift;    }
  Double_t GetLLength()  { return fLLength;  }
  Double_t GetTauMix()   { return fTauMix;   }
  Double_t GetTauFast()  { return fTauFast;  }
  Double_t GetTauSlow()  { return fTauSlow;  }

  Int_t GetFitStatus() { return fFitStatus; }
  Double_t GetFitChiSquare()  { return fFitChiSquare;  }

  void DoFit() { DoFit(""); }

  void DoFit(TString opt) {

    if (fVerbose == 0) {
      opt += "Q";
    } else if (fVerbose>1) {
      opt += "V";
    }

    Double_t pars[] = {fScale,fShift,fLLength,fTauMix,fTauFast,fTauSlow};
    fTemplateFunction->SetParameters(pars);
    if (fVerbose) printf("Initial fit parameters - Scale %f Shift %f LLen %f Mix %f TauF %f TauS %f\n",
			 pars[0],pars[1],pars[2],pars[3],pars[4],pars[5]);

    fFitStatus = fWaveformHisto->Fit(fTemplateFunction,opt,"",0.,fGoodSamples);
    fFitChiSquare = fTemplateFunction->GetChisquare();
    if (fVerbose) printf("Fit status %d ChiSq %f\n",fFitStatus,fFitChiSquare);

    fScale    = fTemplateFunction->GetParameter(0);
    fShift    = fTemplateFunction->GetParameter(1);
    fLLength  = fTemplateFunction->GetParameter(2);
    fTauMix   = fTemplateFunction->GetParameter(3);
    fTauFast  = fTemplateFunction->GetParameter(4);
    fTauSlow  = fTemplateFunction->GetParameter(5);

    if (fVerbose) printf("Final fit parameters - Scale %f Shift %f LLen %f Mix %f TauF %f TauS %f\n",
			 fScale,fShift,fLLength,fTauMix,fTauFast,fTauSlow);

  }

private:

  Int_t fVerbose;

  TF1* fTemplateFunction;

  TH1D* fWaveformHisto;

  Int_t fGoodSamples;

  // Fit parameters
  Double_t fScale;
  Double_t fShift;
  Double_t fLLength;
  Double_t fTauMix;
  Double_t fTauFast;
  Double_t fTauSlow;

  Int_t fFitStatus;
  Double_t fFitChiSquare;

  void DefineFunction() {

    // Analytical template function with double BGO decay time
    fTemplateFunction = new TF1("tempfunc",[&](double* x, double* p){

	// Template has an initial linear slope followed by a double exponential decay

	static double ym = 1.; // Value at maximum point of the parabola

	double scale = p[0];                          // Global scale to wavefunction
	double shift = p[1];                          // Global shift to wavefunction
	double ll    = p[2];                          // Length (from initial point to final point) of linear section (must be >0)
	double ct    = p[3];                          // Mix coefficient for decay times (between 0. and 1.)
	double tau1  = p[4];                          // BGO decay time - fast component
	double tau2  = p[5];                          // BGO decay time - slow component

	// Compute derived quantities
	double xl    = ll;                            // Final point of linear section
	double yl    = ym;                            // Value at end of linear section (template maximum)
	double m     = yl/xl;                         // Angular coefficient of linear section

	double xx    = x[0]-shift;                    // Shift template to waveform position

	//printf("p0 %f p1 %f p2 %f p3 %f p4 %f p5 %f p6 %f p7 %f\n",p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);
	if (xx<=0.) return 0.;                                              // Pedestal section
	if (xx<=xl) return scale*m*xx;                                      // Linear rise section
	return scale*yl*(ct*exp(-(xx-xl)/tau1)+(1.-ct)*exp(-(xx-xl)/tau2)); // Exponential section (BGO decay)

      },0.,1024.,6);

    // Define names of parameters used in the function
    fTemplateFunction->SetParNames("Scale","Shift","LLen","Mix","Tau1","Tau2");

    // Define limits for parameters used in the function
    fTemplateFunction->SetParLimits(0,0.,1000.);  // Scale (mV)
    fTemplateFunction->SetParLimits(1,0.,1000.);  // Time shift (ns)
    fTemplateFunction->SetParLimits(2,1.,30.);    // Length of linear section (ns)
    fTemplateFunction->SetParLimits(3,0.1,0.9);   // Tau mix (p3*fast+(1-p3)*slow)
    fTemplateFunction->SetParLimits(4,5.,100.);   // Tau1 (ns) - Fast BGO component
    fTemplateFunction->SetParLimits(5,200.,350.); // Tau2 (ns) - Slow BGO component

  }

};

#endif
