# Microsoft Developer Studio Project File - Name="notcrisp_gui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=notcrisp_gui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "notcrisp_gui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "notcrisp_gui.mak" CFG="notcrisp_gui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "notcrisp_gui - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "notcrisp_gui - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "notcrisp_gui___Win32_Release"
# PROP BASE Intermediate_Dir "notcrisp_gui___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "notcrisp_gui___Win32_Release"
# PROP Intermediate_Dir "notcrisp_gui___Win32_Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "$(QTDIR)\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "NOVAR" /D "ROUTE_TRACKING" /D "DOWNSTREAM" /D "GUI" /D "REALTIME" /D "NOMINMAX" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(QTDIR)\lib\qt-mt.lib imm32.lib wsock32.lib $(QTDIR)\lib\qtmain.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"msvcrtd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"notcrisp_gui___Win32_Release/compass_gui.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "notcrisp_gui___Win32_Debug"
# PROP BASE Intermediate_Dir "notcrisp_gui___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "notcrisp_gui___Win32_Debug"
# PROP Intermediate_Dir "notcrisp_gui___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "$(QTDIR)\include" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "NOVAR" /D "ROUTE_TRACKING" /D "DOWNSTREAM" /D "GUI" /D "REALTIME" /D "NOMINMAX" /D "QT_THREAD_SUPPORT" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib $(QTDIR)\lib\qt-mt.lib imm32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"msvcrtd.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libcmt.lib" /out:"notcrisp_gui___Win32_Debug/compass_gui.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "notcrisp_gui - Win32 Release"
# Name "notcrisp_gui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "help_compiled"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\help\output\help_1.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_13.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_14.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_15.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_16.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_17.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_18.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_19.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_2.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_20.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_21.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_22.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_23.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_24.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_25.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_26.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_27.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_28.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_3.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_300.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_301.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_303.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_304.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_305.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_306.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_307.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_308.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_309.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_310.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_311.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_312.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_313.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_314.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_315.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_316.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_317.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_318.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_319.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_32.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_320.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_321.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_322.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_323.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_324.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_325.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_326.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_327.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_328.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_329.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_33.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_330.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_331.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_332.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_333.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_334.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_335.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_336.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_337.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_338.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_339.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_34.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_340.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_341.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_342.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_343.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_344.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_345.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_346.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_347.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_348.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_349.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_35.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_350.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_351.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_352.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_353.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_354.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_355.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_356.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_357.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_358.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_359.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_36.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_360.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_361.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_362.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_363.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_364.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_365.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_366.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_367.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_368.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_369.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_37.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_370.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_371.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_372.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_373.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_374.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_375.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_376.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_377.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_378.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_379.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_38.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_380.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_381.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_382.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_383.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_384.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_385.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_386.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_387.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_388.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_389.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_39.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_390.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_391.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_392.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_393.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_394.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_395.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_396.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_397.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_398.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_399.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_4.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_40.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_400.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_401.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_402.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_403.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_404.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_405.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_406.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_407.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_41.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_418.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_419.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_42.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_43.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_44.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_443.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_45.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_46.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_47.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_48.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_49.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_5.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_50.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_51.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_52.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_53.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_54.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_55.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_56.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_57.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_58.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_6.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_60.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_61.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_62.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_63.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_64.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_65.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_66.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_67.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_68.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_69.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_70.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_71.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_72.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_73.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_74.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_75.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_76.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_77.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_78.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_79.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_80.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_81.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_82.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_83.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_84.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_85.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_86.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_87.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_88.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_89.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_90.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_91.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_92.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_95.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_96.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_98.html_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_altcomp.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_bar_xs.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_basvol.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_browse.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_c1icon.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_chp1_pa1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_chp2_a17.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_chp2_a18.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_chp2_a19.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_chp2_a20.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_chp2_a27.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_cityscap.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_condots.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_cris174a.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp.toc_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp113.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp119.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp149.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp151.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp154.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp16.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp160.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp161.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp173.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp174.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp193.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp213.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp214.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp229.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp241.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp246.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp249.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp253.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp288.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp298.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_crisp308.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_delayeq.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_dgt.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_done.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_dpd.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_dpp.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_dwnarrow.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_elev.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_fge.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_fgeeq.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_filemgr.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_filledba.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_fixbtn.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_flowd.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_flowdam.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_flowhd.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_flowr.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_fopen1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_fpcoef.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_fsave1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_gasmort.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_gdis.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_go.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_grid1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_grid2.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_groupbtn.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_help.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_help1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_hmod.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_hollowba.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_hwflow.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_hwoutfl.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_hwtemp.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_hydro.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_hydromap.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_inputrt.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_kentrn.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_lall.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_larrow.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_loss.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_map.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_map1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_map2.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_mapdam.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_maprel.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_maptruck.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_matsur.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_meq.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_mfish.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_monteal.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_montert.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_mortd.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_mouse1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_mrel.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_mrv.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_mtool.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_output.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_outputd.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_outputh.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_outputr.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_owicon.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_pdcoef.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_pdd.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_pfspill.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_phcap.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_phpri.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_phsch.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_phsthres.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_powere.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_powers.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_print1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_pspill.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_ptreq.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_rcan1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_redcirc.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_relsch.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_rgt.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_rivermap.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_roundbtn.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_rparm1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_rpcoef15.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_rpd15.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_rtime.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_rtool2.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_run1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_rvel.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_scap.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_schkey.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_simort.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_slival.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_speff.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_spill.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_ssf.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_stool.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_subm.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_submenu.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_tdgdam.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_tdgrch.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_tmerit.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_tpcoef.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_trmort.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_unreghw.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_unzm.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_uparrow.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_vvar.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_wand1.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_wclose.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_wmax.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_wquit.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_wtempd.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_wtempr.jpg_data.c
# End Source File
# Begin Source File

SOURCE=.\help\output\help_yelldot.jpg_data.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\altern_list.c
# End Source File
# Begin Source File

SOURCE=.\args.c
# End Source File
# Begin Source File

SOURCE=.\at_frame.c
# End Source File
# Begin Source File

SOURCE=.\at_hist.c
# End Source File
# Begin Source File

SOURCE=.\base_gas_mort.c
# End Source File
# Begin Source File

SOURCE=.\basin.c
# End Source File
# Begin Source File

SOURCE=.\batch.c
# End Source File
# Begin Source File

SOURCE=.\command.c
# End Source File
# Begin Source File

SOURCE=.\compute_gas_mort.c
# End Source File
# Begin Source File

SOURCE=.\crisp.c
# End Source File
# Begin Source File

SOURCE=.\Crisp1.rc
# End Source File
# Begin Source File

