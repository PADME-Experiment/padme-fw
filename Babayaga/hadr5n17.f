c;;;;;;;;;;;;;;;;;;;;;;;;;;;;; -*- Mode: Fortran -*- ;;;;;;;;;;;;;;;;;;;;;;;;;;;
c;; hadr5n17.f --- 
c;; Author          : Friedrich Jegerlehner
c;; Created On      : Thu Jul  6 12:20:17 2017
c;; Last Modified By: Friedrich Jegerlehner
c;; Last Modified On: Sun Oct  8 02:19:56 2017
c;; RCS: $Id$
c;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
c;; Copyright (C) 2017 Friedrich Jegerlehner
c;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
c;; 
carlo       subroutine hadr5x(e,st2,der,errdersta,errdersys,
c     &     deg,errdegsta,errdegsys)
       subroutine hadr5n17(e,st2,der,errdersta,errdersys,
     &     deg,errdegsta,errdegsys)
c Provides real part of 5 flavor hadronic contribution to photon vacuum polarization
c single precision HADR5 ; double precision DHADR5
c ******************************************************************
c *                                                                *
c *      subroutine for the evaluation of the light hadron         *
c *           contributions to Delta_r  and  Delta_g               *
c *                    using fits to the                           *
c *          QED vacuum polarization from e^+ e^- data             *
c *                                                                *
c *    F. Jegerlehner, Institut für Physik                         *
c *                    Humboldt-Universität zu Berlin              *
c *                    Newtonstraße 15, D-12489 Berlin, Germany    *
c *    E-mail: fjeger@physik.hu-berlin.de                          *
c *    Phone :   +49-3375 500 277                                  *
c *                                                                *
c *    Reference: F. Jegerlehner, Z. Phys. C32 (1986) 195          *
c *               H. Burkhardt et al., Z. Phys. C42 (1989) 497     *
c *               S. Eidelman, F. Jegerlehner, Z. Phys. C (1995)   *
c *               F. Jegerlehner,                                  *
c *                    Nucl.Phys.Proc.Suppl.131:213-222,2004       *
c *                    Nucl.Phys.Proc.Suppl.162:22-32,2006         *
c *                    Nucl.Phys.Proc.Suppl.181-182:135-140,2008   *
c *                                                                *
c ******************************************************************
c       VERSION: 06/107/2017
c
C  Notation: E energy ( momentum transfer ): E>0 timelike , E<0 spacelike
c      sin2ell=0.23153 ! pm 0.00016 LEPEEWG Phys Rep 427 (2006) 257
C            st2 is sin^2(Theta); st2=0.23153 is the reference value
C  the routine returns the hadronic contribution of 5 flavors (u,d,s,c,b)
C                 to   DER=Delta_r with hadronic error ERRDER
C                and   DEG=Delta_g with hadronic error ERRDEG
C  The effective value of the fine structure constant alphaQED at energy
C  E is alphaQED(E)=alphaQED(0)/(1-Delta_r) ,similarly for the SU(2)
C  coupling alphaSU2(E)=alphaSU2(0)/(1-Delta_g), where Delta_r(g) is the
C  sum of leptonic, hadronic contributions (top to be added).
C
      IMPLICIT NONE
      INTEGER NA,NB,NC,IJ,I,ini
      PARAMETER(NA=979,NB=2174,NC=200)
      real e,st2,der,errdersta,errdersys,deg,errdegsta,errdegsys,fac
      real dal(3),dg2(3)
      REAL ETA(NA),DAT(NA,3),DGT(NA,3)
      REAL ESA(NB),DAS(NB,3),DGS(NB,3)
      REAL EMA(NC),DAM(NC,3),DGM(NC,3)
      REAL XXX(NC),YGG(NC,3),Y3G(NC,3)
      COMMON /DATM17/EMA,DAM,DGM  ! time-like high
      COMMON /DATT17/ETA,DAT,DGT  ! space-like 
      COMMON /DATS17/ESA,DAS,DGS  ! timelike low
      COMMON /DATH17/XXX,YGG,Y3G       ! space-like high
      data ini/0/
      integer ifirst
      common/fred17first/ifirst
      data ifirst /0/
