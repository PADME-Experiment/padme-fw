#ifndef SystemInfo_H
#define SystemInfo_H 1

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SystemInfo
{
public:

  virtual ~SystemInfo() {;};
  static SystemInfo* GetInstance();

private:

  static SystemInfo* fInstance;

protected:

  SystemInfo() {;};

public:

  void timefmt_gm(char*);
  void timefmt_local(char*);

  int vmsize();
  int store_vmsize();
  int delta_vmsize();

private:

  int fCurrentVmSize;
  int fStoredVmSize;

};

#endif // SystemInfo_H