SOURCE=.\custom_survival.c
# End Source File
# Begin Source File

SOURCE=.\dam_model.c
# End Source File
# Begin Source File

SOURCE=.\dataio.c
# End Source File
# Begin Source File

SOURCE=.\daylight.c
# End Source File
# Begin Source File

SOURCE=.\delay.c
# End Source File
# Begin Source File

SOURCE=.\delayed_mort.c
# End Source File
# Begin Source File

SOURCE=.\dir.c
# End Source File
# Begin Source File

SOURCE=.\dump.c
# End Source File
# Begin Source File

SOURCE=.\eqns_dflt.c
# End Source File
# Begin Source File

SOURCE=.\eqns_util.c
# End Source File
# Begin Source File

SOURCE=.\exp_table.c
# End Source File
# Begin Source File

SOURCE=.\fdens.c
# End Source File
# Begin Source File

SOURCE=.\fge.c
# End Source File
# Begin Source File

SOURCE=.\file.c
# End Source File
# Begin Source File

SOURCE=.\flow.c
# End Source File
# Begin Source File

SOURCE=.\fpe.c
# End Source File
# Begin Source File

SOURCE=.\gas_mort.c
# End Source File
# Begin Source File

SOURCE=.\gas_mort_eqn.c
# End Source File
# Begin Source File

SOURCE=.\gc_dist.c
# End Source File
# Begin Source File

SOURCE=.\growth.c
# End Source File
# Begin Source File

SOURCE=.\headw.c
# End Source File
# Begin Source File

SOURCE=.\high_gas_mort.c
# End Source File
# Begin Source File

SOURCE=.\implicit_base.cpp
# End Source File
# Begin Source File

SOURCE=.\include.c
# End Source File
# Begin Source File

SOURCE=.\init.c
# End Source File
# Begin Source File

SOURCE=.\init_river.c
# End Source File
# Begin Source File

SOURCE=.\ks.c
# End Source File
# Begin Source File

SOURCE=.\list.c
# End Source File
# Begin Source File

SOURCE=.\log.c
# End Source File
# Begin Source File

SOURCE=.\main.c
# End Source File
# Begin Source File

SOURCE=.\migration.c
# End Source File
# Begin Source File

SOURCE=.\moment.c
# End Source File
# Begin Source File

SOURCE=.\monte.c
# End Source File
# Begin Source File

SOURCE=.\monte_frame.c
# End Source File
# Begin Source File

SOURCE=.\monte_index.c
# End Source File
# Begin Source File

SOURCE=.\monte_output.c
# End Source File
# Begin Source File

SOURCE=.\monte_spill.c
# End Source File
# Begin Source File

SOURCE=.\multi_frame.c
# End Source File
# Begin Source File

SOURCE=.\nrutil_short.c
# End Source File
# Begin Source File

SOURCE=.\parse.c
# End Source File
# Begin Source File

SOURCE=.\parse_alt.c
# End Source File
# Begin Source File

SOURCE=.\parse_archive.c
# End Source File
# Begin Source File

SOURCE=.\parse_at.c
# End Source File
# Begin Source File

SOURCE=.\parse_monte_index.c
# End Source File
# Begin Source File

SOURCE=.\parseutil.c
# End Source File
# Begin Source File

SOURCE=.\passage.c
# End Source File
# Begin Source File

SOURCE=.\pbn.c
# End Source File
# Begin Source File

SOURCE=.\period.c
# End Source File
# Begin Source File

SOURCE=.\phouse.c
# End Source File
# Begin Source File

SOURCE=.\pred.c
# End Source File
# Begin Source File

SOURCE=.\qCheckBoxDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qCheckBoxDialogMedBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qCheckBoxGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\Qcrisp.cpp
# End Source File
# Begin Source File

SOURCE=.\QCStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\qDamOutputCBDMedBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qDamReachGDT.cpp
# End Source File
# Begin Source File

SOURCE=.\qDataReportTool.cpp
# End Source File
# Begin Source File

SOURCE=.\qDatContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\qDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qDialogMedBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qDialogMedEquationBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qDialogMedTimeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qEditGraphDataToken.cpp
# End Source File
# Begin Source File

SOURCE=.\qEditGraphGDT.cpp
# End Source File
# Begin Source File

SOURCE=.\qEditGraphMediator.cpp
# End Source File
# Begin Source File

SOURCE=.\qEditSliderBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qEditSliderDouble.cpp
# End Source File
# Begin Source File

SOURCE=.\qEditSliderLong.cpp
# End Source File
# Begin Source File

SOURCE=.\qEquationBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qEquationDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qEquationFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\qFileManager.cpp
# End Source File
# Begin Source File

SOURCE=.\qGlobalEvents.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphDataToken.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphDataTokenBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphMedDrawAxisXI.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphMedDrawAxisYD.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphMedDrawData.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphMedDrawLines.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphMedDrawText.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphMediator.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphMedPosition.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphMedZoom.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphPane.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphSpeciesMediator.cpp
# End Source File
# Begin Source File

SOURCE=.\qGraphUpdateMed.cpp
# End Source File
# Begin Source File

SOURCE=.\qHeadwOutputCBDMedBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qHelpManager.cpp
# End Source File
# Begin Source File

SOURCE=.\qHelpWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\qHistogramGDT.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcDamDelayEquation.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcData.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcEditGraph.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcEditGraphBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcEquationBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcFDensEquation.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcFGEEquation.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcGasDistEquation.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcGasMortEquation.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcList.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcListBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcMigrationEquation.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcNSatEquation.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcPredatorProbability.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcPredTempEquation.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcSchedule.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcScheduleBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcSelectBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcSpillEquation.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcTimeBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qIdcTransMortEquation.cpp
# End Source File
# Begin Source File

SOURCE=.\qImConScroller.cpp
# End Source File
# Begin Source File

SOURCE=.\qImpTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\qInputReportTool.cpp
# End Source File
# Begin Source File

SOURCE=.\qInterfaceDataControl.cpp
# End Source File
# Begin Source File

SOURCE=.\qIOCell.cpp
# End Source File
# Begin Source File

SOURCE=.\qIOHistGDT.cpp
# End Source File
# Begin Source File

SOURCE=.\qIONotes.cpp
# End Source File
# Begin Source File

SOURCE=.\qIOPostBonnevilleSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\qIOSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\qIOSIMPASSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\qIOTool.cpp
# End Source File
# Begin Source File

SOURCE=.\qListDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qListDialogMedBase.cpp
# End Source File
# Begin Source File

SOURCE=.\QlogWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\Qmain.cpp
# End Source File
# Begin Source File