C initialize data

      if (ifirst.eq.0) then
         call dalhad_spacelike17
         call dalhad_timelike17
         call dalhad_timelike1h17
         ifirst = 1
      endif
      
c boundaries of data sets
c space-like and unphysical region  -1.0000E+03 - 0.27599999D0 (ETA,DAT)
c time-like R data range 0.28207018D0 - 38.29000000D0          (ESA,DAS)
c time-like1 pQCD range 1.3005E+01 - 2.0000E+04                (EMA,DAM)
c 2 pion threshold at 0.27914....
      fac=0.23153d0/st2
      if (e.eq.0.0) then
        der=0.0
        errdersta=0.0
        errdersys=0.0
        deg=0.0
        errdegsta=0.0
        errdegsys=0.0
        return
c first bridge cracks between regions
      else if ((e.ge.XXX(NC)).and.(e.lt.ETA(1))) then
        DO I=1,3
        dal(i)=YGG(NC,I)
     &    +(DAT(1,I)-YGG(NC,I))/(ETA(1)-XXX(NC))*(E-XXX(NC))
        dg2(i)=Y3G(NC,I)
     &    +(DAT(1,I)-Y3G(NC,I))/(ETA(1)-XXX(NC))*(E-XXX(NC))
        ENDDO
        der=dal(1)
        errdersta=dal(2)
        errdersys=dal(3)
        deg=dg2(1)*fac
        errdegsta=dg2(2)*fac
        errdegsys=dg2(3)*fac
        return
      else if ((e.ge.ETA(NA)).and.(e.lt.ESA(1))) then
        DO I=1,3
        dal(i)=DAT(NA,I)
     &    +(DAS(1,I)-DAT(NA,I))/(ESA(1)-ETA(NA))*(E-ETA(NA))
        dg2(i)=DGT(NA,I)
     &    +(DGS(1,I)-DGT(NA,I))/(ESA(1)-ETA(NA))*(E-ETA(NA))
        ENDDO
        der=dal(1)
        errdersta=dal(2)
        errdersys=dal(3)
        deg=dg2(1)*fac
        errdegsta=dg2(2)*fac
        errdegsys=dg2(3)*fac
        return
      else if ((e.lt.ETA(NA)).and.(e.ge.ETA(1))) then
        ij=NA
        do while (ETA(ij).gt.e)
          ij=ij-1
        enddo
        DO I=1,3
        dal(i)=DAT(IJ,I)
     &    +(DAT(IJ+1,I)-DAT(IJ,I))/(ETA(IJ+1)-ETA(IJ))*(E-ETA(IJ))
        dg2(i)=DGT(IJ,I)
     &    +(DGT(IJ+1,I)-DGT(IJ,I))/(ETA(IJ+1)-ETA(IJ))*(E-ETA(IJ))
        ENDDO
        der=dal(1)
        errdersta=dal(2)
        errdersys=dal(3)
        deg=dg2(1)*fac
        errdegsta=dg2(2)*fac
        errdegsys=dg2(3)*fac
        return
      else if ((e.lt.EMA(NC)).and.(e.ge.EMA(1))) then
        ij=NC
        do while (EMA(ij).gt.e)
          ij=ij-1
        enddo
        DO I=1,3
        dal(i)=DAM(IJ,I)
     &    +(DAM(IJ+1,I)-DAM(IJ,I))/(EMA(IJ+1)-EMA(IJ))*(E-EMA(IJ))
        dg2(i)=DGM(IJ,I)
     &    +(DGM(IJ+1,I)-DGM(IJ,I))/(EMA(IJ+1)-EMA(IJ))*(E-EMA(IJ))
        ENDDO
        der=dal(1)
        errdersta=dal(2)
        errdersys=dal(3)
        deg=dg2(1)*fac
        errdegsta=dg2(2)*fac
        errdegsys=dg2(3)*fac
        return
      else if ((e.lt.ESA(NB)).and.(e.ge.ESA(1))) then
        if ((e.gt.3.96).and.(e.lt.4.52).and.(ini.eq.0)) then
