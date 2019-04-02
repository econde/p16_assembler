; -- Example3.iss --
; Same as Example1.iss, but creates some registry entries too.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppPublisher=ISEL
AppName=P16 Assembler
AppVersion=1.1
DefaultDirName={userdesktop}\P16
DefaultGroupName=P16
UninstallDisplayIcon={app}\pas.exe
OutputDir=.
OutputBaseFilename=p16_setup
PrivilegesRequired=admin

[Files]
Source: "..\debugger\P16Debugger_v3_9_WinRelease_(bin)\*"; DestDir: "{app}\P16Debugger"
Source: "..\simulator\P16Simulator_v2_5_WinRelease_(bin)\*"; DestDir: "{app}\P16Simulator"
Source: "pas.exe"; DestDir: "{app}"

[Icons]
Name: "{group}\pas"; Filename: "{app}\pas.exe"

; NOTE: Most apps do not need registry entries to be pre-created. If you
; don't know what the registry is or if you need to use it, then chances are
; you don't need a [Registry] section.

[Registry]
; Start "Software\My Company\My Program" keys under HKEY_CURRENT_USER
; and HKEY_LOCAL_MACHINE. The flags tell it to always delete the
; "My Program" keys upon uninstall, and delete the "My Company" keys
; if there is nothing left in them.
Root: HKCU; Subkey: "Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"; Check: NeedsAddPath('{app}')

[Setup]
; Tell Windows Explorer to reload the environment
ChangesEnvironment=yes

[Code]

function NeedsAddPath(Param: string): boolean;
var
  OrigPath: string;
  ParamExpanded: string;
begin
  //expand the setup constants like {app} from Param
  ParamExpanded := ExpandConstant(Param);
  if not RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', OrigPath)
  then begin
    Result := True;
    exit;
  end;
  // look for the path with leading and trailing semicolon and with or without \ ending
  // Pos() returns 0 if not found
  Result := Pos(';' + UpperCase(ParamExpanded) + ';', ';' + UpperCase(OrigPath) + ';') = 0;  
  if Result = True then
     Result := Pos(';' + UpperCase(ParamExpanded) + '\;', ';' + UpperCase(OrigPath) + ';') = 0; 
end;
