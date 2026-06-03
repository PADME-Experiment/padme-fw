*********************************************************************
      subroutine setfilenames(programma,corrections)
! written by CMCC, last modified 9/10/2005
      include 'shared.inc'
      character*100  distname(ndistr),tmp(ndistr)
      common/filenames/distname
      character*(*) programma,corrections
      character*2   ec,ac
      character*7   bw

      do k = 1,7
         bw(k:k) = ' '
      enddo
      do k = 1,ndistr
         do i = 1,100
            tmp(k)(i:i) = ' '
         enddo
      enddo

      ebeam = ecmsnom/2.d0

      if (nbin.eq.50)  bw =  '50.txt'
      if (nbin.eq.100) bw = '100.txt'
      if (nbin.eq.200) bw = '200.txt'
      if (nbin.eq.300) bw = '300.txt'
      if (nbin.eq.400) bw = '400.txt'
      if (nbin.eq.500) bw = '500.txt'
      if (nbin.eq.600) bw = '600.txt'
      if (nbin.eq.800) bw = '800.txt'
      if (nbin.eq.1000) bw = '1000.txt'

******************
      tmp(1)  = 'el_th_'
      tmp(2)  = 'el_en_'
      tmp(3)  = 'acoll_'
      tmp(4)  = 'g1_en_'
      tmp(5)  = 'g1_th_'
      tmp(6)  = 'ep_th_'
      tmp(7)  = 'ep_en_'
      tmp(8)  = 'e+e-m_'
      tmp(9)  = 'g2_en_'
      tmp(10) = 'g2_th_'
      tmp(11) = 'e1r_th_'
      tmp(12) = 'e1r_en_'
      tmp(13) = 'e2r_th_'
      tmp(14) = 'e2r_en_'
      tmp(15) = 'aco2_'
      tmp(16) = 't_'
******************
      do k=1,ndistr
         lung(k) = len_trim(tmp(k))
      enddo

      if (corrections.eq.'oal') then
         do k=1,ndistr
            tmp(k)(lung(k)+1:) = 'oal_'//bw
         enddo
      endif
      if (corrections.eq.'oal2') then
         do k=1,ndistr
            tmp(k)(lung(k)+1:) = 'oal2_'//bw
         enddo
      endif

      if (corrections.eq.'exp') then
         do k=1,ndistr
            tmp(k)(lung(k)+1:) = 'exp_'//bw
         enddo
      endif

      if (corrections.eq.'born') then
         do k=1,ndistr
            tmp(k)(lung(k)+1:) = 'born_'//bw
         enddo
      endif

      if (corrections.eq.'struct') then
         do k=1,ndistr
            tmp(k)(lung(k)+1:) = 'stru_'//bw
         enddo
      endif

      do k = 1,len(programma)
         if (programma(k:k).ne.' ') lpath=k
      enddo

      do k = 1,ndistr
         distname(k)(1:lpath)  = programma
         distname(k)(lpath+1:) = tmp(k)
      enddo

      return
      end
*********************************************************************
      subroutine distributions(sd,nc,p3,p4,qph)
! written by CMCC, last modified 9/10/2005
      include 'shared.inc'      
      parameter (ibin = 3 * nbin)      
      parameter (iperj = ndistr*ibin)
      dimension pin1(0:3),pin2(0:3)
      common/momentainitial/pin1,pin2
      common/reducedtoborn/p1b(0:3),p2b(0:3),iiiref
      character*2 fs
      common/finalstate/fs
      common/idebugging/idebug      
      common/distr/distr,s_distr,bmi,bma 
      common/ionlyfirsttime/ifirst,icount
      data ifirst,icount /1,0/ 

      common/tmintmax/tmin,tmax,tcur

      common/beamspreadsigmas/esig1,esig2

      common/parameters/ame,ammu,convfac,alpha,pi_i
      
      common/par/pi,radtodeg,ebeam
      common/idarkon/idarkon
      dimension s_distr(ndistr,ibin)
      dimension distr(ndistr,ibin),bmi(ndistr),bma(ndistr),fun(ndistr)

      data ((distr(i,j),i=1,ndistr),j=1,ibin) / iperj * 0.d0/
      data ((s_distr(i,j),i=1,ndistr),j=1,ibin) / iperj * 0.d0/

      integer*8 nc
      double precision csi(1)
      
      double precision qa(0:3),qb(0:3),px(0:3),py(0:3)
      double precision pluto(0:3)
      common/massainv/amassainvmin,amassainvmax

      double precision qphtot(40,0:3)
      double precision p1inside(0:3),p2inside(0:3)
      double precision p1rad(0:3),p2rad(0:3)
      integer inside(40),icutgg
      common/cutsggcmn/qphtot,p1inside,p2inside,p1rad,p2rad,
     .     nph,npairs,inside,n_inside,icutgg

      
      ebeam = ecmsnom/2.d0
      if (ifirst.eq.1) then
         pi = 4.d0 * atan(1.d0)
         radtodeg = 180.d0/pi
         bmi(1) = thmine * radtodeg - 0.1d0
         bma(1) = thmaxe * radtodeg + 0.1d0

         bmi(1) = (thmine) * radtodeg - 1.d0
         bma(1) = (thmaxe) * radtodeg + 1.d0
         
         if (fs.eq.'gg') then ! for any fs!
            bmi(1) = 0.d0 * radtodeg - 0.1d0
            bma(1) = pi * radtodeg + 0.1d0
         endif
         bmi(2) = emin  - 0.01d0
         bma(2) = ebeam + 0.01d0!max(esig1,esig2)*2.d0 + 0.000001d0