SOURCE=.\QmapController.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapMedCrisp1.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapMedDialogBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapMedGeography.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapMedGridLines.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapMediator.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapMedIcons.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapMedSelector.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapMedStatusBarUpdator.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapMedUnZoom.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapMedZoom.cpp
# End Source File
# Begin Source File

SOURCE=.\QmapObject.cpp
# End Source File
# Begin Source File

SOURCE=.\qMapPane.cpp
# End Source File
# Begin Source File

SOURCE=.\qMediatorBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\qMercatorTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\qMonteAltCmpDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\qMonteAnalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\qMonteCarloManager.cpp
# End Source File
# Begin Source File

SOURCE=.\qMonteCarloTool.cpp
# End Source File
# Begin Source File

SOURCE=.\qMonteReportTool.cpp
# End Source File
# Begin Source File

SOURCE=.\qMouseTool.cpp
# End Source File
# Begin Source File

SOURCE=.\qPassHistGDT.cpp
# End Source File
# Begin Source File

SOURCE=.\QPointTransform.cpp
# End Source File
# Begin Source File

SOURCE=.\qPromptForString.cpp
# End Source File
# Begin Source File

SOURCE=.\qReachClassCBDMed.cpp
# End Source File
# Begin Source File

SOURCE=.\qReachClassEquationMed.cpp
# End Source File
# Begin Source File

SOURCE=.\qReachClassSDMed.cpp
# End Source File
# Begin Source File

SOURCE=.\qReachClassTool.cpp
# End Source File
# Begin Source File

SOURCE=.\qReachOutputCBDMedBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qReleaseEGDT.cpp
# End Source File
# Begin Source File

SOURCE=.\qReleaseEGM.cpp
# End Source File
# Begin Source File

SOURCE=.\qReleaseMedDrawData.cpp
# End Source File
# Begin Source File

SOURCE=.\qReleaseTool.cpp
# End Source File
# Begin Source File

SOURCE=.\qRowColumnBox.cpp
# End Source File
# Begin Source File

SOURCE=.\qRunSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\qScenarioManager.cpp
# End Source File
# Begin Source File

SOURCE=.\qScheduleDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qScheduleDialogMed.cpp
# End Source File
# Begin Source File

SOURCE=.\qScheduleDialogMedBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qScheduleTool.cpp
# End Source File
# Begin Source File

SOURCE=.\qSelectDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qSelectDialogMedBase.cpp
# End Source File
# Begin Source File

SOURCE=.\qTableItem.cpp
# End Source File
# Begin Source File

SOURCE=.\qTextOutDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\qTransportTool.cpp
# End Source File
# Begin Source File

SOURCE=.\qUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\qZabelSlider.cpp
# End Source File
# Begin Source File

SOURCE=.\reach_model.c
# End Source File
# Begin Source File

SOURCE=.\realtime.c
# End Source File
# Begin Source File

SOURCE=.\release.c
# End Source File
# Begin Source File

SOURCE=.\release_util.c
# End Source File
# Begin Source File

SOURCE=.\report_altern.c
# End Source File
# Begin Source File

SOURCE=.\report_frame.c
# End Source File
# Begin Source File

SOURCE=.\report_init.c
# End Source File
# Begin Source File

SOURCE=.\report_out.c
# End Source File
# Begin Source File

SOURCE=.\report_setup.c
# End Source File
# Begin Source File

SOURCE=.\report_tool.c
# End Source File
# Begin Source File

SOURCE=.\return.c
# End Source File
# Begin Source File

SOURCE=.\river.c
# End Source File
# Begin Source File

SOURCE=.\rsw.c
# End Source File
# Begin Source File

SOURCE=.\rz_model.c
# End Source File
# Begin Source File

SOURCE=.\sample.c
# End Source File
# Begin Source File

SOURCE=.\scenario_tracking.c
# End Source File
# Begin Source File

SOURCE=.\spec_ext.c
# End Source File
# Begin Source File

SOURCE=.\species.c
# End Source File
# Begin Source File

SOURCE=.\spill.c
# End Source File
# Begin Source File

SOURCE=.\stock.c
# End Source File
# Begin Source File

SOURCE=.\strtools.c
# End Source File
# Begin Source File

SOURCE=.\summary.c
# End Source File
# Begin Source File

SOURCE=.\supersat.c
# End Source File
# Begin Source File

SOURCE=.\table.c
# End Source File
# Begin Source File

SOURCE=.\time.c
# End Source File
# Begin Source File

SOURCE=.\token.c
# End Source File
# Begin Source File

SOURCE=.\tokens_alt.c
# End Source File
# Begin Source File

SOURCE=.\tokens_archive.c
# End Source File
# Begin Source File

SOURCE=.\tokens_at.c
# End Source File
# Begin Source File

SOURCE=.\tokens_mindex.c
# End Source File
# Begin Source File

SOURCE=.\transport.c
# End Source File
# Begin Source File

SOURCE=.\transport_mort.c
# End Source File
# Begin Source File

SOURCE=.\transport_util.c
# End Source File
# Begin Source File

SOURCE=.\travel_table.c
# End Source File
# Begin Source File

SOURCE=.\tree.c
# End Source File
# Begin Source File

SOURCE=.\var.c
# End Source File
# Begin Source File

SOURCE=.\vectable.c
# End Source File
# Begin Source File

SOURCE=.\water_travel.c
# End Source File
# Begin Source File

SOURCE=.\writeutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\alt_report.h
# End Source File
# Begin Source File

SOURCE=.\altern_list.h
# End Source File
# Begin Source File

SOURCE=.\args.h
# End Source File
# Begin Source File

SOURCE=.\basin.h
# End Source File
# Begin Source File

SOURCE=.\batch.h
# End Source File
# Begin Source File

SOURCE=.\c1tokens.h
# End Source File
# Begin Source File

SOURCE=.\command.h
# End Source File
# Begin Source File

SOURCE=.\compute_gas_mort.h
# End Source File
# Begin Source File

SOURCE=.\crisp.h
# End Source File
# Begin Source File

SOURCE=.\crisp_defs.h
# End Source File
# Begin Source File

SOURCE=.\crisp_time.h
# End Source File
# Begin Source File

SOURCE=.\crisppp.h
# End Source File
# Begin Source File

SOURCE=.\custom_survival.h
# End Source File
# Begin Source File

SOURCE=.\dataio.h
# End Source File
# Begin Source File

SOURCE=.\date.h
# End Source File
# Begin Source File

SOURCE=.\daylight.h
# End Source File
# Begin Source File

SOURCE=.\defaults.h
# End Source File
# Begin Source File

SOURCE=.\delay.h
# End Source File
# Begin Source File

SOURCE=.\delayed_mort.h
# End Source File
# Begin Source File

