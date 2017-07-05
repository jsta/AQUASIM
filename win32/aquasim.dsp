# Microsoft Developer Studio Project File - Name="Aquasim" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Aquasim - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "aquasim.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "aquasim.mak" CFG="Aquasim - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Aquasim - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Aquasim - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Aquasim - Win32 Demo" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Aquasim - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\WinDebug"
# PROP BASE Intermediate_Dir ".\WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\WinDebug"
# PROP Intermediate_Dir ".\WinDebug"
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D AQUA_OS=AQUA_NT /D AQUA_VERSION=AQUA_VCC /D "_AFXDLL" /D "_MBCS" /Fr /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Aquasim - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\WinRel"
# PROP BASE Intermediate_Dir ".\WinRel"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\WinRel"
# PROP Intermediate_Dir ".\WinRel"
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D AQUA_OS=AQUA_NT /D AQUA_VERSION=AQUA_VCC /D "_AFXDLL" /D "_MBCS" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Aquasim - Win32 Demo"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Aquasim___Win32_Demo"
# PROP BASE Intermediate_Dir "Aquasim___Win32_Demo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinDemo"
# PROP Intermediate_Dir "WinDemo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D AQUA_OS=AQUA_NT /D AQUA_VERSION=AQUA_VCC /D "_AFXDLL" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /Fr /YX /Yc /Yu
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D AQUA_OS=AQUA_NT /D AQUA_VERSION=AQUA_VCC /D "_AFXDLL" /D "_MBCS" /D "AQUA_DEMO" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /mktyplib203
# ADD MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Aquasim - Win32 Debug"
# Name "Aquasim - Win32 Release"
# Name "Aquasim - Win32 Demo"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\aqcore\Advcomp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\All.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Aqcomp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqdoc_w.cpp

!IF  "$(CFG)" == "Aquasim - Win32 Debug"

!ELSEIF  "$(CFG)" == "Aquasim - Win32 Release"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Aquasim - Win32 Demo"

# SUBTRACT BASE CPP /YX /Yc /Yu
# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\aqcore\Aqlink.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Aqproc.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aquamac.R
# End Source File
# Begin Source File

SOURCE=.\AQUASIM.cpp
# End Source File
# Begin Source File

SOURCE=.\AQUASIM.rc
# End Source File
# Begin Source File

SOURCE=.\aqcore\Aquasys.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Aqvar.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqview_w.cpp
# End Source File
# Begin Source File

SOURCE=.\caest_w.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\CALCSYS.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\casens_w.cpp
# End Source File
# Begin Source File

SOURCE=.\casim_w.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\Compsys.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Das.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Dassl.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\dcolor_w.cpp
# End Source File
# Begin Source File

SOURCE=.\delsta_w.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\Drawpts.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\drline_w.cpp
# End Source File
# Begin Source File

SOURCE=.\drmark_w.cpp
# End Source File
# Begin Source File

SOURCE=.\drtext_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edadv_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edadvl_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edcomp_w.cpp
# End Source File
# Begin Source File

SOURCE=.\eddiff_w.cpp
# End Source File
# Begin Source File

SOURCE=.\eddynp_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edequp_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edfilm_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edform_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edlake_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edlink_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edmix_w.cpp
# End Source File
# Begin Source File

SOURCE=.\ednum_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edpar_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edprob_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edproc_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edprog_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edriv_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edrlis_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edsed_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edsoil_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edstat_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edvar_w.cpp
# End Source File
# Begin Source File

SOURCE=.\edvlis_w.cpp
# End Source File
# Begin Source File

SOURCE=.\file_w.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\Fileio.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Filmcomp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Fit.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Fitsys.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Formvar.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Fortran.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\GAMMA.CPP
# End Source File
# Begin Source File

SOURCE=.\aqcore\Init.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Intpol.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Lakecomp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Linksys.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Linpack.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\mainfr_w.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\mdi.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\Mixcomp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Numpar.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Othervar.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\plot_w.cpp
# End Source File
# Begin Source File

SOURCE=.\plotdo_w.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\Plotpts.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Plotsys.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\plotvw_w.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\Procsys.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# Begin Source File

SOURCE=.\aqcore\readwords.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\Rivcomp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Sdlist.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\sedcomp.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\Soilcomp.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\sort.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\stat.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\Statesys.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\aqcore\Symbol.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Varsys.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\aqcore\Vectors.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\vires_w.cpp
# End Source File
# Begin Source File