c         bmi(2) = emin
c         bma(2) = ebeam
         
         bmi(3) =  -1d0 !-.01d0
         bma(3) =  11d0!zmax*radtodeg + .01d0 
         bmi(4) =  -0.01d0!0.0d0*ebeam
         bma(4) =  0.7d0*ebeam!bma(2)
         bmi(5) =  -1d0!0.d0 * radtodeg - 0.01d0
         bma(5) =   181d0! pi * radtodeg + 0.01d0

         bmi(6) =  bmi(1)
         bma(6) =  bma(1)
c         if (fs.eq.'ee'.or.fs.eq.'mm') then
c            bmi(6) = thminp * radtodeg - 0.1d0
c            bma(6) = thmaxp * radtodeg + 0.1d0
c         endif
         
         bmi(7) =  bmi(2)
         bma(7) =  bma(2)
         bmi(8) = 2.d0*0.511d-3  - 0.00001d0

         bmin = 1.d0
         cccc = cos(pi-zmax)
         bmi(8) = sqrt(2.d0*emin*emin*(1.d0-bmin*bmin*cccc))

c         bmi(8) = 2.d0*0.4d0
c         bma(8) = 1.0200000000001d0

c         print*,bmi(8),bma(8),(bma(8)-bmi(8))*0.5d0
c         stop
         
c         bmi(8) = 3.096d0
c         bma(8) = 3.098d0!2.d0*ebeam

c         bmi(8) = 0.d0
c         bma(8) = 10.d0
         
         bmi(9)  =  bmi(4)
         bma(9)  =  bma(4)
         bmi(10) =  bmi(5)
         bma(10) =  bma(5)

         bmi(11) = bmi(1)
         bma(11) = bma(1)
         bmi(12) = bmi(2)
         bma(12) = bma(2)
         bmi(13) = bmi(6)
         bma(13) = bma(6)
         bmi(14) = bmi(7)
         bma(14) = bma(7)

         bmi(15) = bmi(3)
         bma(15) = bma(3)

         bmi(16) = -ecmsnom**2*0.5d0*(1.d0-cos(thmaxe))
         bma(16) = -ecmsnom**2*0.5d0*(1.d0-cos(thmine))
         
! filling distr. components when entering the first time
         do k = 1,ndistr
            d = (bma(k) - bmi(k))/nbin
            y = bmi(k)
            do i = 1,nbin
               y = y + d
               distr(k,i*3-2)   = y - d
               s_distr(k,i*3-2) = y - d
            enddo
         enddo
         ifirst = 0
      endif
******************
      if (fs.ne.'gg') then
         px = p3
         py = p4
         call duefotonipiuenergetici(qph,qa,qb)
      else
         px = p1inside
         py = p2inside
         qa = p1rad
         qb = p2rad
      endif
*** electron angle
      pe     = sqrt(px(1)**2+px(2)**2+px(3)**2)           
      fun(1) = radtodeg*acos(px(3)/pe)
**** electron energy
      fun(2) = px(0)
*** positron angle                                     
      pp     = sqrt(py(1)**2+py(2)**2+py(3)**2)           
      fun(6) = radtodeg*acos(py(3)/pp)
**** positron energy
      fun(7) = py(0)
         
**** acollinearity
      fun(3)  = abs(180.d0 - fun(1)- fun(6))
      fun(15) = abs(180.d0 - radtodeg*acos(tridot(px,py)/pe/pp))

      fun(16) = dot(pin1-px,pin1-px)
      
c      if (fun(3).gt.100d0) then
c         print*,'gt 100!',fun(3)
c         print*,fun(1)
c         print*,fun(6)
c         print*,radtodeg*acos(qa(3)/qa(0))
c         print*,px
c         print*,py
c         print*,qa
c      endif
         