SOURCE=.\dir.h
# End Source File
# Begin Source File

SOURCE=.\disp_defs.h
# End Source File
# Begin Source File

SOURCE=.\dump.h
# End Source File
# Begin Source File

SOURCE=.\eqns.h
# End Source File
# Begin Source File

SOURCE=.\exp_table.h
# End Source File
# Begin Source File

SOURCE=.\file.h
# End Source File
# Begin Source File

SOURCE=.\filerext.h
# End Source File
# Begin Source File

SOURCE=.\flow.h
# End Source File
# Begin Source File

SOURCE=.\fpe.h
# End Source File
# Begin Source File

SOURCE=.\gas_mort.h
# End Source File
# Begin Source File

SOURCE=.\gas_mort_params.h
# End Source File
# Begin Source File

SOURCE=.\gc_dist.h
# End Source File
# Begin Source File

SOURCE=.\gif.h
# End Source File
# Begin Source File

SOURCE=.\growth.h
# End Source File
# Begin Source File

SOURCE=.\gui.h
# End Source File
# Begin Source File

SOURCE=.\help.h
# End Source File
# Begin Source File

SOURCE=.\implicit.h
# End Source File
# Begin Source File

SOURCE=.\implicit_base.h
# End Source File
# Begin Source File

SOURCE=.\include.h
# End Source File
# Begin Source File

SOURCE=.\init.h
# End Source File
# Begin Source File

SOURCE=.\init_river.h
# End Source File
# Begin Source File

SOURCE=.\input_hist.h
# End Source File
# Begin Source File

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\main_app_inc.h
# End Source File
# Begin Source File

SOURCE=.\map.h
# End Source File
# Begin Source File

SOURCE=.\mapConstants.h
# End Source File
# Begin Source File

SOURCE=.\mapPoint.h
# End Source File
# Begin Source File

SOURCE=.\migration.h
# End Source File
# Begin Source File

SOURCE=.\mode.h
# End Source File
# Begin Source File

SOURCE=.\monte.h
# End Source File
# Begin Source File

SOURCE=.\monte_frame.h
# End Source File
# Begin Source File

SOURCE=.\monte_index.h
# End Source File
# Begin Source File

SOURCE=.\monte_output.h
# End Source File
# Begin Source File

SOURCE=.\mouse.h
# End Source File
# Begin Source File

SOURCE=.\mouse_tool_base.h
# End Source File
# Begin Source File

SOURCE=.\MouseToolDefines.h
# End Source File
# Begin Source File

SOURCE=.\nrutil_short.h
# End Source File
# Begin Source File

SOURCE=.\nsat.h
# End Source File
# Begin Source File

SOURCE=.\output_flags.h
# End Source File
# Begin Source File

SOURCE=.\parse.h
# End Source File
# Begin Source File

SOURCE=.\parse_alt.h
# End Source File
# Begin Source File

SOURCE=.\parser_file.h
# End Source File
# Begin Source File

SOURCE=.\parseutil.h
# End Source File
# Begin Source File

SOURCE=.\passage.h
# End Source File
# Begin Source File

SOURCE=.\pbn.h
# End Source File
# Begin Source File

SOURCE=.\period.h
# End Source File
# Begin Source File

SOURCE=.\phouse.h
# End Source File
# Begin Source File

SOURCE=.\pred.h
# End Source File
# Begin Source File

SOURCE=.\probability.h
# End Source File
# Begin Source File

SOURCE=.\qConstants.h
# End Source File
# Begin Source File

SOURCE=.\qDamOutputCBDMedBase.h
# End Source File
# Begin Source File

SOURCE=.\qDamReachGDT.h
# End Source File
# Begin Source File

SOURCE=.\qDatContainer.h
# End Source File
# Begin Source File

SOURCE=.\qDefines.h
# End Source File
# Begin Source File

SOURCE=.\qDialogMedEquationBase.h
# End Source File
# Begin Source File

SOURCE=.\qDialogMedTimeBase.h
# End Source File
# Begin Source File

SOURCE=.\qEditGraphGDT.h
# End Source File
# Begin Source File

SOURCE=.\qEditLine.h
# End Source File
# Begin Source File

SOURCE=.\qEditSliderBase.h
# End Source File
# Begin Source File

SOURCE=.\qEquationBase.h
# End Source File
# Begin Source File

SOURCE=.\qEquationCube.h
# End Source File
# Begin Source File

SOURCE=.\qEquationDelay0.h
# End Source File
# Begin Source File

SOURCE=.\qEquationDelay1.h
# End Source File
# Begin Source File

SOURCE=.\qEquationDelay3.h
# End Source File
# Begin Source File

SOURCE=.\qEquationDialogBase.h
# End Source File
# Begin Source File

SOURCE=.\qEquationExp0.h
# End Source File
# Begin Source File

SOURCE=.\qEquationExp1.h
# End Source File
# Begin Source File

SOURCE=.\qEquationExp2.h
# End Source File
# Begin Source File

SOURCE=.\qEquationExp3.h
# End Source File
# Begin Source File

SOURCE=.\qEquationFactory.h
# End Source File
# Begin Source File

SOURCE=.\qEquationFDens.h
# End Source File
# Begin Source File

SOURCE=.\qEquationFGE.h
# End Source File
# Begin Source File

SOURCE=.\qEquationFlAct.h
# End Source File
# Begin Source File

SOURCE=.\qEquationGasDist.h
# End Source File
# Begin Source File

SOURCE=.\qEquationGMort0.h
# End Source File
# Begin Source File

SOURCE=.\qEquationGMort1.h
# End Source File
# Begin Source File

SOURCE=.\qEquationGMort2.h
# End Source File
# Begin Source File

SOURCE=.\qEquationGMort3.h
# End Source File
# Begin Source File

SOURCE=.\qEquationLinear0.h
# End Source File
# Begin Source File

SOURCE=.\qEquationLinear1.h
# End Source File
# Begin Source File

SOURCE=.\qEquationLinear2.h
# End Source File
# Begin Source File

SOURCE=.\qEquationLinear3.h
# End Source File
# Begin Source File

SOURCE=.\qEquationLog0.h
# End Source File
# Begin Source File

SOURCE=.\qEquationMigration.h
# End Source File
# Begin Source File

SOURCE=.\qEquationNSat0.h
# End Source File
# Begin Source File

SOURCE=.\qEquationNSat1.h
# End Source File
# Begin Source File

SOURCE=.\qEquationNSat2.h
# End Source File
# Begin Source File

SOURCE=.\qEquationNSat4.h
# End Source File
# Begin Source File

SOURCE=.\qEquationNSat5.h
# End Source File
# Begin Source File

