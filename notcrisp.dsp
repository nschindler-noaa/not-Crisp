# Microsoft Developer Studio Project File - Name="notcrisp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=notcrisp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "notcrisp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "notcrisp.mak" CFG="notcrisp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "notcrisp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "notcrisp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "notcrisp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /Od /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "ROUTE_TRACKING" /D "NOMINMAX" /D "DOWNSTREAM" /D "NOVAR" /D "REALTIME" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Release/compassb.exe"

!ELSEIF  "$(CFG)" == "notcrisp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "ROUTE_TRACKING" /D "NOMINMAX" /D "DOWNSTREAM" /D "NOVAR" /D "REALTIME" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/compassb.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "notcrisp - Win32 Release"
# Name "notcrisp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
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

SOURCE=.\compassb.c
# End Source File
# Begin Source File

SOURCE=.\compute_gas_mort.c
# End Source File
# Begin Source File

SOURCE=.\crisp.c
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

SOURCE=.\extra_batch.c
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

SOURCE=.\include.c
# End Source File
# Begin Source File

SOURCE=.\init.c
# End Source File
# Begin Source File

SOURCE=.\init_river.c
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

SOURCE=.\custom_survival.h
# End Source File
# Begin Source File

SOURCE=.\dataio.h
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

SOURCE=.\growth.h
# End Source File
# Begin Source File

SOURCE=.\gui.h
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

SOURCE=.\list.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\migration.h
# End Source File
# Begin Source File

SOURCE=.\monte.h
# End Source File
# Begin Source File

SOURCE=.\monte_index.h
# End Source File
# Begin Source File

SOURCE=.\monte_output.h
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

SOURCE=.\reach_gas_specs.h
# End Source File
# Begin Source File

SOURCE=.\realtime.h
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

SOURCE=.\return.h
# End Source File
# Begin Source File

SOURCE=.\river.h
# End Source File
# Begin Source File

SOURCE=.\rsw.h
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

SOURCE=.\species.h
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
# End Target
# End Project