SOURCE=.\waidlg_w.cpp
# End Source File
# Begin Source File

SOURCE=.\aqcore\Zero.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\ZoomD_W.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\aqcore\advcomp.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\all.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\aqcomp.h
# End Source File
# Begin Source File

SOURCE=.\aqdoc_w.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\aqlink.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\aqproc.h
# End Source File
# Begin Source File

SOURCE=.\aquasim.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\aquasys.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\aqvar.h
# End Source File
# Begin Source File

SOURCE=.\aqview_w.h
# End Source File
# Begin Source File

SOURCE=.\caest_w.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\calcsys.h
# End Source File
# Begin Source File

SOURCE=.\casens_w.h
# End Source File
# Begin Source File

SOURCE=.\casim_w.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\compsys.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\das.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\dassl.h
# End Source File
# Begin Source File

SOURCE=.\dcolor_w.h
# End Source File
# Begin Source File

SOURCE=.\delsta_w.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\drawpts.h
# End Source File
# Begin Source File

SOURCE=.\drline_w.h
# End Source File
# Begin Source File

SOURCE=.\drmark_w.h
# End Source File
# Begin Source File

SOURCE=.\drtext_w.h
# End Source File
# Begin Source File

SOURCE=.\edadv_w.h
# End Source File
# Begin Source File

SOURCE=.\edadvl_w.h
# End Source File
# Begin Source File

SOURCE=.\edcomp_w.h
# End Source File
# Begin Source File

SOURCE=.\eddiff_w.h
# End Source File
# Begin Source File

SOURCE=.\eddynp_w.h
# End Source File
# Begin Source File

SOURCE=.\edequp_w.h
# End Source File
# Begin Source File

SOURCE=.\edfilm_w.h
# End Source File
# Begin Source File

SOURCE=.\edform_w.h
# End Source File
# Begin Source File

SOURCE=.\edlake_w.h
# End Source File
# Begin Source File

SOURCE=.\edlink_w.h
# End Source File
# Begin Source File

SOURCE=.\edmix_w.h
# End Source File
# Begin Source File

SOURCE=.\ednum_w.h
# End Source File
# Begin Source File

SOURCE=.\edpar_w.h
# End Source File
# Begin Source File

SOURCE=.\edprob_w.h
# End Source File
# Begin Source File

SOURCE=.\edproc_w.h
# End Source File
# Begin Source File

SOURCE=.\edprog_w.h
# End Source File
# Begin Source File

SOURCE=.\edriv_w.h
# End Source File
# Begin Source File

SOURCE=.\edrlis_w.h
# End Source File
# Begin Source File

SOURCE=.\edsed_w.h
# End Source File
# Begin Source File

SOURCE=.\edsoil_w.h
# End Source File
# Begin Source File

SOURCE=.\edstat_w.h
# End Source File
# Begin Source File

SOURCE=.\edvar_w.h
# End Source File
# Begin Source File

SOURCE=.\edvlis_w.h
# End Source File
# Begin Source File

SOURCE=.\file_w.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\fileio.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\filmcomp.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\fit.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\fitsys.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\formvar.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\fortran.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\init.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\intpol.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\lakecomp.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\linksys.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\linpack.h
# End Source File
# Begin Source File

SOURCE=.\mainfr_w.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\mixcomp.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\numpar.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\othervar.h
# End Source File
# Begin Source File

SOURCE=.\plot_w.h
# End Source File
# Begin Source File

SOURCE=.\plotdo_w.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\plotsys.h
# End Source File
# Begin Source File

SOURCE=.\plotvw_w.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\procsys.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.hm
# End Source File
# Begin Source File

SOURCE=.\aqcore\rivcomp.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\sdlist.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\sedcomp.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\soilcomp.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\statesys.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\symbol.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\varsys.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\vectors.h
# End Source File
# Begin Source File

SOURCE=.\vires_w.h
# End Source File
# Begin Source File

SOURCE=.\waidlg_w.h
# End Source File
# Begin Source File

SOURCE=.\aqcore\zero.h
# End Source File
# Begin Source File

SOURCE=.\zoomd_w.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\aqdoc_w.ico
# End Source File
# Begin Source File

SOURCE=.\res\aquasim.ico
# End Source File
# Begin Source File

SOURCE=.\res\aquasim.rc2
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\idr_plot.ico
# End Source File
# Begin Source File

SOURCE=.\res\toolbar.bmp
# End Source File
# End Group
# End Target
# End Project