SOURCE=.\qEquationNSat6.h
# End Source File
# Begin Source File

SOURCE=.\qEquationPower0.h
# End Source File
# Begin Source File

SOURCE=.\qEquationPower1.h
# End Source File
# Begin Source File

SOURCE=.\qEquationPower2.h
# End Source File
# Begin Source File

SOURCE=.\qEquationPower3.h
# End Source File
# Begin Source File

SOURCE=.\qEquationPredTemp.h
# End Source File
# Begin Source File

SOURCE=.\qEquationSpill.h
# End Source File
# Begin Source File

SOURCE=.\qEquationTrans.h
# End Source File
# Begin Source File

SOURCE=.\qGraphConstants.h
# End Source File
# Begin Source File

SOURCE=.\qGraphDataTokenBuilder.h
# End Source File
# Begin Source File

SOURCE=.\qGraphDataTokenTypes.h
# End Source File
# Begin Source File

SOURCE=.\qGraphMedDrawAxisXI.h
# End Source File
# Begin Source File

SOURCE=.\qGraphMedDrawAxisYD.h
# End Source File
# Begin Source File

SOURCE=.\qGraphMedDrawLines.h
# End Source File
# Begin Source File

SOURCE=.\qGraphMedDrawText.h
# End Source File
# Begin Source File

SOURCE=.\qGraphMedTypes.h
# End Source File
# Begin Source File

SOURCE=.\qHeadwOutputCBDMedBase.h
# End Source File
# Begin Source File

SOURCE=.\qHistogramGDT.h
# End Source File
# Begin Source File

SOURCE=.\qIdcDamDelayEquation.h
# End Source File
# Begin Source File

SOURCE=.\qIdcEditGraph.h
# End Source File
# Begin Source File

SOURCE=.\qIdcEditGraphBase.h
# End Source File
# Begin Source File

SOURCE=.\qIdcEquationBase.h
# End Source File
# Begin Source File

SOURCE=.\qIdcFDensEquation.h
# End Source File
# Begin Source File

SOURCE=.\qIdcGasDistEquation.h
# End Source File
# Begin Source File

SOURCE=.\qIdcGasMortEquation.h
# End Source File
# Begin Source File

SOURCE=.\qIdcList.h
# End Source File
# Begin Source File

SOURCE=.\qIdcListBase.h
# End Source File
# Begin Source File

SOURCE=.\qIdcMigrationEquation.h
# End Source File
# Begin Source File

SOURCE=.\qIdcNSatEquation.h
# End Source File
# Begin Source File

SOURCE=.\qIdcPredatorProbability.h
# End Source File
# Begin Source File

SOURCE=.\qIdcPredTempEquation.h
# End Source File
# Begin Source File

SOURCE=.\qIdcSchedule.h
# End Source File
# Begin Source File

SOURCE=.\qIdcScheduleBase.h
# End Source File
# Begin Source File

SOURCE=.\qIdcSelect.h
# End Source File
# Begin Source File

SOURCE=.\qIdcSelectBase.h
# End Source File
# Begin Source File

SOURCE=.\qIdcSpillEquation.h
# End Source File
# Begin Source File

SOURCE=.\qIdcTimeBase.h
# End Source File
# Begin Source File

SOURCE=.\qIdcTransMortEquation.h
# End Source File
# Begin Source File

SOURCE=qInputReportTool.h
# End Source File
# Begin Source File

SOURCE=.\qIOHistGDT.h
# End Source File
# Begin Source File

SOURCE=.\qItem.h
# End Source File
# Begin Source File

SOURCE=.\qMapBitmaps.h
# End Source File
# Begin Source File

SOURCE=.\QmapController.h
# End Source File
# Begin Source File

SOURCE=.\qMapMedAnnouncements.h
# End Source File
# Begin Source File

SOURCE=.\qMapMedCrisp1.h
# End Source File
# Begin Source File

SOURCE=.\qMapMedGeography.h
# End Source File
# Begin Source File

SOURCE=.\qMapMedGridLines.h
# End Source File
# Begin Source File

SOURCE=.\qMapMedIcons.h
# End Source File
# Begin Source File

SOURCE=.\qMapMedTypes.h
# End Source File
# Begin Source File

SOURCE=.\QmapObject.h
# End Source File
# Begin Source File

SOURCE=qMediatorBuilder.h
# End Source File
# Begin Source File

SOURCE=.\qMercatorTransform.h
# End Source File
# Begin Source File

SOURCE=.\qMonteReportTool.h
# End Source File
# Begin Source File

SOURCE=.\qMouseToolRC.h
# End Source File
# Begin Source File

SOURCE=.\qPassHistGDT.h
# End Source File
# Begin Source File

SOURCE=.\QPointTransform.h
# End Source File
# Begin Source File

SOURCE=.\qPromptForString.h
# End Source File
# Begin Source File

SOURCE=.\qReachClassSDMed.h
# End Source File
# Begin Source File

SOURCE=.\qReachOutputCBDMedBase.h
# End Source File
# Begin Source File

SOURCE=.\qReleaseEGDT.h
# End Source File
# Begin Source File

SOURCE=.\qRowColumnBox.h
# End Source File
# Begin Source File

SOURCE=.\qScheduleDialogMed.h
# End Source File
# Begin Source File

SOURCE=.\qTableItem.h
# End Source File
# Begin Source File

SOURCE=.\qUtils.h
# End Source File
# Begin Source File

SOURCE=.\reach_gas_specs.h
# End Source File
# Begin Source File

SOURCE=.\realtime.h
# End Source File
# Begin Source File

SOURCE=.\realtime_defs.h
# End Source File
# Begin Source File

SOURCE=.\realtime_types.h
# End Source File
# Begin Source File

SOURCE=.\release.h
# End Source File
# Begin Source File

SOURCE=.\release_util.h
# End Source File
# Begin Source File

SOURCE=.\report.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resrc1.h
# End Source File
# Begin Source File

SOURCE=.\return.h
# End Source File
# Begin Source File

SOURCE=.\river.h
# End Source File
# Begin Source File

SOURCE=.\rsw.h
# End Source File
# Begin Source File

SOURCE=.\rtrr.h
# End Source File
# Begin Source File

SOURCE=.\run_mode.h
# End Source File
# Begin Source File

SOURCE=.\run_state.h
# End Source File
# Begin Source File

SOURCE=.\sample.h
# End Source File
# Begin Source File

SOURCE=.\schedule.h
# End Source File
# Begin Source File

SOURCE=.\smart_ptr.h
# End Source File
# Begin Source File

SOURCE=.\species.h
# End Source File
# Begin Source File

SOURCE=.\spill.h
# End Source File
# Begin Source File

