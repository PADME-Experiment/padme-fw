#ifndef _VSamplingChannel_hh_
#define _VSamplingChannel_hh_
#include<string>
typedef unsigned short int UShort_t;
typedef short int          Short_t ;

class VSamplingChannel{
  public:
    virtual ~VSamplingChannel(){}
    VSamplingChannel():
      fSamples(nullptr),
      fNSamples(0),
      fT0(0),
      fType("DefaultType") {}
    //VSamplingChannel(UShort_t n,Short_t* arr){VSamplingChannel(); SetNSamples(n); SetSamplesArr(arr);}
    void SetNSamples  (UShort_t n){fNSamples=n;}
    void SetSamplesArr(Short_t* arr){fSamples=arr;}
    void SetSamples(UShort_t n,Short_t* arr){SetNSamples(n); SetSamplesArr(arr);}
    int  GetT0()const {return fT0;}
    void SetT0(int t){fT0=t;}
    Short_t GetSampT0Cor(int t)const {t-=fT0;return t>=0&&t<fNSamples?fSamples[t]:0;}
    Short_t GetSamp     (int t)const {       return t>=0&&t<fNSamples?fSamples[t]:0;}
    UShort_t GetNSamples()const{return fNSamples;}
    void SetName(const std::string&n){fName=n;}
    const std::string& GetName()const{return fName;}
    const std::string& GetType()const{return fType;}

  protected:
    Short_t* fSamples;  ///< [ADU]
    UShort_t fNSamples;
    int fT0;
    std::string fName;
    std::string fType;
};
#endif