c           write (*,*) ' ******************************************'
c           write (*,*) ' * Note: fluctuations in the Psi(4,5,6)   *'
c           write (*,*) ' * region are because we are using the    *'
c           write (*,*) ' * CB and BES data without any smoothing  *'
c           write (*,*) ' *********** FJ@HU Berlin *****************'
           ini=1
        endif
        ij=NB
        do while (ESA(ij).gt.e)
          ij=ij-1
        enddo
        DO I=1,3
        dal(i)=DAS(IJ,I)
     &    +(DAS(IJ+1,I)-DAS(IJ,I))/(ESA(IJ+1)-ESA(IJ))*(E-ESA(IJ))
        dg2(i)=DGS(IJ,I)
     &    +(DGS(IJ+1,I)-DGS(IJ,I))/(ESA(IJ+1)-ESA(IJ))*(E-ESA(IJ))
        ENDDO
        der=dal(1)
        errdersta=dal(2)
        errdersys=dal(3)
        deg=dg2(1)*fac
        errdegsta=dg2(2)*fac
        errdegsys=dg2(3)*fac
        return
      else if ((e.lt.XXX(NC)).and.(e.ge.XXX(1))) then
        ij=NC
        do while (XXX(ij).gt.e)
          ij=ij-1
        enddo
        DO I=1,3
        dal(i)=YGG(IJ,I)
     &    +(YGG(IJ+1,I)-YGG(IJ,I))/(XXX(IJ+1)-XXX(IJ))*(E-XXX(IJ))
        dg2(i)=Y3G(IJ,I)
     &    +(Y3G(IJ+1,I)-Y3G(IJ,I))/(XXX(IJ+1)-XXX(IJ))*(E-XXX(IJ))
        ENDDO
        der=dal(1)
        errdersta=dal(2)
        errdersys=dal(3)
        deg=dg2(1)*fac
        errdegsta=dg2(2)*fac
        errdegsys=dg2(3)*fac
        return
      else 
         if (ini.le.10) then
            write(*,*) ' out of range! e=',e
            ini=ini+1
            if (ini.eq.10) write(*,*) ' Warning: continuing anyway ...'
         endif
      endif
      return
      end

carlo       subroutine dhadr5x(de,dst2,dder,derrdersta,derrdersys,
c     &     ddeg,derrdegsta,derrdegsys)
       subroutine dhadr5n17(de,dst2,dder,derrdersta,derrdersys,
     &     ddeg,derrdegsta,derrdegsys)
c **********************************************************************
c *    F. Jegerlehner, University of Silesia, PL-40007 Katowice, Poland *
c **********************************************************************
c Converts hadr5n15 to double precision variables in dhadr5n15
c
       implicit none
       real *8 de,dst2,dder,ddeg,derrdersta,derrdersys,
     &      derrdegsta,derrdegsys
       real    se,sst2,sder,sdeg,serrdersta,serrdersys,
     &      serrdegsta,serrdegsys
       se  =sngl(de)
       sst2=sngl(dst2)
carlo       call hadr5x(se,sst2,sder,serrdersta,serrdersys,sdeg,
c     &      serrdegsta,serrdegsys)
       call hadr5n17(se,sst2,sder,serrdersta,serrdersys,sdeg,
     &      serrdegsta,serrdegsys)
       dder   =dble(sder)
       ddeg   =dble(sdeg)
       derrdersta=dble(serrdersta)
       derrdersys=dble(serrdersys)
       derrdegsta=dble(serrdegsta)
       derrdegsys=dble(serrdegsys)
       return
       end
c
c Following datasets obtained with run parameters specified in the header
c
      include 'dalhadslow17.f'
c      include 'deghadslow17.f'

      include 'dalhadshigh17.f'
c      include 'deghadshigh17.f'

      include 'dalhadt17.f'
c      include 'deghadt17.f'
c      include 'dalhadthigh17.f'
c      include 'deghadthigh17.f'