SOURCE=.\stock.h
# End Source File
# Begin Source File

SOURCE=.\strtools.h
# End Source File
# Begin Source File

SOURCE=.\struct_at.h
# End Source File
# Begin Source File

SOURCE=.\table.h
# End Source File
# Begin Source File

SOURCE=.\token.h
# End Source File
# Begin Source File

SOURCE=.\tokens_alt.h
# End Source File
# Begin Source File

SOURCE=.\tokens_archive.h
# End Source File
# Begin Source File

SOURCE=.\tokens_at.h
# End Source File
# Begin Source File

SOURCE=.\tokens_mindex.h
# End Source File
# Begin Source File

SOURCE=.\transport.h
# End Source File
# Begin Source File

SOURCE=.\travel_table.h
# End Source File
# Begin Source File

SOURCE=.\tree.h
# End Source File
# Begin Source File

SOURCE=.\ttopt.h
# End Source File
# Begin Source File

SOURCE=.\unused.h
# End Source File
# Begin Source File

SOURCE=.\var.h
# End Source File
# Begin Source File

SOURCE=.\vectable.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# Begin Source File

SOURCE=.\writeutil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "UI Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AlternComparisonBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\AlternComparisonBase.ui
InputName=AlternComparisonBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\AlternComparisonBase.ui
InputName=AlternComparisonBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\InputReportToolBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\InputReportToolBase.ui
InputName=InputReportToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\InputReportToolBase.ui
InputName=InputReportToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IOToolBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\IOToolBase.ui
InputName=IOToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\IOToolBase.ui
InputName=IOToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MonteAnalToolBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\MonteAnalToolBase.ui
InputName=MonteAnalToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\MonteAnalToolBase.ui
InputName=MonteAnalToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MonteCarloBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\MonteCarloBase.ui
InputName=MonteCarloBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\MonteCarloBase.ui
InputName=MonteCarloBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MouseToolBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\MouseToolBase.ui
InputName=MouseToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\MouseToolBase.ui
InputName=MouseToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ReachClassNameBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\ReachClassNameBase.ui
InputName=ReachClassNameBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\ReachClassNameBase.ui
InputName=ReachClassNameBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ReleaseToolBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\ReleaseToolBase.ui
InputName=ReleaseToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\ReleaseToolBase.ui
InputName=ReleaseToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ScheduleBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\ScheduleBase.ui
InputName=ScheduleBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\ScheduleBase.ui
InputName=ScheduleBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ScheduleToolBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\ScheduleToolBase.ui
InputName=ScheduleToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\ScheduleToolBase.ui
InputName=ScheduleToolBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TransportBase.ui

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\TransportBase.ui
InputName=TransportBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - UICing $(InputPath)
InputDir=.
InputPath=.\TransportBase.ui
InputName=TransportBase

BuildCmds= \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).h" $(InputPath) \
	$(QTDIR)\bin\uic -o "$(InputDir)\$(InputName).cpp" -impl "$(InputDir)\$(InputName).h" $(InputPath) \
	