***   m.e. photon energy
      fun(4)  = bma(4) + 1.d0
      fun(5)  = bma(5) + 1.d0
      fun(9)  = bma(9) + 1.d0
      fun(10) = bma(10) + 1.d0

      soglia = 0.02d0 * ebeam
c      soglia = eps * ebeam
      if (qa(0).ge.soglia) then 
         fun(4) = qa(0)
         fun(5) = radtodeg*acos(qa(3)/qa(0))
      endif
      if (qb(0).ge.soglia) then 
         fun(9)  = qb(0)
         fun(10) = radtodeg*acos(qb(3)/qb(0))
      endif

*** missing momentum variables
      fun(8)  = sqrt(abs(dot(px+py,px+py)))

*** randomized p1,p2      
      call getrnd(csi,1)
      if (csi(1).lt.0.5d0) then
         fun(11) = fun(1)
         fun(12) = fun(2)
         fun(13) = fun(6)
         fun(14) = fun(7)
      else
         fun(13) = fun(1)
         fun(14) = fun(2)
         fun(11) = fun(6)
         fun(12) = fun(7)
      endif
      
***********************************************************************
      DO k = 1,ndistr
         bmax = bma(k)
         d = (bma(k) - bmi(k))/nbin
         x = bmi(k)
** NEW ***
         if (fun(k).lt.bma(k).and.fun(k).gt.bmi(k)) then
            i  = int((fun(k) - bmi(k))/d)
c            if ((i+1).gt.nbin) print*,'WARNING!!',
c     .                            k,i+1,d,fun(k),bmi(k),bma(k)
            i  = min(i+1,nbin)*3

            distr(k,i-1) = distr(k,i-1) + sd
            distr(k,i)   = distr(k,i)   + sd**2                       
            su  = distr(k,i-1)
            su2 = distr(k,i)               
            s_distr(k,i-1) = su / nc
            argument = abs((su2/nc-s_distr(k,i-1)**2)/nc)
            s_distr(k,i) = sqrt(argument)
         endif         
** NEW ***
      ENDDO          
      return
      end
***      
      subroutine writedistributions
! written by CMCC, last modified 9/10/2005
      include 'shared.inc'
      common/ifirstwritedistributions/ifirst
      
      parameter (ibin = 3 * nbin)      
      parameter (iperj = ndistr*ibin)
      
      common/filenames/distname
      common/distr/distr,s_distr,bmi,bma
      
      dimension s_distr(ndistr,ibin),bmi(ndistr),bma(ndistr)
      dimension distr(ndistr,ibin)
      
      character*100 distname(ndistr)

      data ifirst /1/

      do k = 1,ndistr
         open(12,file=distname(k),status='unknown')
         do i=1, nbin
            sezd  = s_distr(k,i*3-1)/(bma(k)-bmi(k))*nbin
            esezd = s_distr(k,i*3)  /(bma(k)-bmi(k))*nbin
            write(12,*)s_distr(k,i*3-2),sezd,esezd
         enddo
         close(12)
      enddo

      ifirst = 0
      return
      end
cccccccccccccccccccccccccccccccccccccccccccccccccccccc
      subroutine mostenergeticphoton(q,q1)
! written by CMCC, last modified 9/10/2005
      implicit real*8 (a-h,o-z)
      dimension q(40,0:3),q1(0:3)
*  LEADING ENERGETIC PHOTON IS EXTRACTED
      q1(0) = 0.d0
      q1(1) = 0.d0
      q1(2) = 0.d0
      q1(3) = 0.d0

ccc      if (q(1,0).lt.1.d-11) return
C for backward compatibility with old ps-based BABAYAGA
      if (q(1,0).lt.1.d-11.and.q(11,0).lt.0.d0.and.
     .     q(21,0).lt.1.d-11.and.q(31,0).lt.0.d0) return
      
      ENPHOT = Q(1,0)
      ENHARD = ENPHOT
      J=1
*     
      DO I = 1,40
         if (q(i,0).gt.0.d0) then
            ENPHOT = Q(I,0)
            IF (ENHARD.GE.ENPHOT) THEN
               ENHARD = ENHARD
            ELSE
               J = I
               ENHARD = ENPHOT
            ENDIF
         endif
      ENDDO 
*     
      ENHARDL = ENHARD
      JL = J
*     
      if (j.gt.0) then
         DO I = 0,3
            Q1(I) = Q(J,I)
         ENDDO
      endif
      return
      end
*****************************************
      function get_dphi(p1,p2)
      implicit double precision (a-h,o-z)
      double precision p1(0:3),p2(0:3)