"$(InputDir)\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputDir)\$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "MOC Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=AlternComparisonBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=AlternComparisonBase.h
InputName=AlternComparisonBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=AlternComparisonBase.h
InputName=AlternComparisonBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=InputReportToolBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=InputReportToolBase.h
InputName=InputReportToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=InputReportToolBase.h
InputName=InputReportToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IOToolBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\IOToolBase.h
InputName=IOToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\IOToolBase.h
InputName=IOToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=MonteAnalToolBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=MonteAnalToolBase.h
InputName=MonteAnalToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=MonteAnalToolBase.h
InputName=MonteAnalToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=MonteCarloBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=MonteCarloBase.h
InputName=MonteCarloBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=MonteCarloBase.h
InputName=MonteCarloBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=MouseToolBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=MouseToolBase.h
InputName=MouseToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=MouseToolBase.h
InputName=MouseToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qCheckBoxDialogBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qCheckBoxDialogBase.h
InputName=qCheckBoxDialogBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qCheckBoxDialogBase.h
InputName=qCheckBoxDialogBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qCheckBoxDialogMedBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qCheckBoxDialogMedBase.h
InputName=qCheckBoxDialogMedBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qCheckBoxDialogMedBase.h
InputName=qCheckBoxDialogMedBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qCheckBoxGroup.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qCheckBoxGroup.h
InputName=qCheckBoxGroup

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qCheckBoxGroup.h
InputName=qCheckBoxGroup

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=Qcrisp.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=Qcrisp.h
InputName=Qcrisp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=Qcrisp.h
InputName=Qcrisp

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=QCStatusBar.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=QCStatusBar.h
InputName=QCStatusBar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=QCStatusBar.h
InputName=QCStatusBar

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qDataReportTool.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qDataReportTool.h
InputName=qDataReportTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qDataReportTool.h
InputName=qDataReportTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qDialogBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qDialogBase.h
InputName=qDialogBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qDialogBase.h
InputName=qDialogBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qDialogMedBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qDialogMedBase.h
InputName=qDialogMedBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qDialogMedBase.h
InputName=qDialogMedBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qEditGraphDataToken.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qEditGraphDataToken.h
InputName=qEditGraphDataToken

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qEditGraphDataToken.h
InputName=qEditGraphDataToken

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qEditGraphMediator.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qEditGraphMediator.h
InputName=qEditGraphMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qEditGraphMediator.h
InputName=qEditGraphMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qEditSliderDouble.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qEditSliderDouble.h
InputName=qEditSliderDouble

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qEditSliderDouble.h
InputName=qEditSliderDouble

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qEditSliderLong.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qEditSliderLong.h
InputName=qEditSliderLong

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qEditSliderLong.h
InputName=qEditSliderLong

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qFileManager.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qFileManager.h
InputName=qFileManager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qFileManager.h
InputName=qFileManager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qFocusEventFilter.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qFocusEventFilter.h
InputName=qFocusEventFilter

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qFocusEventFilter.h
InputName=qFocusEventFilter

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGlobalEvents.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGlobalEvents.h
InputName=qGlobalEvents

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGlobalEvents.h
InputName=qGlobalEvents

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGraphDataToken.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphDataToken.h
InputName=qGraphDataToken

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphDataToken.h
InputName=qGraphDataToken

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGraphDialog.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphDialog.h
InputName=qGraphDialog

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphDialog.h
InputName=qGraphDialog

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGraphMedDrawData.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphMedDrawData.h
InputName=qGraphMedDrawData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphMedDrawData.h
InputName=qGraphMedDrawData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGraphMediator.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphMediator.h
InputName=qGraphMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphMediator.h
InputName=qGraphMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGraphMedPosition.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphMedPosition.h
InputName=qGraphMedPosition

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphMedPosition.h
InputName=qGraphMedPosition

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGraphMedZoom.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphMedZoom.h
InputName=qGraphMedZoom

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphMedZoom.h
InputName=qGraphMedZoom

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGraphPane.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphPane.h
InputName=qGraphPane

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphPane.h
InputName=qGraphPane

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGraphSpeciesMediator.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphSpeciesMediator.h
InputName=qGraphSpeciesMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphSpeciesMediator.h
InputName=qGraphSpeciesMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGraphSubMediator.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphSubMediator.h
InputName=qGraphSubMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphSubMediator.h
InputName=qGraphSubMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qGraphUpdateMed.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphUpdateMed.h
InputName=qGraphUpdateMed

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qGraphUpdateMed.h
InputName=qGraphUpdateMed

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qHelpManager.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qHelpManager.h
InputName=qHelpManager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qHelpManager.h
InputName=qHelpManager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qHelpWindow.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qHelpWindow.h
InputName=qHelpWindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qHelpWindow.h
InputName=qHelpWindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qIdcData.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIdcData.h
InputName=qIdcData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qIdcFGEEquation.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qIdcFGEEquation.h
InputName=qIdcFGEEquation

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qIdcFGEEquation.h
InputName=qIdcFGEEquation

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qImConScroller.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qImConScroller.h
InputName=qImConScroller

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qImConScroller.h
InputName=qImConScroller

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qImpTabCtrl.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qImpTabCtrl.h
InputName=qImpTabCtrl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qImpTabCtrl.h
InputName=qImpTabCtrl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qInterfaceDataControl.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qInterfaceDataControl.h
InputName=qInterfaceDataControl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qInterfaceDataControl.h
InputName=qInterfaceDataControl

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qIOCell.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIOCell.h
InputName=qIOCell

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIOCell.h
InputName=qIOCell

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qIONotes.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIONotes.h
InputName=qIONotes

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIONotes.h
InputName=qIONotes

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qIOPostBonnevilleSheet.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIOPostBonnevilleSheet.h
InputName=qIOPostBonnevilleSheet

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIOPostBonnevilleSheet.h
InputName=qIOPostBonnevilleSheet

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qIOSheet.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIOSheet.h
InputName=qIOSheet

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIOSheet.h
InputName=qIOSheet

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qIOSIMPASSheet.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIOSIMPASSheet.h
InputName=qIOSIMPASSheet

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIOSIMPASSheet.h
InputName=qIOSIMPASSheet

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qIOTool.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIOTool.h
InputName=qIOTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qIOTool.h
InputName=qIOTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qListDialogBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qListDialogBase.h
InputName=qListDialogBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qListDialogBase.h
InputName=qListDialogBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qListDialogMedBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qListDialogMedBase.h
InputName=qListDialogMedBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qListDialogMedBase.h
InputName=qListDialogMedBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=QLogWindow.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=QLogWindow.h
InputName=QLogWindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=QLogWindow.h
InputName=QLogWindow

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Qmain.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\Qmain.h
InputName=Qmain

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\Qmain.h
InputName=Qmain

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMapMedDialogBuilder.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMedDialogBuilder.h
InputName=qMapMedDialogBuilder

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMedDialogBuilder.h
InputName=qMapMedDialogBuilder

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMapMediator.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMediator.h
InputName=qMapMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMediator.h
InputName=qMapMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMapMedSelector.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMedSelector.h
InputName=qMapMedSelector

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMedSelector.h
InputName=qMapMedSelector

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMapMedStatusBarUpdator.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMedStatusBarUpdator.h
InputName=qMapMedStatusBarUpdator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMedStatusBarUpdator.h
InputName=qMapMedStatusBarUpdator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMapMedUnZoom.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMedUnZoom.h
InputName=qMapMedUnZoom

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMedUnZoom.h
InputName=qMapMedUnZoom

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMapMedZoom.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMedZoom.h
InputName=qMapMedZoom

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapMedZoom.h
InputName=qMapMedZoom

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMapPane.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapPane.h
InputName=qMapPane

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapPane.h
InputName=qMapPane

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMapSubMediator.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapSubMediator.h
InputName=qMapSubMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMapSubMediator.h
InputName=qMapSubMediator

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMonteAltCmpDlg.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMonteAltCmpDlg.h
InputName=qMonteAltCmpDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMonteAltCmpDlg.h
InputName=qMonteAltCmpDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMonteAnalDlg.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMonteAnalDlg.h
InputName=qMonteAnalDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMonteAnalDlg.h
InputName=qMonteAnalDlg

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qMonteCarloManager.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qMonteCarloManager.h
InputName=qMonteCarloManager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qMonteCarloManager.h
InputName=qMonteCarloManager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMonteCarloTool.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMonteCarloTool.h
InputName=qMonteCarloTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMonteCarloTool.h
InputName=qMonteCarloTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qMouseTool.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMouseTool.h
InputName=qMouseTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qMouseTool.h
InputName=qMouseTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qReachClassCBDMed.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReachClassCBDMed.h
InputName=qReachClassCBDMed

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReachClassCBDMed.h
InputName=qReachClassCBDMed

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qReachClassEquationMed.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReachClassEquationMed.h
InputName=qReachClassEquationMed

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReachClassEquationMed.h
InputName=qReachClassEquationMed

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qReachClassTool.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReachClassTool.h
InputName=qReachClassTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReachClassTool.h
InputName=qReachClassTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qReleaseEGM.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReleaseEGM.h
InputName=qReleaseEGM

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReleaseEGM.h
InputName=qReleaseEGM

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qReleaseMedDrawData.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReleaseMedDrawData.h
InputName=qReleaseMedDrawData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReleaseMedDrawData.h
InputName=qReleaseMedDrawData

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qReleaseTool.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReleaseTool.h
InputName=qReleaseTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qReleaseTool.h
InputName=qReleaseTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qRunSettings.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qRunSettings.h
InputName=qRunSettings

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qRunSettings.h
InputName=qRunSettings

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\qScenarioManager.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qScenarioManager.h
InputName=qScenarioManager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=.\qScenarioManager.h
InputName=qScenarioManager

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qScheduleDialogBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qScheduleDialogBase.h
InputName=qScheduleDialogBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qScheduleDialogBase.h
InputName=qScheduleDialogBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qScheduleDialogMedBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qScheduleDialogMedBase.h
InputName=qScheduleDialogMedBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qScheduleDialogMedBase.h
InputName=qScheduleDialogMedBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qScheduleTool.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qScheduleTool.h
InputName=qScheduleTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qScheduleTool.h
InputName=qScheduleTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qSelectDialogBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qSelectDialogBase.h
InputName=qSelectDialogBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qSelectDialogBase.h
InputName=qSelectDialogBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qSelectDialogMedBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qSelectDialogMedBase.h
InputName=qSelectDialogMedBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qSelectDialogMedBase.h
InputName=qSelectDialogMedBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qTextOutDialog.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qTextOutDialog.h
InputName=qTextOutDialog

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qTextOutDialog.h
InputName=qTextOutDialog

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qTransportTool.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qTransportTool.h
InputName=qTransportTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qTransportTool.h
InputName=qTransportTool

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qZabelSlider.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qZabelSlider.h
InputName=qZabelSlider

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=qZabelSlider.h
InputName=qZabelSlider

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=ReachClassNameBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=ReachClassNameBase.h
InputName=ReachClassNameBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=ReachClassNameBase.h
InputName=ReachClassNameBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=ReleaseToolBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=ReleaseToolBase.h
InputName=ReleaseToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=ReleaseToolBase.h
InputName=ReleaseToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=ScheduleBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=ScheduleBase.h
InputName=ScheduleBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=ScheduleBase.h
InputName=ScheduleBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=ScheduleToolBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=ScheduleToolBase.h
InputName=ScheduleToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=ScheduleToolBase.h
InputName=ScheduleToolBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=TransportBase.h

!IF  "$(CFG)" == "notcrisp_gui - Win32 Release"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=TransportBase.h
InputName=TransportBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "notcrisp_gui - Win32 Debug"

# Begin Custom Build - MOCing $(InputPath)
InputDir=.
InputPath=TransportBase.h
InputName=TransportBase

"$(InputDir)\moc_$(InputName).cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc -o "$(InputDir)\moc_$(InputName).cpp" $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "MOC Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=moc_AlternComparisonBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_InputReportToolBase.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_IOToolBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_MonteAnalToolBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_MonteCarloBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_MouseToolBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_qCheckBoxDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_qCheckBoxDialogMedBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_qCheckBoxGroup.cpp
# End Source File
# Begin Source File

SOURCE=moc_Qcrisp.cpp
# End Source File
# Begin Source File

SOURCE=moc_QCStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=moc_qDataReportTool.cpp
# End Source File
# Begin Source File

SOURCE=moc_qDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_qDialogMedBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_qEditGraphDataToken.cpp
# End Source File
# Begin Source File

SOURCE=moc_qEditGraphMediator.cpp
# End Source File
# Begin Source File

SOURCE=moc_qEditSliderDouble.cpp
# End Source File
# Begin Source File

SOURCE=moc_qEditSliderLong.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_qFileManager.cpp
# End Source File
# Begin Source File

SOURCE=moc_qFocusEventFilter.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGlobalEvents.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGraphDataToken.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGraphDialog.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGraphMedDrawData.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGraphMediator.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGraphMedPosition.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGraphMedZoom.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGraphPane.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGraphSpeciesMediator.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGraphSubMediator.cpp
# End Source File
# Begin Source File

SOURCE=moc_qGraphUpdateMed.cpp
# End Source File
# Begin Source File

SOURCE=moc_qHelpManager.cpp
# End Source File
# Begin Source File

SOURCE=moc_qHelpWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_qIdcData.cpp
# End Source File
# Begin Source File

SOURCE=moc_qIdcFGEEquation.cpp
# End Source File
# Begin Source File

SOURCE=moc_qImConScroller.cpp
# End Source File
# Begin Source File

SOURCE=moc_qImpTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=moc_qInterfaceDataControl.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_qIOCell.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_qIONotes.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_qIOPostBonnevilleSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_qIOSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_qIOSIMPASSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_qIOTool.cpp
# End Source File
# Begin Source File

SOURCE=moc_qListDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_qListDialogMedBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_QLogWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_Qmain.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMapMedDialogBuilder.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMapMediator.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMapMedSelector.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMapMedStatusBarUpdator.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMapMedUnZoom.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMapMedZoom.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMapPane.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMapSubMediator.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMonteAltCmpDlg.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMonteAnalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_qMonteCarloManager.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMonteCarloTool.cpp
# End Source File
# Begin Source File

SOURCE=moc_qMouseTool.cpp
# End Source File
# Begin Source File

SOURCE=moc_qReachClassCBDMed.cpp
# End Source File
# Begin Source File

SOURCE=moc_qReachClassEquationMed.cpp
# End Source File
# Begin Source File

SOURCE=moc_qReachClassTool.cpp
# End Source File
# Begin Source File

SOURCE=moc_qReleaseEGM.cpp
# End Source File
# Begin Source File

SOURCE=moc_qReleaseMedDrawData.cpp
# End Source File
# Begin Source File

SOURCE=moc_qReleaseTool.cpp
# End Source File
# Begin Source File

SOURCE=moc_qRunSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\moc_qScenarioManager.cpp
# End Source File
# Begin Source File

SOURCE=moc_qScheduleDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_qScheduleDialogMedBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_qScheduleTool.cpp
# End Source File
# Begin Source File

SOURCE=moc_qSelectDialogBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_qSelectDialogMedBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_qTextOutDialog.cpp
# End Source File
# Begin Source File

SOURCE=moc_qTransportTool.cpp
# End Source File
# Begin Source File

SOURCE=moc_qZabelSlider.cpp
# End Source File
# Begin Source File

SOURCE=moc_ReachClassNameBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_ReleaseToolBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_ScheduleBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_ScheduleToolBase.cpp
# End Source File
# Begin Source File

SOURCE=moc_TransportBase.cpp
# End Source File
# End Group
# Begin Group "UI Implementations"

# PROP Default_Filter ""
# Begin Source File

SOURCE=AlternComparisonBase.cpp
# End Source File
# Begin Source File

SOURCE=InputReportToolBase.cpp
# End Source File
# Begin Source File

SOURCE=.\IOToolBase.cpp
# End Source File
# Begin Source File

SOURCE=MonteAnalToolBase.cpp
# End Source File
# Begin Source File

SOURCE=MonteCarloBase.cpp
# End Source File
# Begin Source File

SOURCE=MouseToolBase.cpp
# End Source File
# Begin Source File

SOURCE=ReachClassNameBase.cpp
# End Source File
# Begin Source File

SOURCE=ReleaseToolBase.cpp
# End Source File
# Begin Source File

SOURCE=ScheduleBase.cpp
# End Source File
# Begin Source File

SOURCE=ScheduleToolBase.cpp
# End Source File
# Begin Source File

SOURCE=TransportBase.cpp
# End Source File
# End Group
# End Target
# End Project