! from ALPGEN
      dphi = (p1(1)*p2(1)+p1(2)*p2(2))/
     .     sqrt(p1(1)**2+p1(2)**2)/sqrt(p2(1)**2+p2(2)**2)
      if (abs(dphi).gt.1.d0) then
         dphi = dphi/abs(dphi)
      endif
      get_dphi = acos(dphi)
      return
      end
***********************

      function getphi_here(ppp)
      implicit double precision (a-h,o-z)
      dimension ppp(0:3)
      parameter (pi = 3.141592653589793238462643383279502884197169399d0)
      pm = sqrt(ppp(1)**2+ppp(2)**2+ppp(3)**2)
      c  = ppp(3)/pm
      s  = sqrt(1.d0-c**2)
      if (s.eq.0.d0) then
         getphi_here = 0.d0
         return
      else   
         arg = ppp(1)/pm/s
*  avoiding numerical problems......
         if (abs(arg).ge.1.d0) then
            iarg = arg
            arg  = iarg
         endif   
         if (ppp(2).ge.0.d0) getphi_here = acos(arg)
         if (ppp(2).lt.0.d0) getphi_here = 2.d0*pi-acos(arg)
      endif             
      return
      end
cccccccccccccccccccccccccccccccccccccccccccccc
      subroutine duefotonipiuenergetici(q,q1,q2)
! written by CMCC, last modified 9/10/2005
      implicit real*8 (a-h,o-z)
      dimension q(40,0:3),q1(0:3),q2(0:3),ql(40,0:3)
* 
*  LEADING ENERGETIC PHOTON IS EXTRACTED
*
      q1 = 0.d0
      q2 = 0.d0

      n = 0
      do k = 1,40
         if (q(k,0).gt.0.d0) then
            n = n+1
            ql(n,:) = q(k,:)
         endif
      enddo
      if (n.eq.0) return
      
      ENPHOT = QL(1,0)
      ENHARD = ENPHOT
      J=1
*     
      DO I = 1,N
         ENPHOT = QL(I,0)
         IF (enphot.ge.enhard) THEN
            J = I
            ENHARD = ENPHOT
         ENDIF
      ENDDO 
*    
      Q1 = QL(J,:)
      ENHARDL = ENHARD
      JL = J
*     
      j1 = j
*     
*     NEXT-TO-LEADING ENERGETIC PHOTON IS EXTRACTED
*     
      ENHARD = 0.D0
      J = 0
*     
      DO I = 1,N
         IF (I.NE.JL) THEN
            ENPHOT = QL(I,0)
            IF (ENPHOT.GE.ENHARD) THEN
               J = I
               ENHARD = ENPHOT
            ENDIF
         ENDIF
      ENDDO 
c
      if (j.gt.0) then
         Q2 = QL(J,:)
      endif
      j2=j

      return
      end      
************************************************
      subroutine duefotonipiuenergetici_bck(q,q1,q2)
! written by CMCC, last modified 9/10/2005
      implicit real*8 (a-h,o-z)
      dimension q(40,0:3),q1(0:3),q2(0:3)
* 
*  LEADING ENERGETIC PHOTON IS EXTRACTED
*
        do i = 0,3
           q1(i) = 0.d0
           q2(i) = 0.d0
        enddo

        sum = 0.d0
	do k=1,40
          sum = sum + q(k,0)
        enddo

        if (sum.lt.1.d-11) return

        ENPHOT = Q(1,0)
        ENHARD = ENPHOT
        J=1
* 
        DO I = 1,40
           ENPHOT = Q(I,0)
           IF (ENHARD.GE.ENPHOT) THEN
              ENHARD = ENHARD
           ELSE
              J = I
              ENHARD = ENPHOT
           ENDIF
        ENDDO 
*   
        ENHARDL = ENHARD
        JL = J
*
        if (j.gt.0) then
           DO I = 0,3
              Q1(I) = Q(J,I)
           ENDDO
        endif
        j1 = j
*
*  NEXT-TO-LEADING ENERGETIC PHOTON IS EXTRACTED
*
      ENHARD = 0.D0
      J = 0
* 
      DO I = 1,40
         IF (I.NE.JL) THEN
*
            ENPHOT = Q(I,0)
            IF (ENHARD.GE.ENPHOT) THEN
               ENHARD = ENHARD
            ELSE
               J = I
               ENHARD = ENPHOT
            ENDIF
*     
         ENDIF
      ENDDO 
*   
      ENHARD = ENHARD
*
      if (j.gt.0) then
         DO I = 0,3
            Q2(I) = Q(J,I)
         ENDDO
      endif
      j2=j

      return
      end
